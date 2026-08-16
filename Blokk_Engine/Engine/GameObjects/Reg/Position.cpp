#include "GameObject.hpp"

// Set
void GameObject::SetPosition(float TargetX, float TargetY) 
{
    // Set X pos
    if(IsStatic) 
    {
        // Set x
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->StaticXPositions, EngineVelIdx, TargetX);

        // Set y
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->StaticYPositions, EngineVelIdx, TargetY);
    } else 
    {
        // Set x
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->DynamicXPositions, EngineVelIdx, TargetX);

        // Set y
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->DynamicYPositions, EngineVelIdx, TargetY);
    }
}
void GameObject::SetPosition(Vector2 TargetPos) {
    SetPosition(TargetPos.x, TargetPos.y);
}

void GameObject::SetXPosition(float Target) 
{
    // If static
    if(IsStatic) 
    {
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->StaticXPositions, EngineVelIdx, Target);
    } 
    else // If dynamic 
    {
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->DynamicXPositions, EngineVelIdx, Target);
    }
}
void GameObject::SetYPosition(float Target) 
{
    // Set
    if(IsStatic) {
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->StaticYPositions, EngineVelIdx, Target);
    } else {
        UpdateEngineData(CommandTypes::Set, 
            &EngineObjects->DynamicYPositions, EngineVelIdx, Target);
    }
}

// Get
Vector2 GameObject::GetPosition() 
{
    // If the object is static
    if(IsStatic)
    {
        // Get from the static object position data
        return Vector2(
            EngineObjects->StaticXPositions[EngineVelIdx],
            EngineObjects->StaticYPositions[EngineVelIdx]
        );
    }
    else // If the object is dynamic
    {
        // Get from the dynamic object data
        return Vector2(
            EngineObjects->DynamicXPositions[EngineVelIdx],
            EngineObjects->DynamicYPositions[EngineVelIdx]
        );
    }
}

// Change
void GameObject::ChangePosition(Vector2 Change) 
{
    // If the object is static 
    if(IsStatic) {
        // Add to the static position data

        // X
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticXPositions, EngineVelIdx, Change.x);

        // Y
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticYPositions, EngineVelIdx, Change.y);
    } 
    else // If the object is dynamic  
    {
        // Add to the dynamic position data

        // X
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicXPositions, EngineVelIdx, Change.x);

        // Y
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicYPositions, EngineVelIdx, Change.y);
    }
}
void GameObject::ChangePosition(float ChangeX, float ChangeY) 
{
    // If the object is static 
    if(IsStatic) 
    {
        // Add to the static position data

        // X
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticXPositions, EngineVelIdx, ChangeX);

        // Y
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticYPositions, EngineVelIdx, ChangeY);
    } 
    else // If the object is dynamic  
    {
        // Add to the dynamic position data

        // X
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicXPositions, EngineVelIdx, ChangeX);

        // Y
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicYPositions, EngineVelIdx, ChangeY);
    }
}

void GameObject::ChangeXPosition(float X) 
{
    // If the object is static 
    if(IsStatic) {
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticXPositions, EngineVelIdx, X);
    } 
    else // If the object is dynamic  
    {
        // Add to the dynamic position data
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicXPositions, EngineVelIdx, X);
    }
}
void GameObject::ChangeYPosition(float Y) 
{
    // If the object is static 
    if(IsStatic) 
    {
        // Add to the static position data
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->StaticYPositions, EngineVelIdx, Y);
    } 
    else // If the object is dynamic 
    {
        // Add to the dynamic position data
        UpdateEngineData(CommandTypes::Add, 
            &EngineObjects->DynamicYPositions, EngineVelIdx, Y);
    }
}