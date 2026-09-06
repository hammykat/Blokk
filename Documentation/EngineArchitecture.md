# The Background Engine Architecture

The Blokk engine is the part of the engine responsible for managing and processing `GameObject` data every frame.

It is designed to process large numbers of objects efficiently by combining:

* Parallel processing
* SIMD optimizations
* Cache-friendly data layouts
* Configurable engine systems

The currently implemented systems include:

* Movement with Velocity
* Animations
* Visibility
* Rendering
* Culling

Visibility and culling systems can be configured depending on the needs of the application.

---

## Parallelism

Blokk uses multiple worker threads to parallelize engine processing.

When the engine starts, it determines the available CPU resources and initializes its worker threads. These threads remain active throughout the lifetime of the engine rather than being repeatedly created and destroyed during individual frames.

This avoids the overhead associated with repeatedly creating and destroying operating-system threads.

### Adaptive Timing

At the beginning of each frame, the main thread determines how the current workload should be divided.

Object data is split into **non-overlapping ranges of indexes**, and each worker thread is assigned a range to process.

For example, with four worker threads and 1,000 objects, the workload could be divided as follows:

| Thread   | Assigned Range |
| -------- | -------------- |
| Thread 1 | 0 - 249        |
| Thread 2 | 250 - 499      |
| Thread 3 | 500 - 749      |
| Thread 4 | 750 - 999      |

Because each thread receives a unique range, multiple threads do not modify the same object data at the same time for systems using this processing model.

This reduces the need for synchronization and concurrent data structures within these operations.

The engine processes one system at a time. Once all worker threads have finished the current system, the engine moves on to the next system.

The order of systems is fixed because some systems depend on the results of earlier systems. For example, a system that uses object positions may depend on movement having already been processed.

The general process is:

```text
Start Frame
    ↓
Determine Workload Ranges
    ↓
Assign Ranges to Worker Threads
    ↓
Process System
    ↓
Wait for All Threads
    ↓
Process Next System
    ↓
Repeat
```

### Fixed Thread Count

Blokk also provides a fixed thread-count configuration.

With a fixed thread count, the user can control how many worker threads are used by the engine.

The worker threads are still created once and reused throughout the engine's lifetime. Changing to a fixed thread count does **not** require the engine to repeatedly create and destroy threads during frame processing.

The primary difference is that the engine does not dynamically determine the worker count for each workload.

> **Note:** Creating and destroying threads repeatedly can introduce significant overhead. Blokk therefore keeps its worker threads alive and reuses them.

---

# SIMD

Blokk uses **SIMD (Single Instruction, Multiple Data)** to perform the same operation on multiple values simultaneously.

SIMD allows an engine system to process multiple pieces of object data within a single instruction when the operation and data layout allow it.

For example, a 128-bit register can contain four 32-bit floating-point values:

```text
128 bits
┌────────┬────────┬────────┬────────┐
│ float  │ float  │ float  │ float  │
│  32    │  32    │  32    │  32    │
└────────┴────────┴────────┴────────┘
```

Likewise:

| SIMD Instruction Set | Register Width | 32-bit Floats |
| -------------------- | -------------- | ------------- |
| SSE / SSE2           | 128-bit        | 4             |
| AVX / AVX2           | 256-bit        | 8             |
| AVX-512              | 512-bit        | 16            |
| NEON                 | 128-bit        | 4             |

The exact number of objects processed per SIMD operation depends on the implementation and the data being processed. These values represent the number of 32-bit floating-point values that can fit in a register, not necessarily the number of complete objects processed by every operation.

### SIMD Fallback

SIMD is an optimization rather than a requirement.

Blokk detects the available SIMD capabilities of the target architecture and selects an appropriate implementation.

When a supported SIMD implementation is unavailable, Blokk can use a **scalar implementation** instead.

This allows the engine to remain functional on processors without the SIMD instruction set required by an optimized implementation.

The general model is:

```text
CPU
 │
 ├── Supported SIMD
 │      ├── SSE2
 │      ├── AVX2
 │      ├── AVX-512
 │      └── NEON
 │
 └── No applicable SIMD
          ↓
       Scalar
```

This architecture allows Blokk to prioritize portability without completely giving up hardware-specific optimizations.

---

# Data Layout

Blokk stores object data in **cache-friendly vectors** while exposing a `GameObject` class instance for each object.

This provides a convenient object-oriented interface to the user while allowing the engine's internal systems to use a data-oriented representation.

When a `GameObject` is created, its individual fields are stored in **Structure of Arrays (SoA)** form.

For example, positions are stored conceptually as:

```text
X Positions:
┌────┬────┬────┬────┬────┐
│ x0 │ x1 │ x2 │ x3 │ x4 │
└────┴────┴────┴────┴────┘

Y Positions:
┌────┬────┬────┬────┬────┐
│ y0 │ y1 │ y2 │ y3 │ y4 │
└────┴────┴────┴────┴────┘
```

Instead of storing each object's complete position together:

```text
┌────────┬────────┬────────┬────────┐
│ (x0,y0)│ (x1,y1)│ (x2,y2)│ (x3,y3)│
└────────┴────────┴────────┴────────┘
```

This layout provides better memory locality for systems that operate on a specific field across many objects.

It also makes the data easier to process using SIMD because consecutive values can be loaded into SIMD registers.

---

## Dynamic and Static Objects

Blokk separates dynamic and static object data within its internal storage.

Dynamic objects, which can change position through movement, are kept toward the beginning of the relevant data structures, while static objects are kept toward the end.

This allows systems that operate primarily on moving objects to process a contiguous region of data without unnecessarily processing static objects.

Static objects do not require velocity storage because their velocity is always `{0, 0}`.

A simplified representation is:

```text
Object Data

┌───────────────────────────────────────┐
│ Dynamic Objects                       │
│                                       │
│ Object 0                              │
│ Object 1                              │
│ Object 2                              │
│ ...                                   │
├───────────────────────────────────────┤
│ Static Objects                        │
│                                       │
│ Object N                              │
│ Object N+1                            │
│ ...                                   │
└───────────────────────────────────────┘
```

Blokk also maintains references to the corresponding `GameObject` instances so that the user-facing object interfaces remain associated with their underlying engine data when internal storage is rearranged.

---

# Animation Data

Animation data is also stored separately rather than keeping all animation-related information inside each object's class instance.

Depending on the animation data being used, Blokk maintains separate collections for information such as:

* Current animation
* Current frame
* Animation names
* Animation frames
* Frame dimensions
* Animation frame counts

Separating these values allows engine systems to access the data they need without loading unrelated object data into the CPU cache.

This also provides a more suitable layout for processing animation data across many objects.

---

# Overall Architecture

The combination of **SoA data storage, persistent worker threads, workload-based parallelism, SIMD implementations, and scalar fallbacks** allows Blokk to process object data using both hardware parallelism and data-level parallelism.

Conceptually:

```text
                     Blokk Engine
                          │
             ┌────────────┴────────────┐
             │                         │
       Data-Oriented Storage      GameObject API
             │                         │
             │                   User Interface
             │
       ┌─────┴─────┐
       │           │
   Worker Threads  SIMD
       │           │
       └─────┬─────┘
             │
       Engine Systems
             │
    ┌────────┼─────────┐
    │        │         │
 Movement  Animation  Visibility
                       │
                  Rendering/Culling
```

Blokk's architecture is designed so that the user can work with familiar `GameObject` instances while the engine internally uses data-oriented techniques to improve processing efficiency.
