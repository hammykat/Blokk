#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ObjectManager;
class Worker;


using WorkerJobFunction =
    void (ObjectManager::*)(IndexRange, Worker*);


// Thread
class Worker
{
public:

    ~Worker()
    {
        Stop();
    }

    void Run()
    {
        while (Running)
        {
            std::unique_lock<std::mutex> Lock(Mutex);

            CV.wait(Lock, [this]()
            {
                return HasWork || !Running;
            });

            if (!Running)
                break;

            IndexRange RangeToProcess = TargetRange;
            HasWork = false;

            Lock.unlock();

            // Same function + same manager for everyone
            (Manager->*CurrentJob)(RangeToProcess, this);

            // Tell manager we're finished
            {
                std::lock_guard<std::mutex> Lock(Mutex);
                Finished = true;
            }

            FinishedCV.notify_one();
        }
    }

    void SetRange(IndexRange Range)
    {
        lock_guard<std::mutex> Lock(Mutex);
        TargetRange = Range;
        HasWork = true;
        Finished = false;

        CV.notify_one();
    }

    bool IsFinished()
    {
        lock_guard<std::mutex> Lock(Mutex);
        return Finished;
    }

    void Wake()
    {
        CV.notify_one();
    }

    void Awake()
    {
        Thread = std::thread(&Worker::Run, this);
    }

    void Stop()
    {
        Running = false;
        CV.notify_one();

        if (Thread.joinable())
            Thread.join();
    }

    void WaitUntilFinished()
    {
        unique_lock<mutex> Lock(Mutex);

        FinishedCV.wait(Lock, [this]()
        {
            return Finished;
        });
    }

    inline static ObjectManager* Manager = nullptr;
    inline static WorkerJobFunction CurrentJob = nullptr;
    std::atomic<bool> Running = true;

    vector<uint32_t> IdxResult;

private:

    IndexRange TargetRange;

    std::thread Thread;
    bool HasWork = false;
    bool Finished = true;
    condition_variable FinishedCV;

    std::mutex Mutex;
    condition_variable CV;
};
