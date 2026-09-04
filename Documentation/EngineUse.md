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
* [Using Blokk With Built-In Rendering](#using-blokk-with-built-in-rendering)
  * [Initializing](#initializing)
  * [Creating an ObjectManager](#creating-an-objectmanager-1)
  * [Creating a Window](#creating-a-window)
  * [Creating a Renderer](#creating-a-renderer)
  * [Rendering](#rendering)
  * [Setting the Clear Color](#setting-the-clear-color)
  * [Animations](#animations)
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
```

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

If you are using Blokk's built-in rendering system, enable it before including Blokk:

```cpp
#define Blokk_Rendering_Enabled

#include <Blokk.hpp>
```

You do not need to include SDL3 or SDL_image directly when using Blokk's public rendering API.

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

With the built-in renderer, a frame follows this order:

```text
EngineProcess()
      ↓
ClearScreen()
      ↓
RenderObjects()
      ↓
Present()
```

### `EngineProcess()`

Processes the engine for the current frame.

This is where Blokk updates its internal object data and runs the engine's enabled systems.

### `RenderObjects()`

Renders the objects managed by Blokk.

This function is provided by the `Renderer` when built-in rendering is enabled.

Your own game logic can be performed around the engine's processing step as needed.

---

# Using Blokk With Built-In Rendering

Blokk's built-in rendering system uses SDL3 for rendering and SDL_image for loading image assets.

To enable it, define:

```cpp
#define Blokk_Rendering_Enabled
```

before including Blokk:

```cpp
#define Blokk_Rendering_Enabled

#include <Blokk.hpp>
```

---

## Initializing

Initialize the SDL systems your game needs with `Blokk::Init()`:

```cpp
Blokk::Init(
    Blokk::InitFlags::Video |
    Blokk::InitFlags::Events
);
```

Multiple flags can be combined using `|`.

Available initialization flags:

| Flag       | Purpose                  |
| ---------- | ------------------------ |
| `Video`    | Video and window support |
| `Audio`    | Audio support            |
| `Joystick` | Joystick support         |
| `Haptic`   | Haptic feedback          |
| `Gamepad`  | Gamepad support          |
| `Events`   | Event handling           |
| `Sensor`   | Sensor support           |

For a normal game, you will usually want:

```cpp
Blokk::Init(
    Blokk::InitFlags::Video |
    Blokk::InitFlags::Events
);
```

---

## Creating an ObjectManager

Create an `ObjectManager` using `ManagerCreation`:

```cpp
Blokk::ManagerCreation CreationParams{
    Vector2{1280, 720},
    60
};

Blokk::ObjectManager MyManager(CreationParams);
```

---

## Creating a Window

Use `Blokk::CreateWindow()` to create a window:

```cpp
Blokk::CreateWindow(
    "My Game",
    1280,
    720
);
```

You can also provide `WindowFlags`:

```cpp
Blokk::CreateWindow(
    "My Game",
    1280,
    720,
    Blokk::WindowFlags::Resizable
);
```

Multiple flags can be combined using `|`:

```cpp
Blokk::WindowFlags::Resizable |
Blokk::WindowFlags::HighDPI
```

Available window flags:

| Flag         | Purpose                         |
| ------------ | ------------------------------- |
| `Fullscreen` | Creates a fullscreen window     |
| `Resizable`  | Allows the window to be resized |
| `Borderless` | Removes the window border       |
| `Hidden`     | Creates the window hidden       |
| `Maximized`  | Creates the window maximized    |
| `Minimized`  | Creates the window minimized    |
| `HighDPI`    | Enables high-DPI support        |

---

## Creating a Renderer

Create a `Renderer` by passing your `ObjectManager` and a window:

```cpp
Blokk::Renderer Renderer(
    MyManager,
    Blokk::CreateWindow(
        "My Game",
        1280,
        720
    )
);
```

You can also pass window flags:

```cpp
Blokk::Renderer Renderer(
    MyManager,
    Blokk::CreateWindow(
        "My Game",
        1280,
        720,
        Blokk::WindowFlags::Resizable
    )
);
```

The renderer manages the rendering system and the textures used by Blokk's animation system.

The window passed to the renderer is destroyed automatically when the renderer is destroyed.

---

## Rendering

Rendering a frame requires **three steps**:

```cpp
Renderer.ClearScreen();
Renderer.RenderObjects();
Renderer.Present();
```

### `ClearScreen()`

Clears the screen before rendering the next frame:

```cpp
Renderer.ClearScreen();
```

### `RenderObjects()`

Renders all objects currently registered for rendering:

```cpp
Renderer.RenderObjects();
```

The renderer automatically selects each object's:

* Animation
* Animation frame
* Position
* Frame dimensions

If the camera system is enabled, the camera position is also applied automatically.

See [Camera](Camera.md) for more information about the camera system.

### `Present()`

Displays the completed frame:

```cpp
Renderer.Present();
```

### Setting the Clear Color

You can change the color used when clearing the screen:

```cpp
Renderer.SetClearColor(
    30,  // Red
    30,  // Green
    30   // Blue
);
```

You can also specify an alpha value:

```cpp
Renderer.SetClearColor(
    30,  // Red
    30,  // Green
    30,  // Blue
    255  // Alpha
);
```

Alpha defaults to `255`.

---

## Animations

Blokk uses **individual image files** for animation frames.

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

Each image is loaded as a texture and stored by the engine.

### Creating an Empty Animation

You can create an animation without adding frames:

```cpp
Renderer.CreateNewEmptyAnimation(
    "PlayerIdle"
);
```

Frames can then be added individually:

```cpp
Renderer.AddFrameToAnimation(
    "PlayerIdle",
    "Assets/Player/idle_0.png"
);

Renderer.AddFrameToAnimation(
    "PlayerIdle",
    "Assets/Player/idle_1.png"
);
```

`CreateAnimation()` is a convenient way to create an animation and add multiple frames at once.

---

# Using Blokk Without Built-In Rendering

Blokk's rendering system is optional.

If you are using another graphics or game framework, you can use Blokk to manage your object data and engine systems while handling rendering yourself.

For example:

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

To disable Blokk's built-in renderer, simply do not define:

```cpp
#define Blokk_Rendering_Enabled
```

This allows Blokk to handle the engine systems while your own renderer decides how objects are displayed.

---

# Example Template

> You can also find this template [here](../Example%20Projects/Template.cpp).

```cpp
#define Blokk_Rendering_Enabled

#include <Blokk.hpp>

int main()
{
    // Initialize Blokk
    Blokk::Init(
        Blokk::InitFlags::Video |
        Blokk::InitFlags::Events
    );

    // Create the ObjectManager
    Blokk::ManagerCreation CreationParams{
        Vector2{1280, 720},
        60
    };

    Blokk::ObjectManager MyManager(
        CreationParams
    );

    // Create the window and renderer
    Blokk::Renderer Renderer(
        MyManager,
        Blokk::CreateWindow(
            "Blokk Example",
            1280,
            720,
            Blokk::WindowFlags::Resizable
        )
    );

    // Set the background color
    Renderer.SetClearColor(
        0,
        0,
        0
    );

    // Create an animation
    Renderer.CreateAnimation(
        "PlayerIdle",
        {
            "Assets/Player/idle_0.png",
            "Assets/Player/idle_1.png",
            "Assets/Player/idle_2.png"
        }
    );

    bool Running = true;

    while (Running)
    {
        // Process the engine
        MyManager.EngineProcess();

        // Render
        Renderer.ClearScreen();
        Renderer.RenderObjects();
        Renderer.Present();
    }

    // Shut down Blokk
    Blokk::Quit();

    return 0;
}
```

---

# Next Steps

Now that you have a basic Blokk project running, the following documentation covers the engine's individual systems in more detail:

* [GameObject Functions](GameObjectFunctions.md) - Complete list of `GameObject` functions.
* [Engine Architecture](EngineArchitecture.md) - How Blokk internally manages and processes objects.
* [Configuration Macros](ConfigurationMacros.md) - Explains different configurations, like camera, render culling, threads, etc.
