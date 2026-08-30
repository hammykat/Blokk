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
