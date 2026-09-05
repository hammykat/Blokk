#include "EngineClassData.hpp"
#include <sstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <stdexcept>

namespace Blokk
{

double ObjectManager::TimeEngineProcesses()
{
    // Get the total start time
    auto TotalStartTime = std::chrono::steady_clock::now();

    // EngineProcesses-----------------------------------------

    { // Velocities

        #ifdef Blokk_Diagnostics
            auto StartTime = std::chrono::steady_clock::now();
        #endif

        // Get ranges
        std::vector<IndexRange> VelRanges =
            GetRanges(DynamicObjectCount, UsedThreads);

        // Set function
        Worker::CurrentJob = UpdateRangeOfPositions;

        // Loop through
        uint32_t Count = VelRanges.size();
        for (uint32_t i = 0; i < Count; i++)
        {
            Workers[i]->SetRange(VelRanges[i]);
        }

        #ifdef Blokk_Rendering_Enabled

        { // Animation Frame Incrementing 
            // (Done while velocities are being done by other threads)

            #ifdef Blokk_Diagnostics
                auto StartTime = std::chrono::steady_clock::now();
            #endif

            // Let the main thread increment
            IncrementFrames(FrameNums);

            #ifdef Blokk_Diagnostics
                auto EndTime = std::chrono::steady_clock::now();

                AnimationIncrementTime =
                    std::chrono::duration<double, std::milli>(
                        EndTime - StartTime
                    ).count();
            #endif
        }

        #endif

        // Wait for each to finish
        for (uint32_t i = 0; i < Count; i++)
        {
            Workers[i]->WaitUntilFinished();
        }

        #ifdef Blokk_Diagnostics
            auto EndTime = std::chrono::steady_clock::now();

            VelocityTime =
                std::chrono::duration<double, std::milli>(
                    EndTime - StartTime
                ).count();
        #endif
    }

    { // Animations

        { // Visibility checks

            #ifdef Blokk_Diagnostics
                auto StartTime = std::chrono::steady_clock::now();
            #endif

            // Get ranges
            std::vector<IndexRange> VisRanges =
                GetRanges(ObjectCount, UsedThreads);

            // Set function
            Worker::CurrentJob = CheckVisibleRange;

            // Loop
            uint32_t Count = VisRanges.size();
            for (uint32_t i = 0; i < Count; i++)
            {
                Workers[i]->SetRange(VisRanges[i]);
            }

            // Wait for each to finish
            for (uint32_t i = 0; i < Count; i++)
            {
                Workers[i]->WaitUntilFinished();
            }

            // Clear idxs
            RenderObjectIdxs.clear();

            // Loop through workers
            for (uint32_t i = 0; i < Count; i++)
            {
                RenderObjectIdxs.insert(
                    RenderObjectIdxs.end(),
                    Workers[i]->IdxResult.begin(),
                    Workers[i]->IdxResult.end()
                );
            }

            #ifdef Blokk_Diagnostics
                auto EndTime = std::chrono::steady_clock::now();

                VisibilityCullingTime =
                    std::chrono::duration<double, std::milli>(
                        EndTime - StartTime
                    ).count();
            #endif
        }
    }

    // Get the end time
    auto TotalEndTime = std::chrono::steady_clock::now();

    // Get the total time
    auto TotalTime =
        std::chrono::duration<double, std::milli>(
            TotalEndTime - TotalStartTime
        ).count();

    return TotalTime;
}

#ifdef Blokk_Thread_AdaptiveTiming

// Main function
void ObjectManager::EngineProcess()
{
    FrameExecutionTime = TimeEngineProcesses();

    if (!AdaptiveThreadingEnabled)
    {
        // If a thread was opened previous frame
        if (ThreadOpenedPrevFrame)
        {
            // If took longer than previous frame
            if (FrameExecutionTime > PrevFrameTime)
            {
                // Stop using a thread if can
                if(UsedThreads != 1) {
                    UsedThreads--;
                }
                
                // Update vars
                ThreadOpenedPrevFrame = false;
            }

            // If took too long
            else if (FrameExecutionTime > TargetExecutionTime)
            {
                // Use another thread if can
                if(UsedThreads != OpenedThreads) 
                {
                    UsedThreads++;

                    // Update var
                    ThreadOpenedPrevFrame = true;
                } 
                
                // Do nothing if else
            }

            // If took a good time
            else 
            {
                ThreadOpenedPrevFrame = false;
            }
        }
        else // If a thread wasn't opened the previous frame
        {
            // If it took too long
            if (FrameExecutionTime > TargetExecutionTime)
            {
                // Use another thread if can
                if(UsedThreads != OpenedThreads) 
                {
                    UsedThreads++;

                    // Update var
                    ThreadOpenedPrevFrame = true;
                }
            }
        }

        PrevFrameTime = FrameExecutionTime;
    }
}

void ObjectManager::StopAdaptiveThreadingSystem() {
    AdaptiveThreadingEnabled = false;
}

void ObjectManager::EnableAdaptiveThreadingSystem() {
    AdaptiveThreadingEnabled = true;
}

#elif defined(Blokk_Thread_FixedCount)

// Main function
void ObjectManager::EngineProcess()
{
    TimeEngineProcesses();
}

void ObjectManager::SetThreadCount(uint32_t Count)
{
    // Safety
    if(Count == 0)
    {
        throw std::invalid_argument(
            "Blokk error: Thread count must be at least 1."
        );
    }
    else if (Count > ThreadCount) // ThreadCount represents max capacity/hardware limits
    {
        throw std::invalid_argument(
            "Blokk error: Requested thread count exceeds hardware thread count."
        );
    }

    // Slide active boundaries smoothly instead of allocating/deallocating OS memory
    UsedThreads = Count;
    OpenedThreads = Count; 
}

#endif

}