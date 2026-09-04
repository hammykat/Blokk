#pragma once

#include <cstddef>
#include <cstdint>
#include <queue>
#include <thread>
#include <immintrin.h>
#include <intrin.h>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include <SDL3/SDL.h>

// Get correct default values
#ifndef Blokk_Visibility_CullType
    #define Blokk_Visibility_CullType 1 // Axis culling
#endif

#ifndef Blokk_Thread_Control
    #ifndef Blokk_Thread_AdaptiveTiming
        #define Blokk_Thread_AdaptiveTiming
    #endif
#endif

#ifndef Blokk_Thread_AdaptiveTiming
    #define Blokk_Thread_FixedCount
#endif

#include "GameTypes.hpp"
#include "ObjectUpdateStructs.hpp"
#include "SIMD_Finder.hpp"

namespace Blokk
{

struct IndexRange
{
    uint32_t Start, End;

    uint32_t GetSize() const
    {
        return End - Start;
    }
};

    class ObjectManager;
    class GameObject;
    class Renderer;

    using CheckVisibleRangeFnPtr =
        void(ObjectManager::*)(IndexRange Range, Worker* Thread);

    using UpdatePositionsFnPtr =
        void(ObjectManager::*)(
            float* PosX, float* PosY,
            const float* VelX, const float* VelY,
            uint32_t Size
        );

    using TimeEngineProcessesFnPtr =
        double(ObjectManager::*)();
}

#include "Threading.hpp"

namespace Blokk
{
    enum BlokkCulling
    {
        Axis,
        Basic
    };

    struct EngineDiagnostics
    {
        uint32_t TotalThreadCount, OpenedThreadCount;
    };

    struct ManagerCreation
    {
        Vector2 ScreenDimensions;
        uint32_t FPS = 30;
    };

    // Object manager
    class ObjectManager
    {
    public:
        friend class GameObject;
        friend class Renderer;

        ObjectManager(ManagerCreation Cr) :

            // Threads
            ThreadOpenedPrevFrame(false),
            ThreadDestroyedPrevFrame(false),
            OptimalThreadCountReached(false),
            ThreadCount(std::thread::hardware_concurrency()),
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

                #ifdef Blokk_Rendering_Enabled
                    RenderTime(0),
                #endif

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

            // Set the worker's manager
            Worker::Manager = this;

            // Open starting thread
            OpenThread();

            // Get the proper functions
            GetFunctions();
        }

        void EngineProcess();

        #ifdef Blokk_Thread_FixedCount
        void SetThreadCount(uint32_t Count);
        #endif

        // Diagnostics -----------------------------------

        #ifdef Blokk_Diagnostics

        uint32_t GetTotalObjects() const
        {
            return ObjectCount;
        }

        uint32_t GetStaticObjectCount() const
        {
            return StaticObjectCount;
        }

        uint32_t GetDynamicObjectCount() const
        {
            return DynamicObjectCount;
        }

        uint32_t GetOpenedThreads() const
        {
            return OpenedThreads;
        }

        uint32_t GetTotalThreads() const
        {
            return ThreadCount;
        }

        double GetFrameExecutionTime() const
        {
            return FrameExecutionTime;
        }

        double GetTargetExecutionTime() const
        {
            return TargetExecutionTime;
        }

        uint32_t GetFPS() const
        {
            return FPS;
        }

        double GetUserUpdateTime() const
        {
            return UserUpdateTime;
        }

        double GetVelocityTime() const
        {
            return VelocityTime;
        }

        double GetAnimationIncrementTime() const
        {
            return AnimationIncrementTime;
        }

        double GetVisibilityCullingTime() const
        {
            return VisibilityCullingTime;
        }

        #ifdef Blokk_Rendering_Enabled
        double GetRenderTime() const
        {
            return RenderTime;
        }
        #endif

        void SetTargetExecutionTime(double Time)
        {
            // Make sure time isn't negative
            if (Time <= 0)
            {
                throw std::invalid_argument(
                    "Blokk error: Tried to set frame time to negative value or 0."
                );
            }

            TargetExecutionTime = Time;
        }

