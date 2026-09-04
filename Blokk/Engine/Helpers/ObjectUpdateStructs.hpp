#pragma once

#include <vector>
#include <concepts>

#include "GameObject.hpp"

namespace Blokk {

enum class CommandTypes {
    Destroy,
    Set,
    Subtract,
    Add, 
    Multiply,
    Divide
};


// Creation

struct ObjectCreationParams 
{
    bool Visible = false;
    Vector2 Position;
    Vector2 Velocity;
    GameObject *Object;
};

}