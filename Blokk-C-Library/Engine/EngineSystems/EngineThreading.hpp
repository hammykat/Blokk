#include "EngineClassData.hpp"

using WorkerJobFunction =
    void (ObjectManager::* )(IndexRange);


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
            unique_lock<mutex> Lock(Mutex);

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
            (Manager->*CurrentJob)(RangeToProcess);

            // Tell manager we're finished
            {
                lock_guard<mutex> Lock(Mutex);
                Finished = true;
            }

            FinishedCV.notify_one();
        }
    }

    void SetRange(IndexRange Range)
    {
        lock_guard<mutex> Lock(Mutex);
        TargetRange = Range;
        HasWork = true;
        Finished = false;

        CV.notify_one();
    }

    bool IsFinished()
    {
        lock_guard<mutex> Lock(Mutex);
        return Finished;
    }

    void Wake()
    {
        CV.notify_one();
    }

    void Awake()
    {
        Thread = thread(&Worker::Run, this);
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
    atomic<bool> Running = true;

private:

    IndexRange TargetRange;

    thread Thread;
    bool HasWork = false;
    bool Finished = true;
    condition_variable FinishedCV;

    mutex Mutex;
    condition_variable CV;
};

// Engine threading -----------------------------------------------

