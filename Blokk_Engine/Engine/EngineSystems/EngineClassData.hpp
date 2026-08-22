#pragma once

#include <cstddef>
#include <queue>
#include <thread>
#include <immintrin.h>
#include <intrin.h>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <stdexcept>

#include "GameTypes.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"

class ObjectManager;
class GameObject;

using namespace std;

struct IndexRange {
    uint32_t Start, End;

    uint32_t GetSize()
    {
        return End - Start;
    }
};

#include "Threading.hpp"

/*
Engine stores all data for each fields in one vector,
keeps track of static object count and dynamic object count
and then keeps data in sync (Splits static and Dynamic objects)
*/

using CheckVisibleRangeFnPtr = void(ObjectManager::*)(
    IndexRange Range, Worker* Thread);

using UpdatePositionsFnPtr = void(ObjectManager::*)(
    float* PosX, float* PosY, 
    const float* VelX, const float *VelY, 
    uint32_t Size
);

enum BlokkCulling {
    Axis,
    Basic
};

struct ManagerCreation 
{
    BlokkCulling CullingType;
    Vector2 ScreenDimensions;
    uint32_t FPS = 30;
};

//  Object manager
class ObjectManager 
{

public:
    friend class GameObject;

    ObjectManager(ManagerCreation Cr) :
        
        // Threads
        ThreadOpenedPrevFrame(false),
        ThreadDestroyedPrevFrame(false),
        OptimalThreadCountReached(false),
        ThreadCount(thread::hardware_concurrency()),
        OpenedThreads(0),
        PrevOpenedThreads(0),

        // Frames
        FrameTime(1000.0 / Cr.FPS),
        CurrentFrameTime(0),
        PrevFrameTime(0),
        TargetFrameTime(FrameTime * 0.8),

        // SIMD
        SIMDRegisterLevel(DetectSIMD()),

        // Screen
        ScreenWidth(Cr.ScreenDimensions.x),
        ScreenHeight(Cr.ScreenDimensions.y),

        // Counts
        ObjectCount(0),
        StaticObjectCount(0),
        DynamicObjectCount(0)
    {
        // Throw an error if unsupported
        if (SIMDRegisterLevel == SIMDLevel::Unsupported)
        {
            throw std::runtime_error(
                "Blokk requires an x86 CPU with SSE2 support."
            );
        }

        // Throw an error if no threads were found
        if (ThreadCount == 0)
        {
            throw std::runtime_error(
                "Blokk couldn't find the hardware thread count."
            );
        }

        // Open starting thread
        OpenThread();
        
        // Set the worker's manager
        Worker::Manager = this;

        // Get the proper functions 
        GetFunctions(Cr.CullingType);
    }

    void EngineProcess();

private:

    SIMDLevel SIMDRegisterLevel;
    uint32_t ScreenHeight;
    uint32_t ScreenWidth;

    // Positions
    vector<float> XPositions;
    vector<float> YPositions;

    // Velocities
    vector<float> XVelocities;
    vector<float> YVelocities;

    // Collisions
    vector<float> Right;
    vector<float> Left;
    vector<CollisionBoxType> CollisionTypes;
    vector<CollisionHit> Collisions;
    
    // Animations
    vector<uint32_t> FrameNums;
    vector<uint32_t> AnimNums;
    // Stores the animation names, points to an index in the animations
    unordered_map<string, uint32_t> AnimNames;
    // Stores a list of animations
    vector<vector<Texture2D>> Frames;
    vector<vector<uint32_t>> FrameWidths;
    vector<vector<uint32_t>> FrameHeights;
    vector<uint32_t> AnimFrameCounts;
    
    // Visibility
    vector<uint32_t> AnimHeights;
    vector<uint32_t> AnimWidths;

    // Pointers to object instances
    vector<GameObject*> ObjectInstances;

    // Update commands
    queue<FieldUpdate<float>> FieldUpdateCommands;
    queue<DoubleFieldUpdate<float>> DoubleFieldUpdateCommands;
    queue<ObjectCreationParams> Creations;

    queue<DynamicRegisterInfo> IntoDynamic;
    queue<uint32_t> IntoStatic;

    queue<FieldUpdate<bool>> BoolUpdates;
    queue<FieldUpdate<uint32_t>> UIntUpdates;

    // Rendering
    vector<uint32_t> RenderObjectIdxs;

    // Workers
    uint32_t ThreadCount;
    uint32_t OpenedThreads;
    uint32_t PrevOpenedThreads;

    bool ThreadOpenedPrevFrame;
    bool ThreadDestroyedPrevFrame;
    bool OptimalThreadCountReached;

    double CurrentFrameTime;
    double PrevFrameTime;
    double TargetFrameTime;
    double FrameTime;
    vector<unique_ptr<Worker>> Workers;
    
    // Counts
    uint32_t ObjectCount;
    uint32_t StaticObjectCount;
    uint32_t DynamicObjectCount;

    // Functions -------------------------------------------------

    // Get the right function implementations according to the user's SIMD
    void GetFunctions(BlokkCulling CullType)
    {
        switch(SIMDRegisterLevel)
        {
            // 256 bit
            case SIMDLevel::AVX2:
            
                if (CullType == BlokkCulling::Basic) {
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::AVX2>;
                } else if (CullType == BlokkCulling::Axis) {
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::AVX2>;
                }

                UpdatePositions = UpdatePositionsFn<SIMDLevel::AVX2>;
                break;
            
            // 512 bit
            case SIMDLevel::AVX512:

                if (CullType == BlokkCulling::Basic) {
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::AVX512>;
                } else if (CullType == BlokkCulling::Axis) {
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::AVX512>;
                }
                UpdatePositions = UpdatePositionsFn<SIMDLevel::AVX512>;
                break;

            // 128 bit - default
            default:
                if (CullType == BlokkCulling::Basic) {
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::SSE2>;
                } else if (CullType == BlokkCulling::Axis) {
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::SSE2>;
                }
                UpdatePositions = UpdatePositionsFn<SIMDLevel::SSE2>;
        }
    }

