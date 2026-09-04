# The background engine architecture

The Blokk engine is the part of the engine that handles GameObjects for you every frame.
It is designed
It uses systems, which are currently:

- Movement with Velocity and Collisions
- Animations
- Visibility, Rendering, Culling (Configurable)

## Parallelism

Blokk uses multiple worker threads to parallelize engine processing.

When the engine starts, it determines the available CPU resources and initializes its worker threads. These threads remain active throughout the lifetime of the engine rather than being repeatedly created and destroyed during individual frames.

### Adaptive Timing

At the beginning of each frame, the main thread determines how the current workload should be divided. The object's data is split into **non-overlapping ranges of indexes**, and each worker thread is assigned a range to process.

For example, with 4 worker threads, the object data may be divided into:

| Thread | Assigned Range |
|--------|----------------|
| Thread 1 | 0 - 249   |
| Thread 2 | 250 - 499 |
| Thread 3 | 500 - 749 |
| Thread 4 | 750 - 999 |

Because each thread receives a unique range, two threads do not modify the same object data at the same time. This allows the engine to avoid unnecessary concurrent data structures for these operations.

The engine processes one system at a time. Once all threads have finished the current system, the engine moves on to the next system.

The order of systems is fixed because some systems depend on the results of earlier systems. For example, collision processing may depend on updated movement data.

The general process is:

```text
Start Frame
    ↓
Determine workload ranges
    ↓
Assign ranges to worker threads
    ↓
Process System
    ↓
Wait for all threads
    ↓
Process next System
    ↓
Repeat
```

### Fixed Count

With this system, you can control the thread count yourself, and you can decide how many threads to open, destroy, etc.
The engine will continue to distribute work across threads, but it won't use it's adaptive timing system and will not open/destroy threads.

> **Note**:
> You should avoid the creation/destruction of threads every frame as it's expensive

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

| Positions | | | |-
|---|---|---|---|---
| x | x | x | x | x
| y | y | y | y | y

Instead of:

| Positions | | |-
|------|------|------|-----
| x, y | x, y | x, y | x, y 

This design also helps with the SIMD. The engine stores the data of dynamic (objects that move) and static (objects that don't move) together, but makes sure that dynamic object data stays at the start and static object data stays at the back. There is also a vector storing pointers to all the object instances, so that when the engine needs to rearrange the vectors or perform tasks, it can make sure that the object's internal variables stay in sync.

  The engine won't store the velocities of static objects, because they are all {0, 0}. With animations, it stores the frame numbers, the names, the current animations, and the frames separatelyfor speed and cache-locality.

