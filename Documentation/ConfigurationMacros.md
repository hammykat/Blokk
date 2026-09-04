# Configuration Macros

These are macros that you can define to change the engine's behavior.

> Make sure to **define these before including the engine**.
For example:
```cpp
#define Blokk_CamEnabled
#define Blokk_Diagnostics
#define Blokk_Thread_Control

#include <Blokk/Blokk.hpp>

int main()
{
    // ...
}
```

* [Camera](#camera)
* [Diagnostics](#diagnostics)
* [Render Culling Systems](#render-culling-systems)
* [Thread Control](#thread-control)

| Feature        | Macro                                                       | Default  |
| -------------- | ----------------------------------------------------------- | -------- |
| Camera         | `Blokk_CamEnabled`                                          | Disabled |
| Diagnostics    | `Blokk_Diagnostics`                                         | Disabled |
| Thread Control | `Blokk_Thread_Control`                                      | Disabled |
| Thread System  | `Blokk_Threads_AdaptiveTiming` / `Blokk_Threads_FixedCount` | Adaptive |
| Culling        | `Blokk_Render_CullingType`                                  | Axis, `1` |


## Camera

The camera is an optional feature that lets you move the view around the game world. When enabled, objects are rendered relative to the camera's position, so moving the camera changes which part of the world is shown on screen.

To enable the camera, add:

```cpp
#define Blokk_CamEnabled
```

You can set or change the camera's position using these functions:

| Function                                  | Use                                         |
| ----------------------------------------- | ------------------------------------------- |
| `void SetPosition(int32_t X, int32_t Y);` | Set the camera's position                   |
| `void SetPosition(Vector2 Pos);`          | Set the camera's position using a `Vector2` |
| `void SetXPosition(int32_t X);`           | Set the camera's X position                 |
| `void SetYPosition(int32_t Y);`           | Set the camera's Y position                 |
| `void ChangeXPosition(int32_t X);`        | Move the camera along the X axis            |
| `void ChangeYPosition(int32_t Y);`        | Move the camera along the Y axis            |

The camera position represents the point the view is offset from in the world. For example, increasing the camera's X position moves the view toward the right side of the world.

## Diagnostics

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

For example:
```
========== Blokk Diagnostics ==========

Frame
FPS: 144
Frame Time: 6.91 ms
Execution Time: 2.43 ms
Target Execution Time: 6.94 ms

Threads
Opened: 7
Available: 8
Optimal: Yes

SIMD
Register Size: 128 bits
Register Type: SSE2

System Timing
User Updates: 0.42 ms
Velocity: 0.31 ms
Animation: 0.08 ms
Culling: 0.21 ms
Rendering: 0.73 ms
```

## Render culling systems

There are currently 2 culling systems that you can choose the engine to use for rendering:

### 1. Basic Culling

Here, the engine compares every object's x and y to check if they are in the screen.
You need to define:
```cpp
#define Blokk_Render_CullingType 0 // Basic
```

#### Good situations:
* A few hundred objects
* Objects are spread fairly evenly
* Objects don't have wildly different sizes
* You want the simplest culling implementation
* You care more about low overhead than squeezing out every last operation

### 2. Axis Culling

In this, the engine will check each object's y position, filter out all the objects that can't be inside the screen based off of the results, then check the remaining x positions. This can let it perform almost 2x less work than basic culling.
You need to define:
```cpp
#define Blokk_Render_CullingType 0 // Basic
```

**Best for:** large scenes, large worlds, and situations where many objects can be quickly rejected by their X or Y position.

#### Good situations
- Thousands or more objects
- Large worlds with a relatively small viewport
- Objects spread across a large area
- Many objects are completely outside the screen
- Objects have predictable or relatively simple bounding dimensions
- Visibility checks are a major part of frame time
- You want to take advantage of SIMD processing
- You need high-throughput culling for large object counts

### Example

A large world might contain:

```
100,000 objects  
↓  
X-axis culling  
↓  
20,000 candidates  
↓  
Y-axis culling  
↓  
4,000 visible objects  
↓  
Render
```

AxisCulling is especially useful when most objects are far outside the viewport, because objects can be rejected without performing the complete visibility check.

## Thread control

Blokk has a feature called thread control. Use it by defining `Blokk_Thread_Control`.
Using it, you can manually destroy/create threads for the engine to use.

Here are the basic functions:

| Function | Use
|----------|-----
|`OpenThread();` | Open a thread for the engine to use
|`DestroyThread();` | Destroy one of the engine's threads
|`SetThreadCount();` | Set the engine's thread count

> **NOTE:**
> Avoid creating or destroying threads every frame as it can be expensive

Another feature blokk has is to decide what thread system to use. There are 2 types:
* Adaptive timing (default, `Blokk_Threads_AdaptiveTiming`)
* Fixed count (default for thread control, `Blokk_Threads_FixedCount`)

With a fixed count, you'll need to manually manage the threads, but with adaptive, the engine will manually create or destroy threads itself.