    // Split a number into x ranges
    vector<IndexRange> GetRanges(uint32_t Length, uint32_t Count)
    {
        if(Count == 0 || Length == 0) return {};
        uint32_t Size = Length / Count;

        vector<IndexRange> Result;
        Result.reserve(Count);

        for(uint32_t i = 0; i < Count; i++)
        {
            uint32_t Start = i * Size;
            uint32_t End = (i == Count - 1)? Length : (i + 1) * Size;
            Result.push_back(
                IndexRange{Start, End}
            );
        }

        return Result;
    }

    void OpenThread()
    {
        Workers.emplace_back(make_unique<Worker>());
        Workers.back()->Awake();

        OpenedThreads++;
    }

    void DestroyThread()
    {
        Workers.back()->Stop();
        Workers.pop_back();

        OpenedThreads--;
    }

    // Do the engine processes and time it
    double TimeEngineProcesses()
    {
        // Get the start time
        auto StartTime = chrono::steady_clock::now();

        // EngineProcesses-------------------------------------

        { // User updates
            
            // Process single update commands
            while(!FieldUpdateCommands.empty())
            {
                // Process front command
                ProcessFieldUpdateCommand(FieldUpdateCommands.front());
            }

            // Process double update commands
            while(!DoubleFieldUpdateCommands.empty())
            {
                // Process front command
                ProcessDoubleUpdateCommand(DoubleFieldUpdateCommands.front());
            }
        }

        { // Velocities 

            // Get ranges
            vector<IndexRange> VelRanges = GetRanges(DynamicObjectCount, OpenedThreads);
            // Set function
            Worker::CurrentJob = UpdateRangeOfPositions;
            // Loop through
            for(uint32_t i = 0; i < OpenedThreads; i++)
            {
                // Give it a range
                Workers[i]->SetRange(VelRanges[i]);
            }
            // Wait for each to finish
            for (auto& Worker : Workers) {
                Worker->WaitUntilFinished();
            }
        } 

        { // Animations

            { // Incrementing
                
                // Let the main thread increment
                IncrementFrames(FrameNums);
            }

            { // Visibility checks
                // Get ranges
                vector<IndexRange> VisRanges = GetRanges(ObjectCount, OpenedThreads);
                // Set function
                Worker::CurrentJob = CheckVisibleRange;
                // Loop through
                for(uint32_t i = 0; i < OpenedThreads; i++)
                {
                    // Give it a range
                    Workers[i]->SetRange(VisRanges[i]);
                }

                // Wait for each to finish
                for (auto& Worker : Workers) {
                    Worker->WaitUntilFinished();
                }

                // Clear idxs
                RenderObjectIdxs.clear();

                // Loop through workers
                for (auto& Worker : Workers)
                {
                    // Get the idxs
                    RenderObjectIdxs.insert(
                        RenderObjectIdxs.end(),
                        Worker->IdxResult.begin(),
                        Worker->IdxResult.end()
                    );
                }
            }
        }

        { // Render
            RenderObjects();
        }


        // Get the end time
        auto EndTime = chrono::steady_clock::now();

        // Get the total time
        auto TotalTime = chrono::duration<double, milli>(EndTime - StartTime).count();

        // Return
        return TotalTime;
    }

    void SwapObjects(uint32_t Obj1, uint32_t Obj2);

    // POSITIONS W/ VELS -------------------------------------------------------------------

    void UpdateRangeOfPositions(IndexRange TRange, Worker* Thread);

    UpdatePositionsFnPtr UpdatePositions;

    template <SIMDLevel Level>
    void UpdatePositionsFn(
        float* PosX, float* PosY, 
        const float* VelX, const float *VelY, 
        uint32_t Size
    );

    // Rendering --------------------------------------------------------

    void RenderObjects()
    {
        for(auto Idx : RenderObjectIdxs)
        {
            uint32_t Anim = AnimNums[Idx];
            uint32_t FrameCount = AnimFrameCounts[Anim];

            if (FrameCount == 0)
                continue;

            uint32_t Frame = FrameNums[Idx] % FrameCount;
            Texture2D Texture = Frames[Anim][Frame];
            
            DrawTexture(Texture, XPositions[Idx], YPositions[Idx], WHITE);
        }
    }

    // Visibility checks --------------------------------------

    template <SIMDLevel Level>
    void CheckVisibilityFn_Basic(IndexRange Range, Worker* Thread);

    template <SIMDLevel Level>
    void CheckVisibilityFn_Axis(IndexRange Range, Worker* Thread);

    CheckVisibleRangeFnPtr CheckVisibleRange;

    // Updates ----------------------------------------------------------------

    template <ConfiguredUpdateType T>
    void ProcessFieldUpdateCommand(FieldUpdate<T> Command);

    void ProcessAddCommands(ObjectCreationParams Fields);

    void ProcessDoubleUpdateCommand(DoubleFieldUpdate<float> Command);

    // Frame increment------------------------------

    void IncrementFrames(
        vector<uint32_t>& FrameNums
    );

    // Animations ----------------------------------------------------------

    void CreateNewEmptyAnimation(string Name);

    void CreateAnimation(string Name, vector<Texture2D>& Frames);

    void AddFramesToAnimation(string Name, vector<Texture2D>& Frames);

};
