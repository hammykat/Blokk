You're right. I changed existing wording and structure when you specifically asked to **update it without changing the rest**. Here's the **entire document**, copy-pastable, with the existing content preserved and only the v0.1.7 changes incorporated.

````markdown
# Using the engine

## Table of Contents

* [Engine Requirements](#engine-requirements)

  * [Platform Support](#platform-support)
  * [CPU](#cpu)
  * [Memory](#memory)
  * [Graphics](#graphics)
  * [Development Requirements](#development-requirements)
  * [Supported Compilers](#supported-compilers)
* [Configuration Macros](#configuration-macros)
* [Getting Started](#getting-started)

  * [Including Blokk](#including-blokk)
  * [Creating an ObjectManager](#creating-an-objectmanager)
* [Game Objects](#game-objects)

  * [Creating Objects](#creating-objects)
  * [Modifying Objects](#modifying-objects)
* [Engine Loop](#engine-loop)
* [Using Blokk With SDL3](#using-blokk-with-sdl3)
* [Using Blokk Without Built-In Rendering](#using-blokk-without-built-in-rendering)
* [Example Template](#example-template)
* [Next Steps](#next-steps)

---

## Engine Requirements

### Platform Support

| Platform            | Architecture | Status      |
| ------------------- | ------------ | ----------- |
| Windows             | x86-64       | Supported   |
| Linux               | x86-64       | Untested    |
| macOS Intel         | x86-64       | Untested    |
| macOS Apple Silicon | ARM64        | Unsupported |
| Linux ARM64         | ARM64        | Unsupported |
| Windows ARM64       | ARM64        | Unsupported |

### CPU

Blokk currently targets x86-64 processors.

* x86-64 processor
* SSE2 support required
* AVX2 supported
* AVX-512 supported
* AVX is not supported
* ARM NEON is not supported

Blokk uses SIMD instructions where supported to improve performance. SSE2 is currently required, while AVX2 and AVX-512 can provide additional optimization on compatible processors.

### Memory

No fixed minimum has been established yet.

Required memory depends on the application, asset sizes, and number of objects being managed.

* Recommended minimum: 4 GB RAM

### Graphics

Blokk's built-in rendering system uses SDL3 and SDL_image.

* GPU with a SDL3-supported graphics API
* Hardware-accelerated graphics recommended

If you are using a different rendering framework, Blokk can still be used for its object and engine systems. See [Using Blokk Without Built-In Rendering](#using-blokk-without-built-in-rendering).

### Development Requirements

To develop with Blokk, you will need:

* C++20-compatible compiler
* CMake
* Git
* SDL3
* SDL_image

### Supported Compilers

Blokk is intended to work with:

* MSVC
* GCC
* Clang

---

## Configuration Macros

Blokk provides several compile-time configuration options. These allow you to enable or select engine features before the engine is included.

Configuration macros **must be defined before including Blokk**.

For example:

```cpp
#define Blokk_Diagnostics
#define Blokk_CamEnabled
#define Blokk_Rendering_Enabled
#define Blokk_Visibility_CullType 1

#include <Blokk.hpp>
````

The available configuration macros are:

* `Blokk_Visibility_CullType` - Selects the visibility-culling system. `0` uses basic culling and `1` uses axis culling. See [Render Culling Systems](RenderCullingSystems.md).
* `Blokk_Diagnostics` - Enables engine diagnostics. See [Diagnostics](Diagnostics.md).
* `Blokk_CamEnabled` - Enables the camera system. See [Camera](Camera.md).
* `Blokk_Thread_Control` - Enables manual control over the engine's threading system. See [Thread Control](ThreadControl.md).
* `Blokk_Rendering_Enabled` - Enables Blokk's built-in rendering system.

Only define the features your project needs. This allows Blokk to avoid enabling systems that your project does not use.

---

## Getting Started

Once Blokk is configured, you can begin using the engine in your project.

Most Blokk classes and functions are contained within the `Blokk` namespace.

```cpp
#include <Blokk.hpp>
```

### Including Blokk

Include Blokk after defining any configuration macros you want to use:

```cpp
#define Blokk_Diagnostics

#include <Blokk.hpp>
```

If you are using Blokk's built-in rendering system, you will also need SDL3 and SDL_image:

```cpp
#include <Blokk.hpp>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
```

### Creating an ObjectManager

The `ObjectManager` is the main interface for managing objects and running the engine.

Create an instance by passing a `ManagerCreation` structure:

```cpp
Blokk::ManagerCreation CreationParams{
    Vector2{1280, 720},
    60
};

Blokk::ObjectManager MyManager(CreationParams);
```

`ManagerCreation` contains the basic information the engine needs when starting:

```cpp
struct ManagerCreation 
{
    Vector2 ScreenDimensions;
    uint32_t FPS = 30;
};
```

* `ScreenDimensions` - The dimensions of the screen used by the rendering system.
* `FPS` - The target FPS used for the engine's default timing. Defaults to `30`.

Once the `ObjectManager` has been created, you can begin creating objects and processing the engine.

---

## Game Objects

Game objects in Blokk are represented by instances of the `GameObject` class.

### Creating Objects

When creating a `GameObject`, you can provide starting values using `ObjectCreationParams`.

For example:

```cpp
Blokk::ObjectCreationParams Params{
    Vector2{100, 100},
    Vector2{5, 0}
};

Blokk::GameObject Player(Params);
```

The starting position and velocity default to `{0, 0}` when they are not specified.

Once the object has been created, you can interact with it using its member functions.

### Modifying Objects

Blokk provides functions for getting, setting, and changing object data.

For example:

```cpp
Player.SetVelocity(Vector2{5, 6});

std::cout << Player.GetVelocityX();
```

The object directly accesses the engine's underlying object data, allowing changes made through the `GameObject` interface to be reflected in the engine.

For a complete list of available functions, see the [GameObject Function List](GameObjectFunctions.md).

For a deeper look at how Blokk stores and processes objects, see the [Engine Architecture](EngineArchitecture.md) documentation.

---

## Engine Loop

Blokk is designed to fit naturally into a game loop.

Each frame, the engine should first process its internal systems and then render the objects.

A basic loop looks like this:

```cpp
while (!WindowShouldClose())
{
    MyManager.EngineProcess();

    BeginDrawing();
    ClearBackground(BLACK);

    MyManager.RenderObjects();

    EndDrawing();
}
```

### `EngineProcess()`

Processes the engine for the current frame.

This is where Blokk updates its internal object data and runs the engine's enabled systems.

### `RenderObjects()`

Renders the objects managed by Blokk.

This function uses SDL3's rendering system.

The typical order should therefore be:

```text
EngineProcess()
      ↓
RenderObjects()
```

Your own game logic can be performed around the engine's processing step as needed.

---

## Using Blokk With SDL3

Blokk's built-in renderer uses SDL3 for rendering and SDL_image for loading image assets.

A basic SDL3 project and a Blokk project therefore have a very similar structure:

```cpp
SDL_Init(SDL_INIT_VIDEO);

SDL_Window* Window =
    SDL_CreateWindow("Blokk Example", 1280, 720, 0);

Blokk::ManagerCreation CreationParams{
    Vector2{1280, 720},
    60
};

Blokk::ObjectManager MyManager(CreationParams);

Blokk::Renderer Renderer(MyManager, Window);

while (true)
{
    // Game logic
    MyManager.EngineProcess();

    // Rendering
    Renderer.RenderObjects();
}

SDL_DestroyWindow(Window);
SDL_Quit();
```

The `Renderer` manages the SDL3 rendering system while the `ObjectManager` continues to manage the engine's object data.

### Animations

Animation frames can be loaded individually using SDL_image.

For example:

```cpp
Renderer.CreateAnimation(
    "PlayerIdle",
    {
        "Assets/Player/idle_0.png",
        "Assets/Player/idle_1.png",
        "Assets/Player/idle_2.png"
    }
);
```

The renderer loads each image into an `SDL_Texture` and stores the animation data in the `ObjectManager`.

---

## Using Blokk Without Built-In Rendering

Blokk's rendering system is optional.

If you are using another graphics or game framework, you can use Blokk to manage your object data and systems while handling rendering yourself.

For example, your application could use:

```text
Your Game
   │
   ├── Blokk
   │    ├── Object management
   │    ├── Movement
   │    ├── Collision
   │    └── Other engine systems
   │
   └── Your renderer
```

This allows Blokk to handle the data and processing while your own renderer decides how objects are displayed.

---

## Example Template

> You can also find this template [here](../Example%20Projects/Template.cpp).

```cpp
/*
Here define the things you want the engine to use.
For example:
#define Blokk_Diagnostics
#define Blokk_Rendering_Enabled
*/

#include <Blokk.hpp>
#include <SDL3/SDL.h>

int main()
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window
    SDL_Window* Window =
        SDL_CreateWindow("Blokk Example", 1280, 720, 0);

    // Create an ObjectManager
    Blokk::ManagerCreation MCr = {
        Vector2{1280, 720}, // Screen dimensions
        60                  // FPS
    };

    Blokk::ObjectManager MyManager(MCr);

    // Create the renderer
    Blokk::Renderer Renderer(MyManager, Window);

    // Game loop
    bool Running = true;

    while (Running)
    {
        SDL_Event Event;

        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_QUIT)
                Running = false;
        }

        // Let the engine update
        MyManager.EngineProcess();

        // Let the engine render objects
        Renderer.RenderObjects();
    }

    // Close SDL
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}
```

---

## Next Steps

Now that you have a basic Blokk project running, the following documentation covers the engine's individual systems in more detail:

* [GameObject Functions](GameObjectFunctions.md) - Complete list of `GameObject` functions.
* [Engine Architecture](EngineArchitecture.md) - How Blokk internally manages and processes objects.
* [Render Culling Systems](RenderCullingSystems.md) - Information about Blokk's rendering and visibility-culling systems.
* [Diagnostics](Diagnostics.md) - Using Blokk's diagnostic tools.
* [Camera](Camera.md) - Using the camera system.
* [Thread Control](ThreadControl.md) - Controlling the engine's threading behavior.

```
```
