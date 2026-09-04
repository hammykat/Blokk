# Using the engine

## Table of Contents

* Engine Requirements
* Configuration Macros
* Engine

## Requirements

### Platform Support

| Platform            | Architecture | Status      |
| ------------------- | ------------ | ----------- |
| Windows             | x86-64       | Supported   |
| Linux               | x86-64       | Untested      |
| macOS Intel         | x86-64       | Untested      |
| macOS Apple Silicon | ARM64        | Unsupported |
| Linux ARM64         | ARM64        | Unsupported |
| Windows ARM64       | ARM64        | Unsupported |

### CPU

- x86-64 processor
- SSE2 support required
- AVX2 supported
- AVX-512 supported
- AVX is not supported
- ARM NEON is not supported

### Memory

No fixed minimum has been established yet.
Required memory depends on the application, asset sizes, and object count.

- Recommended Minimum: 4 GB RAM

### Graphics

- GPU with Raylib-supported graphics API
- Hardware-accelerated graphics recommended

### Development Requirements

- C++20-compatible compiler
- CMake
- Git

### Supported Compilers

- MSVC
- GCC
- Clang


## Configuration Macros

Using the engine is made to be as easy as possible. 
Before you use it, you need to `#define` specific things you want the engine to do **before** including the engine:
* `Blokk_Visibility_CullType` - `0` for basic, `1` for axis ([more info](RenderCullingSystems.md))
* `Blokk_Diagnostics` - Define for the engine to get diagnostics ([more info](Diagnostics.md))
* `Blokk_CamEnabled` - Define for the engine to enable the camera ([more info](Camera.md))
* `Blokk_Thread_Control` - Define to control the engine's threading ([more info](ThreadControl.md))

## Engine

> Most things in the engine are defined under the 'Blokk' namespace.

After defining the wanted configuration macros, create a new instance off the `ObjectManager` class. 
```cpp
Blokk::ObjectManager MyManager(CreationParams);
```
Here, you pass a struct called `ManagerCreation`, which looks like:
```cpp
struct ManagerCreation 
{
    Vector2 ScreenDimensions; // Screen dimensions (used for rendering)
    uint32_t FPS = 30; // FPS, defaults to 30 (used for default timing)
};
```

Then, you can start using the manager!
The 2 most important functions are:
* `RenderObjects();` - Use to render all the objects
* `EngineProcesses();` - Use to let the engien update all the data

Using the engine has the same basic structure as a raylib project. You should familiarize yourself with raylib before using the engine, or you can use the engine with any other game engine framework, but **the rendering only works with raylib**. Therefore, if you're using something other than raylib, you can use the engine to handle the object data and render the objects yourself.

## Game Objects

Creating a game object is creating an instance of the class. While creating the instance, you can specify certain starting values, like the position and velocity, in the form of a struct called `ObjectCreationParams` (`Vector2 Velocity; Vector2 Position`). The default starting position and velocity are `{0, 0}`. For example:
```cpp
// Creating a new game object
Blokk::ObjectCreationParams Params{
    Vector2{100, 100},
    Vector2{5, 0}
};

Blokk::GameObject Player(Params);
```

With the instance created, you can then call its functions normally like any class. You can get, set or change certain variables with getting / setting functions. 

                                                                                                                                            For a list of the functions, check the [function list](GameObjectFunctions.md).
To take a deeper dive on how the engine handles objects, check the [engine documentation](EngineArchitecture.md).

If you do this:

```cpp

GameObject MyObject = new();

MyObject.SetVelocity(Vector2{5, 6});

std::cout << MyObject.GetVelocityX();

```

The output will be 5 because the object directly reads and mutates the engine's data.



## Example template

> You can also find it [here](../Example%20Projects/Template.cpp)

```cpp
/*
Here define the things you want the engine to use.
For example:
#define Blokk_Diagnostics
*/

#include <Blokk.hpp>
#include "raylib.h"

int main()
{
    // Create a window
    InitWindow(1280, 720, "Blokk Example Project");

    // Set the FPS
    SetTargetFPS(60);

    // Create an ObjectManager
    Blokk::ManagerCreation MCr = {
        Vector2{1280, 720}, // Screen dimensions
        60 // FPS
    };
    Blokk::ObjectManager MyManager(MCr); // New instance

    // raylib loop
    while(!WindowShouldClose())
    {

        // Here, perform all your tasks for the frame

        {
            // Let engine update
            MyManager.EngineProcess();

            // Begin drawing
            BeginDrawing();
            ClearBackground(BLACK); // Clear background

            // Let engine render objects
            MyManager.RenderObjects();

            EndDrawing();
        }
    }

    // Make sure to close!
    CloseWindow();

    return 0;
}

```

