#include "MainScreen.h"
#include "raylib.h"
#include <stdbool.h>

// Draw the main screen
void DrawMainScreen(bool *MainAnimLoaded, bool *MainAnimPlayed, Texture2D *MainAnimFrames, Color *MainScreenBackgroundColor, Color *MainScreenSideBarColor, Rectangle *MainScreenSideBar) {
    // Load main anim is not loaded
    if (!MainAnimLoaded) {
        LoadDefaultAnimation(MainAnimFrames);
    }

    // Display main anim if not done yet
    if (MainAnimLoaded && !MainAnimPlayed) {
        for(int i = 0; i < 20; i++) {
            DrawTexture(MainAnimFrames[i], 50, 0, WHITE);
            WaitTime(0.1);
        }
        MainAnimPlayed = true;
    }

    // Background
    ClearBackground(*MainScreenBackgroundColor);

    // SideBar
    DrawRectangleRec(*MainScreenSideBar, *MainScreenSideBarColor);
}


// Load the defualt blokk animation
void LoadDefaultAnimation(Texture2D *MainAnimFrames) {
    Texture2D MainAnimFrames[20];
    for(int i = 0; i < 20; i++) {
        char Path[100];
        sprintf(Path, "MainAnim/ss-%010d.png", i);
        MainAnimFrames[i] = LoadTexture(Path);
    }
}

void LoadLoadingAnimation() {
    
}