# Using the engine

Using the engine is made to be as easy as possible. 
Before you use it, you need to `#define` specific things you want the engine to do **before** including the engine:
* `Blokk_Visibility_CullType` - `0` for basic, `1` for axis ([more info](RenderCullingSystems.md))
* `Blokk_Diagnostics` - Define for the engine to get diagnostics ([more info](Diagnostics.md))

To use it, create a new instance off the `ObjectManager` class. 
```cpp
ObjectManager MyManager(CreationParams);
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

Using the engine has the same basic structure as a raylib project. 
Here's an example template (You can also find it [here](ExampleProjects/Template.cpp)):

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
    ManagerCreation MCr = {
        Vector2{1280, 720}, // Screen dimensions
        60 // FPS
    };
    ObjectManager MyManager(MCr); // New instance


    // Here, perform all your tasks for the 

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
