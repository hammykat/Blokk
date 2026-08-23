#include "EngineClassData.hpp"

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