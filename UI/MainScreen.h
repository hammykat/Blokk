#include "raylib.h"
#include <stdbool.h>

#ifndef MAINSCREEN_H
    #define MAINSCREEN_H
#endif

void DrawMainScreen(bool *MainAnimLoaded, bool *MainAnimPlayed, Texture2D *MainAnimFrames, Color *MainScreenBackgroundColor, Color *MainScreenSideBarColor, Rectangle *MainScreenSideBar);
void LoadDefaultAnimation(Texture2D *MainAnimFrames);