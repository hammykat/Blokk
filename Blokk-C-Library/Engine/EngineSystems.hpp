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





//  Object manager
class ObjectManager 
{

public:
    friend class GameObject;


public:
    SIMDLevel CoreCount;

    void EngineProcess() 
    {
        CurrentFrameTime = TimeEngineProcesses();
        
        if(!OptimalThreadCountReached)
        {
            // If a thread was opnened previous frame
            if(ThreadOpenedPrevFrame)
            {
                // If took longer than previous frame
                if(CurrentFrameTime > PrevFrameTime)
                {
                    // Destroy a thread
                    DestroyThread();

                    // Update vars
                    ThreadDestroyedPrevFrame = true;
                    ThreadOpenedPrevFrame = false;
                }
                // If took too long
                else if(CurrentFrameTime > TargetFrameTime)
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
                if(CurrentFrameTime > TargetFrameTime)
                {
                    // Open another thread
                    OpenThread();

                    // Update var
                    ThreadOpenedPrevFrame = true;
                }
            }

            PrevFrameTime = CurrentFrameTime;
        }
    }

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
    vector<uint32_t> VisibleCurrentAnimNums;
    vector<uint32_t> InVisibleCurrentAimNums;
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

    // Update a range of positions
    void UpdateRangeOfPositions(IndexRange TRange)
    {
        float *XPos = &DynamicXPositions[TRange.Start];
        float *YPos = &DynamicYPositions[TRange.Start];
        float *XVels = &XVelocities[TRange.Start];
        float *YVels = &YVelocities[TRange.Start];

        size_t Size = TRange.GetSize();
        UpdatePositions(XPos, YPos, XVels, YVels, Size);
    }

    void UpdatePositions(
        float* PosX, float* PosY, 
        const float* VelX, const float *VelY, 
        size_t Size
    ) {
        switch(SIMDRegisterLevel) 
        {
            // 256 bit
            case SIMDLevel::AVX:
            case SIMDLevel::AVX2:
                ProcessVelocities_SIMD_AVX2(PosX, PosY, VelX, VelY, Size);
                break;

            // 512 bit
            case SIMDLevel::AVX512:
                ProcessVelocities_SIMD_AVX512(PosX, PosY, VelX, VelY, Size);
                break;

            // 128 bit
            default:
                ProcessVelocities_SIMD_SSE2(PosX, PosY, VelX, VelY, Size);
        }
        
    }

    // Helpers for SIMD velocities ---------------------------

