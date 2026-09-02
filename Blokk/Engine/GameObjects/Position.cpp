#include "GameObject.hpp" 
 
namespace Blokk { 
 
// Set 
void GameObject::SetPosition(float TargetX, float TargetY)  
{ 
    // Update X and Y 
    Engine->XPositions[EngineIdx] = TargetX;
    Engine->YPositions[EngineIdx] = TargetY;
} 

void GameObject::SetPosition(Vector2 TargetPos) 
{ 
    SetPosition(TargetPos.x, TargetPos.y); 
} 
 
void GameObject::SetXPosition(float Target)  
{ 
    Engine->XPositions[EngineIdx] = Target;
} 

void GameObject::SetYPosition(float Target)  
{ 
    Engine->YPositions[EngineIdx] = Target;
} 
 
// Get 
Vector2 GameObject::GetPosition()  
{ 
    return Vector2( 
        Engine->XPositions[EngineIdx], 
        Engine->YPositions[EngineIdx] 
    ); 
} 
 
// Change 
void GameObject::ChangePosition(float ChangeX, float ChangeY)  
{ 
    Engine->XPositions[EngineIdx] += ChangeX;
    Engine->YPositions[EngineIdx] += ChangeY;
} 

void GameObject::ChangePosition(Vector2 Change) 
{ 
    ChangePosition(Change.x, Change.y); 
} 
 
void GameObject::ChangeXPosition(float X)  
{ 
    Engine->XPositions[EngineIdx] += X;
} 

void GameObject::ChangeYPosition(float Y)  
{ 
    Engine->YPositions[EngineIdx] += Y;
} 
 
}