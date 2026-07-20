#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameTypes.h"
#include "GameObject.h"

// Velocity
void SetVelocity(GameObject* Object, Vector2 Velocity) {
    Object->Velocity = Velocity;
}
Vector2 GetVelocity(GameObject* Object) {
    return Object->Velocity;
}

// Visibility
void Hide(GameObject *Object) {
    Object->IsVisible = false;
}
void Show(GameObject *Object) {
    Object->IsVisible = true;
}
void IsVisible(GameObject *Object) {
    return Object->IsVisible;
}
void ToggleVisibility(GameObject *Object) {
    if(Object->IsVisible) {
        Object->IsVisible = false;
    } else {
        Object->IsVisible = true;
    }
}

// Positioning
void SetPosition(GameObject *Object, Vector2 Position) {
    Object->Position = Position;
}
Vector2 GetPosition(GameObject *Object) {
    return Object->Position;
}
void ChangePosition(GameObject *Object, Vector2 Change) {
    Object->Position.x += Change.x;
    Object->Position.y += Change.y;
}
void ChangeXPosition(GameObject *Object, float X) {
    Object->Position.x += X;
}
void ChangeYPosition(GameObject *Object, float Y) {
    Object->Position.y += Y;
}

void SmoothMoveTo()