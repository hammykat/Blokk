# Blokk

A beginner-friendly, open source 2D game engine in **C++ and raylib** designed for **easy use** and **high performance**.

It is designed to have:
- **Beginner-friendly API** - Simple, easy to understand functions and systems designed to be easy to use and understand.
- **High Performance** - The engine is designed to be high performance and utilizes things like **SIMD**, **SoA for data layout**, and **Parallel tasking** to distribute work across cores.
- **Adaptability** - The engine will time its processes and adapt to the user's computer, allowing for it to be used on many computers of different speed.
- **2D Focused** - The engine is built specifically around 2D game development.
- **C++** -- Full access to C++ and the underlying engine.
- **Lightweight** - The engine is made to be lightweight and uses **raylib**
- **Open source** - Anyone can inspect, learn from, contribute to, and improve the engine.

The goal is to provide a simple API for beginners without sacrificing the performance and control expected from a C++ engine.

## High-Performance 

The engine uses many things to be high-performance, like:
- Data-oriented design
- Efficient memory layouts
- SIMD
- Parallel task processing
- Cache-friendly data structures
- Batched processing
- Separation of static and dynamic object data

The engine is designed for extreme speed, utilizing things like efficient data layout and SIMD to handle a very large amount (hundreds of thousands to millions in later updates) of objects at stable fps (For more about how the engine works, check out the [background engine architecture](Documentation/EngineArchitecture.md).

## Documentation

For a complete documentation, check out the [documentation](Documentation).
Here you'll see how the engine is structured, how to use it, and cool features it has!

## Support / Contributing

If you want to offer support, hang out, contribute and get announcements when a new update gets posted, join the [discord server](https://discord.gg/kdzdDNvxdg)!
You can also reach out to **@Hammykat** on discord!

In the server:

* We'll send announcements about new updates
* You can contribute (see below)
* You can chill and hang out with people
* You can see what's in the works for future updates

You can contribute in many ways, such as:

* Testing
* Documentation
* Code review
* Bug fixes
* UI design
* Ideas
* Suggestions
* Feedback

And any other place you think you could help.

## Required specs

| Component            | Minimum                           |
| -------------------- | --------------------------------- |
| **CPU architecture** | x86 / x86-64                      |
| **SIMD**             | **SSE2**                          |
| **AVX**              | Not required                      |
| **AVX2**             | Not required                      |
| **AVX-512**          | Not required                      |
| **NEON/ARM**         | Not currently supported           |
| **OS**               | Windows / Linux / macOS*          |
| **RAM**              | No hard minimum established yet   |
| **GPU**              | OpenGL-capable GPU through Raylib |
| **C++**              | C++20                             |
| **Build system**     | CMake                             |
| **Compiler**         | MSVC, GCC, or Clang               |

### Licensing
This project is protected under the zlib license.
For more, [check here](LICENSE)
