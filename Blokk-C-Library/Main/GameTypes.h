#ifndef GAMESTYPES_H
#define GAMETYPES_H

#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    CollisionRect,
    CollisionCircle
} CollisionBoxType;

typedef struct {
    int *Velocity;
    size_t ValidVelCount;

    int *Collisions;
    size_t ValidCollisionCount;

    int *Animations;
    size_t ValidAnimCount;
} ValidIdx;

typedef struct {
    char *Characters;
    size_t Length;
} String;

typedef struct {
    Vector2 Center;
    float Radius;
} Circle;

typedef union {
    Rectangle RectHitBox;
    Circle CircleHitBox;
} Collider;

typedef struct {
    Collider HitBox;
    CollisionBoxType HitBoxType;
} Collision;

typedef struct {
    String Name;
    Texture2D *Frames;
    size_t FrameCount;
    size_t CurrentFrame;
} Animation;

typedef struct {
    Animation *Animations;
    size_t CurrentAnimation;
    size_t Count;
} ObjectAnimations;

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

typedef struct { 
    Vector2 *Position;
    Vector2 *Velocity;
    Collider *Collisions;
    ObjectAnimations *Animations; 
    bool *IsStatic;
    bool *IsVisible;
    bool *IsAnimated;
    bool *IsCollidable;
    int ObjectCount;
} Objects;

#endif
