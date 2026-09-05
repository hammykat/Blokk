# Contributing to Blokk

Thank you for your interest in contributing to **Blokk**!

Blokk is an open-source C++ game engine focused on being lightweight, understandable, and performance-oriented. Contributions are welcome, and they don't have to mean writing engine code.

## Ways to Contribute

There are many ways to contribute to Blokk:

### Code

You can contribute improvements or new features to the engine.

Examples include:

* Bug fixes
* Rendering improvements
* Performance improvements
* New engine systems
* Collision systems
* SIMD optimizations
* Threading improvements
* Camera improvements
* Animation improvements
* Build-system improvements
* Improvements to existing APIs

Before implementing a large feature, it is recommended that you discuss the idea first so it fits Blokk's direction.

### Testing

**Testing is a contribution.**

You can help by:

* Building Blokk on your system
* Testing different C++ compilers
* Testing different operating systems
* Testing different hardware
* Testing engine features
* Trying example projects
* Finding crashes or unexpected behavior
* Testing new releases
* Reporting reproducible bugs

When reporting a bug, include as much useful information as possible, such as:

* Operating system
* Compiler and version
* CMake version
* Blokk version or commit
* What you were doing
* What you expected to happen
* What actually happened
* Relevant error messages or logs
* A minimal example that reproduces the problem, if possible

### Documentation

Documentation is another important way to contribute.

You can improve:

* `Documentation/UsingTheEngine.md`
* `Documentation/EngineArchitecture.md`
* `Documentation/UpdateLog.md`
* API explanations
* Code examples
* Tutorials
* Comments
* Typographical errors
* Explanations that are difficult to understand

If you find something that is confusing or missing from the documentation, feel free to improve it.

### Examples

Example projects help new users understand how Blokk works.

You can contribute by:

* Improving existing examples
* Creating small example projects
* Demonstrating engine features
* Improving `Example Projects/Template.cpp`
* Creating examples for new systems

Examples should remain reasonably simple and easy to understand.

### Bug Reports

You don't need to know how to fix a bug to contribute.

If you find a problem, opening a clear issue is already a useful contribution.

Before opening an issue, check whether the problem has already been reported.

### Feature Ideas

Have an idea for Blokk?

Open an issue or discussion describing:

* What you want to add
* Why it would be useful
* How you think it could work
* Any potential performance or design concerns

Not every suggestion will necessarily be implemented. Blokk prioritizes features that fit its goals of being lightweight, understandable, and performance-oriented.

---

# Getting Started

## Requirements

Blokk currently uses:

* **C++20**
* **CMake**
* **SDL3**
* **SDL3_image**

Make sure the required dependencies are available before building the project.

## Project Structure

The repository is organized roughly like this:

```text
Blokk/
├── Blokk_Engine/
│   ├── GameObjects/
│   ├── Helpers/
│   └── Systems/
│
├── Documentation/
│
├── Example Projects/
│
├── Include/
│
├── vendored/
│   ├── SDL/
│   └── SDL_image/
│
├── CMakeLists.txt
└── README.md
```

The exact structure may change as Blokk develops.

### `Blokk_Engine/`

Contains the main engine implementation, including game objects, systems, and supporting code.

### `Documentation/`

Contains documentation for using and understanding Blokk.

### `Example Projects/`

Contains examples showing how Blokk can be used.

### `Include/`

Contains public headers intended to be included by projects using Blokk.

### `vendored/`

Contains external dependencies used by the engine.

---

# Development Guidelines

## Keep Blokk Understandable

Blokk is intended to be understandable to developers who want to learn how the engine works.

Avoid unnecessary abstraction or complexity when a simpler solution is sufficient.

Performance is important, but readability still matters.

## Consider Performance

Blokk is designed with performance in mind.

When modifying performance-sensitive code, consider:

* Memory access patterns
* Cache locality
* Unnecessary allocations
* Branching
* SIMD opportunities
* Threading overhead
* Data-oriented design
* Work performed every frame

Do not optimize blindly. If possible, measure the change before and after the modification.

## Follow Existing Style

Try to follow the existing code style and naming conventions.

Keep changes focused. A pull request that fixes one problem is generally easier to review than one that changes many unrelated systems.

## Avoid Unnecessary Dependencies

Blokk aims to remain lightweight.

Before adding a dependency, consider whether the functionality can reasonably be implemented using the existing engine or standard library.

---

# Making a Pull Request

1. Fork the repository.
2. Create a branch for your change.
3. Make your changes.
4. Build the project.
5. Test your changes.
6. Update documentation or examples when appropriate.
7. Commit your changes with a clear commit message.
8. Open a pull request.

A pull request should explain:

* What changed
* Why it changed
* How it was tested
* Any limitations or known issues

For larger changes, include additional design information when useful.

---

# Before Submitting

Before opening a pull request, make sure:

* [ ] The project builds successfully.
* [ ] Your changes have been tested.
* [ ] Existing functionality still works.
* [ ] New functionality has been tested where possible.
* [ ] Documentation has been updated when necessary.
* [ ] Examples have been updated when necessary.
* [ ] No unnecessary dependencies were added.
* [ ] The pull request describes the changes clearly.

---

# Code of Conduct

Please be respectful to other contributors.

Constructive criticism, questions, and different approaches are welcome. The goal is to improve Blokk together and make the project easier to understand and use.

---

# Thank You

Whether you contribute a major engine feature, fix a typo, test Blokk on another system, improve documentation, create an example, or report a bug, **your contribution helps the project**.

You do not have to be an experienced C++ developer to contribute.