    // AXV / AXV2 (256 bit - 8 floats) - 8 at a time
    void ProcessVelocities_SIMD_AVX2(
        float* PosX, float* PosY, 
        const float* VelX, const float* VelY, 
        size_t Size
    ) {
        // Loop
        size_t i = 0;
        for(; i + 8 <= Size; i += 8)
        {
            // Positions
            __m256 XPos = _mm256_loadu_ps(&PosX[i]);
            __m256 YPos = _mm256_loadu_ps(&PosY[i]);
                    
            // Velocities
            __m256 XVel = _mm256_loadu_ps(&VelX[i]);
            __m256 YVel = _mm256_loadu_ps(&VelY[i]);

            // Add them
            XPos = _mm256_add_ps(XPos, XVel);
            YPos = _mm256_add_ps(YPos, YVel);

            // Store back
            _mm256_storeu_ps(&PosX[i], XPos);
            _mm256_storeu_ps(&PosY[i], YPos);
        }

        // Make sure to do the last ones
            
        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            PosX[i] += VelX[i];
                
            // Update y
            PosY[i] += VelY[i];
        }
    }



    // AXV512 (512 bit, 16 floats) - 16 at a time
    void ProcessVelocities_SIMD_AVX512(
        float* PosX, float* PosY, 
        const float* VelX, const float* VelY, 
        size_t Size
    ) {
        // Loop
        size_t i = 0;
        for(; i + 16 <= Size; i += 16)
        {
            // Positions
            __m512 XPos = _mm512_loadu_ps(&PosX[i]);
            __m512 YPos = _mm512_loadu_ps(&PosY[i]);
                    
            // Velocities
            __m512 XVel = _mm512_loadu_ps(&VelX[i]);
            __m512 YVel = _mm512_loadu_ps(&VelY[i]);

            // Add them
            XPos = _mm512_add_ps(XPos, XVel);
            YPos = _mm512_add_ps(YPos, YVel);

            // Store back
            _mm512_storeu_ps(&PosX[i], XPos);
            _mm512_storeu_ps(&PosY[i], YPos);
        }

        // Make sure to do the last ones
            
        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            PosX[i] += VelX[i];
                
            // Update y
            PosY[i] += VelY[i];
        }
    }



    // SSE2 (128 bit, 4 floats) - 4 at a time
    void ProcessVelocities_SIMD_SSE2(
        float* PosX, float* PosY, 
        const float* VelX, const float* VelY, 
        size_t Size
    ) {
        // Loop
        size_t i = 0;
        for(; i + 4 <= Size; i += 4)
        {
            // Positions
            __m128 XPos = _mm_loadu_ps(&PosX[i]);
            __m128 YPos = _mm_loadu_ps(&PosY[i]);
                    
            // Velocities
            __m128 XVel = _mm_loadu_ps(&VelX[i]);
            __m128 YVel = _mm_loadu_ps(&VelY[i]);

            // Add them
            XPos = _mm_add_ps(XPos, XVel);
            YPos = _mm_add_ps(YPos, YVel);

            // Store back
            _mm_storeu_ps(&PosX[i], XPos);
            _mm_storeu_ps(&PosY[i], YPos);
        }

        // Make sure to do the last ones
            
        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            PosX[i] += VelX[i];
                
            // Update y
            PosY[i] += VelY[i];
        }
    }

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

    void ProcessFieldUpdateCommand(FieldUpdate Command)
    {
        switch(Command.Type) 
        {
            case CommandTypes::Destroy:
                
                break;

            case CommandTypes::Set:
                (*Command.Vector)[Command.Idx] = Command.Value;
                break;

            case CommandTypes::Subtract:
                (*Command.Vector)[Command.Idx] -= Command.Value;
                break;

            case CommandTypes::Add:
                (*Command.Vector)[Command.Idx] += Command.Value;
                break;

            case CommandTypes::Multiply:
                (*Command.Vector)[Command.Idx] *= Command.Value;
                break;

            case CommandTypes::Divide:
                (*Command.Vector)[Command.Idx] /= Command.Value;
                
        }
    }

    // TODO: Finish
    void ProcessAddCommands()
    {

    }

    // TODO: Finish
    void ProcessDoubleUpdateCommands(DoubleFieldUpdate Command)
    {
        switch(Command.Type) 
        {
            case CommandTypes::Set:
                (*Command.XVector)[Command.Idx] = Command.XValue;
                (*Command.YVector)[Command.Idx] = Command.YValue;
                break;

            case CommandTypes::Subtract:
                (*Command.XVector)[Command.Idx] -= Command.XValue;
                (*Command.YVector)[Command.Idx] -= Command.YValue;
                break;

            case CommandTypes::Add:
                (*Command.XVector)[Command.Idx] += Command.XValue;
                (*Command.YVector)[Command.Idx] += Command.YValue;
                break;

            case CommandTypes::Multiply:
                (*Command.XVector)[Command.Idx] *= Command.XValue;
                (*Command.YVector)[Command.Idx] *= Command.YValue;
                break;

            case CommandTypes::Divide:
                (*Command.XVector)[Command.Idx] /= Command.XValue;
                (*Command.YVector)[Command.Idx] /= Command.YValue;
                break;
                
            default:
                break;
        }
    }

    // Animations ----------------------------------------------------------

    void IncrementFrames(
        vector<uint32_t>& FrameNums
    ) {
        size_t Size = FrameNums.size();
        switch(SIMDRegisterLevel)
        {
            // 256 bit
            case SIMDLevel::AVX:
            case SIMDLevel::AVX2:
                IncrementFrames_SIMD_AVX2(FrameNums, Size);
                break;

            // 512 bit
            case SIMDLevel::AVX512:
                IncrementFrames_SIMD_AVX512(FrameNums, Size);
                break;

            // 128 bit
            default:
                IncrementFrames_SIMD_SSE2(FrameNums, Size);
        }
    }

    // Helpers for incrementing with SIMD --------------------

    // AXV / AXV2 (256 bit - 8 floats) - 8 at a time
    void IncrementFrames_SIMD_AVX2(
        vector<uint32_t>& FrameNums,
        size_t Size
    ) {
        // Helper
        __m256i One = _mm256_set1_epi32(1);
    
        // Loop
        size_t i = 0;
        for(; i + 8 <= Size; i += 8)
        {
            // Frame nums
            __m256i FrameData = _mm256_loadu_si256(
                reinterpret_cast<const __m256i*>(&FrameNums[i])
            );

            // Add them
            FrameData = _mm256_add_epi32(FrameData, One);

            // Store back
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(&FrameNums[i]),
                FrameData
            );
        }

        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            FrameNums[i]++;
        }
    }



    // AXV512 (512 bit, 16 floats) - 16 at a time
    void IncrementFrames_SIMD_AVX512(
        vector<uint32_t>& FrameNums,
        size_t Size
    ) {
        // Helper
        __m512i One = _mm512_set1_epi32(1);
    
        // Loop
        size_t i = 0;
        for(; i + 16 <= Size; i += 16)
        {
            // Frame nums
            __m512i FrameData = _mm512_loadu_si512(
                reinterpret_cast<const __m512i*>(&FrameNums[i])
            );

            // Add them
            FrameData = _mm512_add_epi32(FrameData, One);

            // Store back
            _mm512_storeu_si512(
                reinterpret_cast<__m512i*>(&FrameNums[i]),
                FrameData
            );
        }

        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            FrameNums[i]++;
        }
    }



    // SSE2 (128 bit, 4 floats) - 4 at a time
    void IncrementFrames_SIMD_SSE2(
        vector<uint32_t>& FrameNums,
        size_t Size
    ) {
        // Helper
        __m128i One = _mm_set1_epi32(1);
    
        // Loop
        size_t i = 0;
        for(; i + 4 <= Size; i += 4)
        {
            // Frame nums
            __m128i FrameData = _mm_loadu_si128(
                reinterpret_cast<const __m128i*>(&FrameNums[i])
            );

            // Add them
            FrameData = _mm_add_epi32(FrameData, One);

            // Store back
            _mm_storeu_si128(
                reinterpret_cast<__m128i*>(&FrameNums[i]),
                FrameData
            );
        }

        // Loop through the rest of the items
        for(; i < Size; ++i)
        {
            // Update x
            FrameNums[i]++;
        }
    }
};