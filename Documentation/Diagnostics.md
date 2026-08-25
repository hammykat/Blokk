# Engine Diagnostics

The engine has a diagnostics feature, which you can enable with `#define Blokk_Diagnostics`.
Here, you can get the engine's diagnostics, by using the functions it provides you with:

> Please note that all timings are in **milliseconds**

| Function | Use
|----------|----
| `uint32_t GetTotalObjects();` | Get the total object count
| `uint32_t GetStaticObjectCount();` | Get the static object count
| `uint32_t GetDynamicObjectCount();` | Get the dynamic object count
| `uint32_t GetOpenedThreads();` | Get the number of threads currently opened by the engine
| `uint32_t GetTotalThreads();` | Get the total number of threads the engine can use
| `double GetFrameExecutionTime();` | Get the total time the engine took to process the previous frame
| `double GetTargetExecutionTime();` | Get the engine's target execution time for all its processes in a frame
| `uint32_t GetFPS();` | Get the engine's FPS it was initialized with
| `double GetUserUpdateTime();` | Get the amount of time it took to update the user's update commands
| `double GetVelocityTime();` | Get the amount of time the engine took to dynamic object's positions using their velocities
| `double GetAnimationIncrementTime();` | Get the amount of time the engine took to increment all object's frames for animation
| `double GetVisibilityCullingTime();` | Get the amount of time the engine took to cull objects that are going to get rendered
| `double GetRenderTime();` | Get the amount of time the engine took to render the on-screen objects
| `uint32_t GetSIMDRegisterSize();` | Get the engine's detected SIMD register size
| `std::string GetSIMDRegisterType();` | Get the engine's detected SIMD register type (look below)

It also has a function called:
`void SetTargetExecutionTime(double Time);`
Use this function to set the target time the engine will try to execute all it's processes in.

Quite a useful function is:
`void PrintDiagnostics();`
Using this function will print a diagnostics report containing the engine's:

**Frame:**
* FPS
* Total frame time
* Frame execution time
* Target execution time

**Threads:**
* Opened # of threads
* Total sensed thread count
* If the optimal thread count has been reached

**SIMD:**
* SIMD register size (in bits)
* SIMD register type

**System Timing:**
* User update processing
* Positions with Velocities
* Animation frame incrementing
* Visibility / Rendering culling
* Rendering
