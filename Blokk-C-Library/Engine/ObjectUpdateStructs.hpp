#include <vector>
#include "raylib.h"

enum class CommandTypes {
    Destroy,
    Set,
    Subtract,
    Add, 
    Multiply,
    Divide
};

enum class RenderTypes {
    Circle,
    Rectangle,
    Texture
};

struct FieldUpdate {
    CommandTypes Type;
    std::vector<float>* Vector;
    size_t Idx;
    float Value;
};

struct DoubleFieldUpdate {
    CommandTypes Type;
    std::vector<float> *XVector;
    std::vector<float> *YVector;
    float Value1;
    float Value2;
    size_t Idx;
};

struct ObjectCreationParams 
{
    Vector2 Position;
    Vector2 Velocity;
};