        uint32_t GetSIMDRegisterSize() const
        {
            switch (SIMDRegisterLevel)
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

        std::string GetSIMDRegisterType() const
        {
            switch (SIMDRegisterLevel)
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
            std::cout << "Optimal thread count reached: "
                      << (OptimalThreadCountReached ? "true" : "false")
                      << "\n\n";

            std::cout << "Object Counts ---------------" << '\n';
            std::cout << "Total object count: " << ObjectCount << '\n';
            std::cout << "Dynamic object count: " << DynamicObjectCount << '\n';
            std::cout << "Static object count: " << StaticObjectCount << "\n\n";

            std::cout << "SIMD Register ---------------" << '\n';
            std::cout << "SIMD register type: "
                      << GetSIMDRegisterType() << '\n';
            std::cout << "SIMD register size: "
                      << GetSIMDRegisterSize() << " bits" << '\n';

            std::cout << "System timing ---------------" << '\n';
            std::cout << "User update processing: "
                      << UserUpdateTime << '\n';
            std::cout << "Updating positions with velocities: "
                      << VelocityTime << '\n';
            std::cout << "Animation frame# incrementing: "
                      << AnimationIncrementTime << '\n';
            std::cout << "Visibility culling: "
                      << VisibilityCullingTime << '\n';

            #ifdef Blokk_Rendering_Enabled
            std::cout << "Rendering: "
                      << RenderTime << '\n';
            #endif

            std::cout << "END ==================================" << '\n';
        }

        #endif

        // Camera functions
        #ifdef Blokk_CamEnabled

        CameraController Camera;

        class CameraController
        {
        public:
            void SetPosition(int32_t X, int32_t Y)
            {
                CameraX = X;
                CameraY = Y;
            }

            void SetPosition(Vector2 Pos)
            {
                CameraX = Pos.x;
                CameraY = Pos.y;
            }

            void SetXPosition(int32_t X)
            {
                CameraX = X;
            }

            void SetYPosition(int32_t Y)
            {
                CameraY = Y;
            }

            void ChangeXPosition(int32_t X)
            {
                CameraX += X;
            }

            void ChangeYPosition(int32_t Y)
            {
                CameraY += Y;
            }
        };

        #endif

        #ifdef Blokk_Thread_Control

        void OpenThread()
        {
            if(OpenedThreads == ThreadCount)
            {
                throw std::out_of_range(
                    "Blokk error: Cannot create another thread because the maximum thread count has been reached."
                );
            }

            Workers.emplace_back(std::make_unique<Worker>());
            Workers.back()->Awake();

            OpenedThreads++;
        }

        void DestroyThread()
        {
            if(OpenedThreads == 1)
            {
                throw std::out_of_range(
                    "Blokk error: Cannot destroy another thread because the minimum thread count has been reached (1)."
                );
            }

            Workers.back()->Stop();
            Workers.pop_back();

            OpenedThreads--;
        }

        #endif

    // PRIVATE -------------------------------------------
    private:

        SIMDLevel SIMDRegisterLevel;
        uint32_t ScreenHeight;
        uint32_t ScreenWidth;

        // Camera
        #ifdef Blokk_CamEnabled

        int32_t CameraX;
        int32_t CameraY;

        CameraController Camera;

        #endif

        // Positions
        std::vector<float> XPositions;
        std::vector<float> YPositions;

        // Velocities
        std::vector<float> XVelocities;
        std::vector<float> YVelocities;

        // Collisions
        std::vector<float> Right;
        std::vector<float> Left;
        std::vector<CollisionHit> Collisions;

        // Animations
        #ifdef Blokk_Animations_Enabled

        std::vector<uint32_t> FrameNums;
        std::vector<uint32_t> AnimNums;

        // Stores the animation names, points to an index in the animations
        std::unordered_map<std::string, uint32_t> AnimNames;

        // Stores a list of animations
        std::vector<std::vector<SDL_Texture*>> Frames;
        std::vector<std::vector<uint32_t>> FrameWidths;
        std::vector<std::vector<uint32_t>> FrameHeights;
        std::vector<uint32_t> AnimFrameCounts;

        // Visibility
        std::vector<uint32_t> AnimHeights;
        std::vector<uint32_t> AnimWidths;

        std::vector<uint8_t> IsVisible;

        #endif

        // Pointers to object instances
        std::vector<GameObject*> ObjectInstances;

        // Updating velocity stuff
        std::queue<uint32_t> IntoStatic;
        std::queue<uint32_t> IntoDynamic;

        // Rendering
        std::vector<uint32_t> RenderObjectIdxs;

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

        std::vector<std::unique_ptr<Worker>> Workers;

        #ifdef Blokk_Diagnostics

        double UserUpdateTime;
        double VelocityTime;
        double AnimationIncrementTime;
        double VisibilityCullingTime;

            #ifdef Blokk_Rendering_Enabled
                double RenderTime;
            #endif

        #endif

        // Counts
        uint32_t ObjectCount;
        uint32_t StaticObjectCount;
        uint32_t DynamicObjectCount;

        // Functions -------------------------------------------------

        // Get the right function implementations according to the user's SIMD
        void GetFunctions()
        {
            switch (SIMDRegisterLevel)
            {
                // 256 bit
                case SIMDLevel::AVX2:

                    #if (Blokk_Visibility_CullType == 0)
                        CheckVisibleRange =
                            CheckVisibilityFn_Basic<SIMDLevel::AVX2>;
                    #elif (Blokk_Visibility_CullType == 1)
                        CheckVisibleRange =
                            CheckVisibilityFn_Axis<SIMDLevel::AVX2>;
                    #endif

                    UpdatePositions =
                        UpdatePositionsFn<SIMDLevel::AVX2>;

                    break;

                // 512 bit
                case SIMDLevel::AVX512:

                    #if (Blokk_Visibility_CullType == 0)
                        CheckVisibleRange =
                            CheckVisibilityFn_Basic<SIMDLevel::AVX512>;
                    #elif (Blokk_Visibility_CullType == 1)
                        CheckVisibleRange =
                            CheckVisibilityFn_Axis<SIMDLevel::AVX512>;
                    #endif

                    UpdatePositions =
                        UpdatePositionsFn<SIMDLevel::AVX512>;

                    break;

                // 128 bit - default
                default:

                    #if (Blokk_Visibility_CullType == 0)
                        CheckVisibleRange =
                            CheckVisibilityFn_Basic<SIMDLevel::SSE2>;
                    #elif (Blokk_Visibility_CullType == 1)
                        CheckVisibleRange =
                            CheckVisibilityFn_Axis<SIMDLevel::SSE2>;
                    #endif

                    UpdatePositions =
                        UpdatePositionsFn<SIMDLevel::SSE2>;
            }
        }

        // Split a number into x ranges
        std::vector<IndexRange> GetRanges(
            uint32_t Length,
            uint32_t Count
        )
        {
            if (Count == 0 || Length == 0)
                return {};

            uint32_t NewCount = std::min(Length, Count);

            uint32_t Size = Length / NewCount;

            std::vector<IndexRange> Result;
            Result.reserve(NewCount);

            for (uint32_t i = 0; i < NewCount; i++)
            {
                uint32_t Start = i * Size;
                uint32_t End =
                    (i == NewCount - 1)
                    ? Length
                    : (i + 1) * Size;

                Result.push_back(
                    IndexRange{Start, End}
                );
            }

            return Result;
        }

        #ifndef Blokk_Thread_Control

        void OpenThread()
        {
            Workers.emplace_back(std::make_unique<Worker>());
            Workers.back()->Awake();

            OpenedThreads++;
        }

        void DestroyThread()
        {
            Workers.back()->Stop();
            Workers.pop_back();

            OpenedThreads--;
        }

        #endif

        // Time engine processes -------------------------------------

        // Do the engine processes and time it
        double TimeEngineProcesses();

        // Swap 2 objects
        void SwapStaticObjects(uint32_t Obj1, uint32_t Obj2);
        void SwapDynamicObjects(uint32_t Obj1, uint32_t Obj2);

        // Destroy an object
        void DestroyDynamicObject(uint32_t ObjIdx);
        void DestroyStaticObject(uint32_t ObjIdx);

        // POSITIONS W/ VELS -----------------------------------------

        void UpdateRangeOfPositions(
            IndexRange TRange,
            Worker* Thread
        );

        UpdatePositionsFnPtr UpdatePositions;

        template <SIMDLevel Level>
        void UpdatePositionsFn(
            float* PosX,
            float* PosY,
            const float* VelX,
            const float* VelY,
            uint32_t Size
        );

        // Visibility checks ------------------------------------------

        #if (Blokk_Visibility_CullType == 0)

            template <SIMDLevel Level>
            void CheckVisibilityFn_Basic(
                IndexRange Range,
                Worker* Thread
            );

        #elif (Blokk_Visibility_CullType == 1)

            template <SIMDLevel Level>
            void CheckVisibilityFn_Axis(
                IndexRange Range,
                Worker* Thread
            );

        #endif

        CheckVisibleRangeFnPtr CheckVisibleRange;

        // Adding objects ----------------------------------------------------

        void ProcessAddCommand(ObjectCreationParams Fields);
        void ProcessAddCommand(Vector2 Velocity, Vector2 Position, GameObject* Object, bool Visible);

        // Frame increment --------------------------------------------

        void IncrementFrames(
            std::vector<uint32_t>& FrameNums
        );
    };
}