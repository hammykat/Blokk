# Engine Diagnostics

You can get the engine diagnostics by enabling 'Debug' in the 'ManagerCreation' struct while creating the object instance.

It introduces new functions:
| Function | Use 
|----------|----
|`GetTotalObjects();`| Get the total object count
|`GetStaticObjectCount();`| Get the static object count
|`GetDynamicObjectCount();`| Get the dynamic object count
|`GetOpenedThreads();`| Get the number of threads opened by the engine
|`GetTotalThreads();`| Get the number of total threads the engine could use
|`GetPrevFrameExecutionTime();`| Get the amount of time the engine executed all of the processes in the previous frame
|`GetTargetExecutionTime();`| Get the target amount of time the engine will try to execute all of it's systems
|`GetFPS();`| Get the configured FPS
|`GetUserUpdateTime();`| Get the time the engine took to process the user's update commands
|`GetVelocityTime();`| Get the amount of time the engine took to update positions with velocities
|`GetAnimationIncrementTime();`| Get the amount of time the engine took to increment all the animation frames
|`GetVisibilityCullingTime();`| Get the amount of time the engine took to cull all the visible objects
|`GetRenderTime();`| Get the amount of time the engine took to render all the on-screen objects
|`SetTargetExecutionTime(double Time);`| Set the engine's target execution time
|`GetSIMDRegisterSize();`| Get the detected SIMD register size in bits
|`GetSIMDRegisterType();`| Get the detected SIMD type
|`PrintDiagnostics();`| Get the engine to print the diagnostics

## PrintDiagnostics

The diagnostic report prints all of the engine's stats.
This includes:
* The FPS
* The total frame time
* The engine's frame execution time
* The engine's target execution time
* How many threads the engine opened
* The max amount of threads the engine could open
* If the optimal thread count has been reached
* The total object count
* The static object count
* The dynamic object count
* The SIMD register type
* The SIMD register size in bits
* The user update system timing
* The velocity update system timing
* The animation incrementing system timing
* The visibility culling system timing
* The object render timing
