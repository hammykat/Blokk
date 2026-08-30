#include "EngineClassData.hpp"
#include <sstream>
#include <algorithm>

namespace Blokk
{

double ObjectManager::TimeEngineProcesses()
{
    // Get the total start time
    auto TotalStartTime = std::chrono::steady_clock::now();

    // EngineProcesses-----------------------------------------

    // Camera
    #ifdef Blokk_CamEnabled
        UpdateCamPosition();
    #endif

    { // User updates

        #ifdef Blokk_Diagnostics
            auto StartTime = std::chrono::steady_clock::now();
        #endif

        // Process object creation commands
        while (!Creations.empty())
        {
            ProcessAddCommand(Creations.front());
            Creations.pop();
        }

        // Process single update commands
        while (!FieldUpdateCommands.empty())
        {
            ProcessFieldUpdateCommand(FieldUpdateCommands.front());
            FieldUpdateCommands.pop();
        }

        // Process double update commands
        while (!DoubleFieldUpdateCommands.empty())
        {
            ProcessDoubleUpdateCommand(DoubleFieldUpdateCommands.front());
            DoubleFieldUpdateCommands.pop();
        }

        #ifdef Blokk_Diagnostics
            auto EndTime = std::chrono::steady_clock::now();

            UserUpdateTime =
                std::chrono::duration<double, std::milli>(
                    EndTime - StartTime
                ).count();
        #endif
    }

    { // Velocities

        #ifdef Blokk_Diagnostics
            auto StartTime = std::chrono::steady_clock::now();
        #endif

        // Get ranges
        std::vector<IndexRange> VelRanges =
            GetRanges(DynamicObjectCount, OpenedThreads);

        // Set function
        Worker::CurrentJob = UpdateRangeOfPositions;

        // Loop through
        uint32_t Count = std::min(OpenedThreads, static_cast<uint32_t>(VelRanges.size()));
        for (uint32_t i = 0; i < Count; i++)
        {
            Workers[i]->SetRange(VelRanges[i]);
        }

        // Wait for each to finish
        for (auto& Worker : Workers)
        {
            Worker->WaitUntilFinished();
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

        { // Incrementing

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

        { // Visibility checks

            #ifdef Blokk_Diagnostics
                auto StartTime = std::chrono::steady_clock::now();
            #endif

            // Get ranges
            std::vector<IndexRange> VisRanges =
                GetRanges(ObjectCount, OpenedThreads);

            // Set function
            Worker::CurrentJob = CheckVisibleRange;

            // Loop through
            for (uint32_t i = 0; i < OpenedThreads; i++)
            {
                Workers[i]->SetRange(VisRanges[i]);
            }

            // Wait for each to finish
            for (auto& Worker : Workers)
            {
                Worker->WaitUntilFinished();
            }

            // Clear idxs
            RenderObjectIdxs.clear();

            // Loop through workers
            for (auto& Worker : Workers)
            {
                RenderObjectIdxs.insert(
                    RenderObjectIdxs.end(),
                    Worker->IdxResult.begin(),
                    Worker->IdxResult.end()
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


double ObjectManager::TimeRenderObjects()
{
    // Start time
    auto StartTime = std::chrono::steady_clock::now();

    RenderObjects();

    // End time
    auto EndTime = std::chrono::steady_clock::now();

    // Calculate total time
    auto TotalTime =
        std::chrono::duration<double, std::milli>(
            EndTime - StartTime
        ).count();

    #ifdef Blokk_Diagnostics
        RenderTime = TotalTime;
    #endif

    return TotalTime;
}

#ifdef Blokk_Thread_AdaptiveTiming

// Main function
void ObjectManager::EngineProcess()
{
    FrameExecutionTime = TimeEngineProcesses();

    if (!OptimalThreadCountReached)
    {
        if (OpenedThreads == ThreadCount)
        {
            OptimalThreadCountReached = true;
        }
        else
        {
            // If a thread was opened previous frame
            if (ThreadOpenedPrevFrame)
            {
                // If took longer than previous frame
                if (FrameExecutionTime > PrevFrameTime)
                {
                    // Destroy a thread
                    DestroyThread();

                    // Update vars
                    ThreadOpenedPrevFrame = false;
                    OptimalThreadCountReached = true;
                }

                // If took too long
                else if (FrameExecutionTime > TargetExecutionTime)
                {
                    // Open another thread
                    OpenThread();

                    // Update var
                    ThreadOpenedPrevFrame = true;
                }
            }
            else // If a thread wasn't opened the previous frame
            {
                // If it took too long
                if (FrameExecutionTime > TargetExecutionTime)
                {
                    // Open another thread
                    OpenThread();

                    // Update var
                    ThreadOpenedPrevFrame = true;
                }
            }
        }

        PrevFrameTime = FrameExecutionTime;
    }
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
    else if (Count > ThreadCount)
    {
        throw std::invalid_argument(
            "Blokk error: Requested thread count exceeds hardware thread count."
        );
    }

    // If need to destroy threads
    if(Count < OpenedThreads)
    {
        // Loop 
        for(uint32_t i = 0; i < OpenedThreads - Count; i++) 
        {
            // Destroy
            DestroyThread();
        }
    }
    else // If need to create threads
    {
        // Loop
        for(uint32_t i = 0; i < Count - OpenedThreads; i++)
        {
            // Open
            OpenThread();
        }
    }
}

#endif

}