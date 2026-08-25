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
#include <iostream>

// Get correct default values
#ifndef Blokk_Visibility_CullType
    #define Blokk_Visibility_CullType 1 // Axis culling 
#endif

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

using TimeEngineProcessesFnPtr = double(ObjectManager::*)();

enum BlokkCulling {
    Axis,
    Basic
};

struct EngineDiagnostics 
{
    uint32_t TotalThreadCount, OpenedThreadCount;
};

struct ManagerCreation 
{
    BlokkCulling CullingType;
    Vector2 ScreenDimensions;
    uint32_t FPS = 30;
    bool Debug = false;
    bool CameraEnabled = false;
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
        FPS(Cr.FPS),
        FrameTime(1000.0 / Cr.FPS),
        FrameExecutionTime(0),
        PrevFrameTime(0),
        TargetExecutionTime(FrameTime * 0.8),

        // Diagnostics timing
        #ifdef Blokk_Diagnostics
        UserUpdateTime(0),
        VelocityTime(0),
        AnimationIncrementTime(0),
        VisibilityCullingTime(0),
        RenderTime(0),
        #endif

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
        GetFunctions();
    }

    void EngineProcess();

    // Diagnostics -----------------------------------
    #ifdef Blokk_Diagnostics

    uint32_t GetTotalObjects() {
        return ObjectCount;
    }

    uint32_t GetStaticObjectCount() {
        return StaticObjectCount;
    }
    
    uint32_t GetDynamicObjectCount() {
        return DynamicObjectCount;
    }

    uint32_t GetOpenedThreads() {
        return OpenedThreads;
    }

    uint32_t GetTotalThreads() {
        return ThreadCount;
    }

    double GetFrameExecutionTime() {
        return FrameExecutionTime;
    }

    double GetTargetExecutionTime() {
        return TargetExecutionTime;
    }

    uint32_t GetFPS() {
        return FPS;
    }

    double GetUserUpdateTime() const {
        return UserUpdateTime;
    }

    double GetVelocityTime() const {
        return VelocityTime;
    }

    double GetAnimationIncrementTime() const {
        return AnimationIncrementTime;
    }

    double GetVisibilityCullingTime() const {
        return VisibilityCullingTime;
    }

    double GetRenderTime() const {
        return RenderTime;
    }

    void SetTargetExecutionTime(double Time) 
    {
        // Make sure time isn't negative
        if(Time <= 0) {
            throw std::invalid_argument("Blokk error: Tried to set frame time to negative value or 0.");
        }

        // Set
        TargetExecutionTime = Time;
    }

    uint32_t GetSIMDRegisterSize() {
        switch(SIMDRegisterLevel)
        {
            case SIMDLevel::AVX2:
                return 256;

            case SIMDLevel::AVX512:
                return 512;
            
            case SIMDLevel::SSE2:
                return 128;

            default:
                return 0;
        }
    }

    string GetSIMDRegisterType()
    {
        switch(SIMDRegisterLevel)
        {
            case SIMDLevel::AVX2:
                return "avx2";

            case SIMDLevel::AVX512:
                return "avx512";
            
            case SIMDLevel::SSE2:
                return "sse2";

            default:
                return "unknown";
        }
    }

    void PrintDiagnostics() 
    {
        std::cout << "BLOKK Diagnostics report ==============" << "\n\n";

        std::cout << "Frames ---------------" << '\n';
        std::cout << "FPS: " << FPS << '\n';
        std::cout << "Total frame time: " << FrameTime << '\n';
        std::cout << "FrameExecutionTime: " << FrameExecutionTime << '\n';
        std::cout << "TargetExecutionTime: " << TargetExecutionTime << "\n\n";

        std::cout << "Threads ---------------" << '\n';
        std::cout << "Opened Threads: " << OpenedThreads << '\n';
        std::cout << "Total Threads: " << ThreadCount << '\n';
        std::cout << "Optimal thread count reached: " << (OptimalThreadCountReached? "true" : "false") << "\n\n";

        std::cout << "Object Counts ---------------" << '\n';
        std::cout << "Total object count: " << ObjectCount << '\n';
        std::cout << "Dynamic object count: " << DynamicObjectCount << '\n';
        std::cout << "Static object count: " << StaticObjectCount << "\n\n";

        std::cout << "SIMD Register ---------------" << '\n';
        std::cout << "SIMD register type: " << GetSIMDRegisterType() << '\n';
        std::cout << "SIMD register size: " << GetSIMDRegisterSize() << " bits" << '\n';

        std::cout << "System timing ---------------" << '\n';
        std::cout << "User update processing: " << UserUpdateTime << '\n';
        std::cout << "Updating positions with velocities: " << VelocityTime << '\n';
        std::cout << "Animation frame# incrementing: " << AnimationIncrementTime << '\n';
        std::cout << "Visibility culling: " << VisibilityCullingTime << '\n';
        std::cout << "Rendering: " << RenderTime << '\n';

        std::cout << "END ==================================" << '\n';
    }

    #endif

    // Camera functions
    #ifdef Blokk_CamEnabled
    class Camera {
        void SetPosition(int32_t X, int32_t Y)
        {
            CameraPosition.x = X;
            CameraPosition.y = Y;
        }

        void SetPosition(Vector2 Pos)
        {
            CameraPosition = Pos;
        }

        void SetXPosition(int32_t X) {
            CameraPosition.x = X;
        }

        void SetYPosition(int32_t Y) {
            CameraPosition.y = Y;
        }

        
        void ChangeYPosition(int32_t Y) {
            CameraPosition += Y;
        }
    }
    #endif

