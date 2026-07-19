// 

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    bool MainAnimLoaded = false;
    bool MainAnimPlayed = false;
    Texture2D MainAnimFrames[20];
    Color MainScreenBackgroundColor = {36, 54, 49, 255};
    Color MainScreenSideBarColor = {55, 90, 80, 255};
    Rectangle MainScreenSideBar = {0};

    InitWindow(1920, 1200, "My Raylib Window");
    SetWindowPosition(0, 0);
    ToggleFullscreen();

    while(!IsWindowReady()) {}
    WaitTime(1);

    int MonitorWidth = GetRenderWidth();
    int MonitorHeight = GetRenderHeight();

    if (MonitorWidth == 0 || MonitorHeight == 0) {
        MonitorWidth = 1920; 
        MonitorHeight = 1200; 
    }

    Color MainScreenBackgroundColor = {36, 54, 49, 255};
    Color MainScreenSideBarColor = {55, 90, 80, 255};
    Rectangle MainScreenSideBar = {
        .x = MonitorWidth - (MonitorWidth / 5),
        .y = 0,
        .height = MonitorHeight, 
        .width = MonitorWidth / 5
    };



    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            DrawMainScreen(&MainAnimLoaded, &MainAnimPlayed, &MainAnimFrames, &MainScreenBackgroundColor, &MainScreenSideBarColor, &MainScreenSideBar);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}



