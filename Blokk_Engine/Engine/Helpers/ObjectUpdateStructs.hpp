#pragma once

#include <vector>
#include <concepts>

#include "raylib.h"
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


// Updates 

template <typename T>
concept ConfiguredUpdateType =
    std::same_as<T, uint32_t> ||
    std::same_as<T, float> ||
    std::same_as<T, bool>;

template<ConfiguredUpdateType T>
struct FieldUpdate {
    CommandTypes Type;
    std::vector<T>* Vector;
    uint32_t Idx;
    T Value;
};

template <ConfiguredUpdateType T>
struct DoubleFieldUpdate {
    CommandTypes Type;
    std::vector<T> *XVector;
    std::vector<T> *YVector;
    T XValue;
    T YValue;
    uint32_t Idx;
};

// Creation

struct ObjectCreationParams 
{
    Vector2 Position;
    Vector2 Velocity;
    GameObject *Object;
};

}