// PRIVATE -------------------------------------------
private:

    SIMDLevel SIMDRegisterLevel;
    uint32_t ScreenHeight;
    uint32_t ScreenWidth;

    // Camera
    #ifdef Blokk_CamEnabled
    Vector2 CameraPosition;
    Vector2 CameraVelocity;
    #endif

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

    double FrameExecutionTime;
    double PrevFrameTime;
    double TargetExecutionTime;
    double FrameTime;
    uint32_t FPS;
    vector<unique_ptr<Worker>> Workers;

    #ifdef Blokk_Diagnostics
    double UserUpdateTime;
    double VelocityTime;
    double AnimationIncrementTime;
    double VisibilityCullingTime;
    double RenderTime;
    #endif
    
    // Counts
    uint32_t ObjectCount;
    uint32_t StaticObjectCount;
    uint32_t DynamicObjectCount;

    // Functions -------------------------------------------------

    // Get the right function implementations according to the user's SIMD
    void GetFunctions()
    {
        switch(SIMDRegisterLevel)
        {
            // 256 bit
            case SIMDLevel::AVX2:

                #if (Blokk_Visibility_CullType == 0)
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::AVX2>;
                #elif (Blokk_Visibility_CullType == 1)
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::AVX2>;
                #endif

                UpdatePositions = UpdatePositionsFn<SIMDLevel::AVX2>;
                break;
            
            // 512 bit
            case SIMDLevel::AVX512:

                #if (Blokk_Visibility_CullType == 0)
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::AVX512>;
                #elif (Blokk_Visibility_CullType == 1)
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::AVX512>;
                #endif

                UpdatePositions = UpdatePositionsFn<SIMDLevel::AVX512>;
                break;

            // 128 bit - default
            default:
                #if (Blokk_Visibility_CullType == 0)
                    CheckVisibleRange = CheckVisibilityFn_Basic<SIMDLevel::SSE2>;
                #elif (Blokk_Visibility_CullType == 1)
                    CheckVisibleRange = CheckVisibilityFn_Axis<SIMDLevel::SSE2>;
                #endif
                
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

    // Time engine processes -------------------------------------------------

    double TimeEngineProcesses();

    // Do the engine processes and time it
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

    // Camera
    #ifdef Blokk_CamEnabled
    void UpdateCamPosition()
    {
        CameraPosition.x += CameraVelocity.x;
        CameraPosition.y += CameraVelocity.y;
    }
    #endif

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

    #if (Blokk_Visibility_CullType == 0)
        template <SIMDLevel Level>
        void CheckVisibilityFn_Basic(IndexRange Range, Worker* Thread);
    #elif (Blokk_Visibility_CullType == 1)
        template <SIMDLevel Level>
        void CheckVisibilityFn_Axis(IndexRange Range, Worker* Thread);
    #endif

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
