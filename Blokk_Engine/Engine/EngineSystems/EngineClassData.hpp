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

#include "GameTypes.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"
#include "Threading.hpp"

class ObjectManager;
class GameObject;

using namespace std;

struct IndexRange {
    size_t Start, End;

    int GetSize()
    {
        return End - Start;
    }
};



//  Object manager
class ObjectManager 
{

public:
    friend class GameObject;


public:
    SIMDLevel CoreCount;


    ObjectManager(size_t FPS) :
        
        // Threads
        ThreadOpenedPrevFrame(false),
        ThreadDestroyedPrevFrame(false),
        OptimalThreadCountReached(false),
        ThreadCount(std::thread::hardware_concurrency()),
        OpenedThreads(0),
        PrevOpenedThreads(0),

        // Frames
        FrameTime(1000.0 / FPS),
        CurrentFrameTime(0),
        PrevFrameTime(0),
        TargetFrameTime(FrameTime * 0.8),

        // SIMD
        SIMDRegisterLevel(DetectSIMD())
    {
        // Open starting thread
        OpenThread();
        
        // Set the worker's manager
        Worker::Manager = this;
    }

    void EngineProcess();

private:

    SIMDLevel SIMDRegisterLevel;

    // Positions
    vector<float> StaticXPositions;
    vector<float> StaticYPositions;

    vector<float> DynamicXPositions;
    vector<float> DynamicYPositions;

    // Velocities

    /* Static vels not stored as they're all 0
    vector<float> StaticXVelocities;
    vector<float> StaticYVelocities; */

    vector<float> XVelocities;
    vector<float> YVelocities;

    // Collisions
    vector<float> Right;
    vector<float> Left;
    vector<CollisionBoxType> StaticCollisionTypes;
    vector<CollisionBoxType> DynamicCollisionTypes;
    vector<CollisionHit> DynamicCollisions;
    vector<CollisionHit> StaticCollisions;
    
    // Animations
    vector<uint32_t> VisibleFrameNums;
    vector<uint32_t> InvisibleFrameNums;
    vector<uint32_t> VisibleAnimNums;
    vector<uint32_t> InVisibleAimNums;
    // Stores the animation names, points to an index in the animations
    unordered_map<string, uint32_t> AnimNames;
    // Stores a list of animations
    vector<vector<Texture2D>> Frames;
    vector<vector<uint32_t>> FrameWidths;
    vector<vector<uint32_t>> FrameHeights;

    // Pointers to object instances
    vector<GameObject*> ObjectInstances;

    // Index tracking 
    vector<uint32_t> ValidVisibleIdxs;
    vector<uint32_t> ValidVelIdxs;
    vector<uint32_t> ValidAnimIdxs;
    vector<uint32_t> ValidCollIdxs;

    // Update commands
    queue<FieldUpdate> FieldUpdateCommands;
    queue<DoubleFieldUpdate> DoubleFieldUpdateCommands;
    queue<ObjectCreationParams> Creations;

    queue<DynamicRegisterInfo> IntoDynamic;
    queue<uint32_t> IntoStatic;

    queue<uint32_t> IntoVisible;
    queue<uint32_t> FromVisible;

    // Rendering
    std::vector<RenderTypes> RenderTypes;
    std::vector<uint32_t> RenderObjects;

    // Ranges
    queue<Range> VelRanges;
    queue<Range> CollisionRanges;

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
    
    // 
    size_t ObjectCount;

    // Functions -------------------------------------------------

    // Split a number into x ranges
    vector<IndexRange> GetRanges(size_t Length, size_t Count)
    {
        if(Count == 0 || Length == 0) return {};
        size_t Size = Length / Count;

        vector<IndexRange> Result;
        Result.reserve(Count);

        for(size_t i = 0; i < Count; i++)
        {
            size_t Start = i * Size;
            size_t End = (i == Count - 1)? Length : (i + 1) * Size;
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

    // TODO: Complete this
    // Do the engine processes and time it
    double TimeEngineProcesses()
    {
        // Get the start time
        auto StartTime = chrono::steady_clock::now();

        // EngineProcesses-------------------------------------

        { // Velocities 

            // Get ranges
            vector<IndexRange> VelRanges = GetRanges(XVelocities.size(), OpenedThreads);
            // Set function
            Worker::CurrentJob = UpdateRangeOfPositions;
            // Loop through
            for(size_t i = 0; i < OpenedThreads; i++)
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
                IncrementFrames(VisibleFrameNums);
                IncrementFrames(InvisibleFrameNums);
            }

            { // Visibility checks


            }
        }


        // Get the end time
        auto EndTime = chrono::steady_clock::now();

        // Get the total time
        auto TotalTime = chrono::duration<double, milli>(EndTime - StartTime).count();

        // Return
        return TotalTime;
    }

    // POSITIONS W/ VELS -------------------------------------------------------------------

    void UpdateRangeOfPositions(IndexRange TRange);

    void UpdatePositions(
        float* PosX, float* PosY, 
        const float* VelX, const float *VelY, 
        size_t Size
    );

    // Rendering --------------------------------------------------------

    void GetRenderObjects()
    {
        for(auto&& A : RenderObjects)
        {

        }
    }

    void RenderObjects()
    {
        
    }

    // Visibility checks --------------------------------------

    void CheckVisible(
        vector<float>& XPositions,
        vector<float>& YPositions,
        uint32_t Size
    )
    {

    }

    // SIMD helpers --------------------



    // Updates ----------------------------------------------------------------

    void ProcessFieldUpdateCommand(FieldUpdate Command);

    void ProcessAddCommands(ObjectCreationParams Fields);

    void ProcessDoubleUpdateCommands(DoubleFieldUpdate Command);

    // Frame increment------------------------------

    void IncrementFrames(
        vector<uint32_t>& FrameNums
    );

    // Animations ----------------------------------------------------------


};
