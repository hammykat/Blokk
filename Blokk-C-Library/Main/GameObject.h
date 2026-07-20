#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameTypes.h"

// TODO: Create in-built functions, link to this game object

typedef struct GameObject { 
    Vector2 Position;
    Vector2 Velocity;
    Collider Collisions;
    ObjectAnimations Animations;

    // States
    bool IsStatic;
    bool IsVisible;
    bool IsAnimated;
    bool IsCollidable;

    // Functions
    void (*SetVelocity)(GameObject* Object, Vector2 Velocity);
    Vector2 (*GetVelocity)(GameObject* Object);

    void (*Hide)(GameObject*);
    void (*Show)(GameObject *Object);
    void (*IsVisible)(GameObject *Object);
    void (*ToggleVisibility)(GameObject *Object);

    void (*SetPosition)(GameObject *Object, Vector2 Position);
    Vector2 (*GetPosition)(GameObject *Object);
    void (*ChangePosition)(GameObject *Object, Vector2 Change);
    void (*ChangeXPosition)(GameObject *Object, float X);
    void (*ChangeYPosition)(GameObject *Object, float Y);

} GameObject;

DefComplexList(GameObject, GameObjectList)

#endif
