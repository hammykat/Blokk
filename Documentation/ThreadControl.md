# Thread control

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

