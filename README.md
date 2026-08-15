# Blokk

A beginner-friendly, open source 2D game engine in **C++ and raylib** designed for **easy use** and **high performance**.

It is designed to have:
- **Beginner-friendly API** -- Simple, easy to understand functions and systems designed to be easy to use and understand.
- **High Performance** -- The engine is designed to be high performance and utilizes things like **SIMD**, **SoA for data layout**, and **Parallel tasking** to distribute work across cores.
- **Adaptability** -- The engine will time it's processes and adapt to the user's computer, allowing for it to be used on many computers of different speed.
- **2D Focused** -- The engine is built specifically around 2D game development.
- **C++** -- Full access to C++ and the underlying engine.
- **LightWeight** -- The engine is made to be lightweight and uses **raylib**
- **Open source** — Anyone can inspect, learn from, contribute to, and improve the engine.

The goal is to provide a simple API for beginners without sacrificing the performance and control expected from a C++ engine.

### Speed 

The engine uses many things to be high-performance, like:
- Data-oriented design
- Efficient memory layouts
- SIMD
- Parallel task processing
- Cache-friendly data structures
- Batched processing
- Separation of static and dynamic object data

The engine is designed for extreme speed, utilizing things like efficient data layout and SIMD to handle hundreds of thousands to millions of objects at stable fps (For more about how the engine works, check the [background engine architecture](Documentation/EngineArchitecture.md)).

I'm looking for contributors! If you're interested, check [here](CONTRIBUTING.md]

This project is protected under the zlib license
