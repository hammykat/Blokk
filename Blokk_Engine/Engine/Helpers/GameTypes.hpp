#pragma once

#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <string>

namespace Blokk {
    
struct Range {
    int Start, End;
    size_t GetSize() {
        return End - Start + 1;
    }
};

struct Circle {
    Vector2 Center;
    float Radius;
};

union CollisionHit {
    Rectangle RectHitBox;
    Circle CircleHitBox;
};

struct Animation {
    std::string Name;
    std::vector<Texture2D> Frames;
    size_t CurrentFrame;
};

class ObjectAnimations {
    std::vector<Animation> Animations;
    size_t CurrentAnimation;
};

struct DynamicRegisterInfo {
    Vector2 Vel;
    size_t Idx;
};
}