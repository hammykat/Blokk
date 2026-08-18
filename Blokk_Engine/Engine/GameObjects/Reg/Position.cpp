#include "GameObject.hpp"

// Set
void GameObject::SetPosition(float TargetX, float TargetY) 
{
    // Update X and Y
    UpdateEngineData_Double(CommandTypes::Set, 
            &EngineObjects->XPositions, 
            &EngineObjects->YPositions,
            TargetX, TargetY,
            EngineIdx
        );
}
void GameObject::SetPosition(Vector2 TargetPos) {
    SetPosition(TargetPos.x, TargetPos.y);
}

void GameObject::SetXPosition(float Target) 
{
    UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->XPositions, EngineIdx, Target);
}
void GameObject::SetYPosition(float Target) 
{
    // Set
    UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->YPositions, EngineIdx, Target);
}

// Get
Vector2 GameObject::GetPosition() 
{
    return Vector2(
        EngineObjects->XPositions[EngineIdx],
        EngineObjects->YPositions[EngineIdx]
    );
}

// Change
void GameObject::ChangePosition(float ChangeX, float ChangeY) 
{
    UpdateEngineData_Double(CommandTypes::Add, 
        &EngineObjects->XPositions,
        &EngineObjects->YPositions, 
        ChangeX, ChangeY,
        EngineIdx
    );
}
void GameObject::ChangePosition(Vector2 Change) {
    ChangePosition(Change.x, Change.y);
}

void GameObject::ChangeXPosition(float X) 
{
    UpdateEngineData(CommandTypes::Add, 
        &EngineObjects->XPositions, EngineIdx, X);
}
void GameObject::ChangeYPosition(float Y) 
{
    UpdateEngineData(CommandTypes::Add, 
        &EngineObjects->YPositions, EngineIdx, Y);
}