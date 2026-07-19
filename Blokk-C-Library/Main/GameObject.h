#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameTypes.h"

// TODO: Create in-built functions, link to this game object

typedef struct { 
    Vector2 Position;
    Vector2 Velocity;
    Collider Collisions;
    ObjectAnimations Animations;

    bool IsStatic;
    bool IsVisible;
    bool IsAnimated;
    bool IsCollidable;

    
} GameObject;

DefComplexList(GameObject, GameObjectList)

#endif