# The background engine architecture

The Blokk engine is the part of the engine that handles GameObjects for you every frame.
It uses systems, which are currently:

- Movement with Velocity and Collisions
- Animations
- Visibility / Rendering

## Parallelism

The engine will open threads according to the workload.
The engine decides this by timing how long it takes to execute all the systems. It'll first get the frame time and then get the optimal time (Around **80% of the total** frame time). Then, it'll time it's processes, and compare it to the optimal frame time. If it surpasses it, it'll open another thread (If any are available) to distribute the workload with. The next frame, it'll compare the previous frame time to the current frame time, and if it surpasses that or is equal to it, it'll destroy the previously created thread, as it didn't offer any performance difference.

At the start of the program, the engine will only open up one thread, and keep creating more if it needs like explained above. The engine has to synchronize multiple vectors containing data to  store the right data for each object. Each thread is assigned a specific **range of indexes** to iterate over (With SIMD), which are unique, which eliminates the need for concurrent data structures, as **they will not edit the same range at the same time**. The engine will execute one system at a time, and will move on to the next after one is finished **in a fixed order**, which is needed as some systems have to be executed before others (Like movement is dependent on updating velocity, or how collisions use movement).

The engine splits the workload across all threads by **assigning ranges** (determined at the **start of the frame** by the main thread) to each thread. Then, it assigns the threads their ranges and the function, and then waits for the threads to finish. 

## SIMD

The engine will use **SIMD** to carry out **multiple operations at one time**. It matters on how big the user's computer's SIMD  register width. For example, if it can carry 4 floats (128 bit),  then the engine will carry out the operations on 4 objects at the same  time. The same happens with 256 and 512 bit registers (8 and 16  floats).

Register Name | Register Width | # Of Objects at once
|-------------|----------------|---------------------
| SSE / SSE2  | 128-bit        | 4
| AVX / AVX2  | 256-bit        | 8
| AVX512      | 512-bit        | 16

### Data Layout

  The engine stores all object data in **cache-friendly vectors**. The user is exposed a **class instance** for each object. This makes the UI friendlier to the user, as they can call functions directly on those instances. When the user creates a new object, the engine stores all the object's fields in an SoA (Structure of Arrays) form, for cache locality and easier SIMD implementation.

For example, it stores them like:

| Positions | --|--|--|--
|---|---|---|---|---
| x | x | x | x | x
| y | y | y | y | y

Instead of:

| Positions | -----|------|-----
|------|------|------|-----
| x, y | x, y | x, y | x, y 

This design also helps with the SIMD. The engine stores the data of dynamic (objects that move) and static (objects that don't move) together, but makes sure that dynamic object data stays at the start and static object data stays at the back. There is also a vector storing pointers to all the object instances, so that when the engine needs to rearrange the vectors or perform tasks, it can make sure that the object's internal variables stay in sync.

  The engine won't store the velocities of static objects, because they are all {0, 0}. With animations, it stores the frame numbers, the names, the current animations, and the frames separatelyfor speed and cache-locality.

