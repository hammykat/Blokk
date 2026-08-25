#include "EngineClassData.hpp"


double ObjectManager::TimeEngineProcesses()
{
    // Get the start time
    auto TotalStartTime = chrono::steady_clock::now();

    // EngineProcesses-------------------------------------

    // Camera
    #ifdef Blokk_CamEnabled
        UpdateCamPosition();
    #endif

    { // User updates

        #ifdef Blokk_Diagnostics // Start time
            auto StartTime = chrono::steady_clock::now();
        #endif
        
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

        #ifdef Blokk_Diagnostics
            // End time
            auto EndTime = chrono::steady_clock::now();

            // Calculate total time
            UserUpdateTime = chrono::duration<double, milli>(EndTime - StartTime).count();
        #endif
    }


    { // Velocities 

        #ifdef Blokk_Diagnostics // Start time
            auto StartTime = chrono::steady_clock::now();
        #endif

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

        #ifdef Blokk_Diagnostics
            // End time
            auto EndTime = chrono::steady_clock::now();

            // Calculate total time
            VelocityTime = chrono::duration<double, milli>(EndTime - StartTime).count();
        #endif
    } 

    { // Animations

        { // Incrementing

            #ifdef Blokk_Diagnostics // Start time
                auto StartTime = chrono::steady_clock::now();
            #endif
            
            // Let the main thread increment
            IncrementFrames(FrameNums);

            #ifdef Blokk_Diagnostics
                // End time
                auto EndTime = chrono::steady_clock::now();

                // Calculate total time
                AnimationIncrementTime = chrono::duration<double, milli>(EndTime - StartTime).count();
            #endif
        }

        { // Visibility checks

            #ifdef Blokk_Diagnostics // Start time
                auto StartTime = chrono::steady_clock::now();
            #endif

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

            #ifdef Blokk_Diagnostics
                // End time
                auto EndTime = chrono::steady_clock::now();

                // Calculate total time
                VisibilityCullingTime = chrono::duration<double, milli>(EndTime - StartTime).count();
            #endif
        }
    }

    { // Render

        #ifdef Blokk_Diagnostics // Start time
            auto StartTime = chrono::steady_clock::now();
        #endif

        RenderObjects();

        #ifdef Blokk_Diagnostics
            // End time
            auto EndTime = chrono::steady_clock::now();

            // Calculate total time
            RenderTime = chrono::duration<double, milli>(EndTime - StartTime).count();
        #endif
    }


    // Get the end time
    auto TotalEndTime = chrono::steady_clock::now();

    // Get the total time
    auto TotalTime = chrono::duration<double, milli>(TotalEndTime - TotalStartTime).count();

    // Return
    return TotalTime;
}


// Main function
void ObjectManager::EngineProcess() 
{
    FrameExecutionTime = TimeEngineProcesses();
    
    if(!OptimalThreadCountReached)
    {
        if(OpenedThreads == ThreadCount) {
            OptimalThreadCountReached = true;
        } else {

        // If a thread was opnened previous frame
        if(ThreadOpenedPrevFrame)
        {
            // If took longer than previous frame
            if(FrameExecutionTime > PrevFrameTime)
            {
                // Destroy a thread
                DestroyThread();

                // Update vars
                ThreadDestroyedPrevFrame = true;
                ThreadOpenedPrevFrame = false;
            }
            // If took too long
            else if(FrameExecutionTime > TargetExecutionTime)
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
            if(FrameExecutionTime > TargetExecutionTime)
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