#pragma once

#include <vector>
#include <cctype>
#include <cmath>
#include <queue>
#include "GameTypes.hpp"
#include "EngineClassData.hpp"
#include "ObjectUpdateStructs.hpp"

using namespace std;

#define Vel_ZERO Vector2{0, 0}

namespace Blokk {

class ObjectManager;

class GameObject {

public:
    friend class ObjectManager;
    static inline ObjectManager *Engine;

    // Remembers where it is located in the engine's data
    uint32_t EngineIdx;

    // Anim
    string CurrentAnim;

    // States
    bool IsStatic;
    bool IsVisible;
    bool IsAnimated;
    bool IsCollidable;

    GameObject(ObjectCreationParams CP = {true, {0, 0}, {0, 0}}) :
        IsStatic(CP.Velocity.x == 0 && CP.Velocity.y == 0),
        EngineIdx(0),
        IsVisible(true)
    {
        Engine->ProcessAddCommand(CP);
    }

    GameObject(Vector2 Velocity, Vector2 Position, GameObject* Object, bool Visible = true) :
        IsStatic(Velocity.x == 0 && Velocity.y == 0),
        EngineIdx(0),
        IsVisible(true)
    {
        Engine->ProcessAddCommand(Velocity, Position, Object, Visible);
    }

    ~GameObject()
    {
        // If static
        if(IsStatic) {
            Engine->DestroyStaticObject(EngineIdx);
        } 
        // If dynamic
        else {
            Engine->DestroyDynamicObject(EngineIdx);
        }
    }

    // HELPERS -----------------------------------------------------

    // Set to static if dynamic
    void SetToStatic()
    {
        // Update internal var
        IsStatic = true;

        // Remove from dynamic, place at end of static
        Engine->SwapDynamicObjects(EngineIdx, Engine->DynamicObjectCount - 1);
        Engine->SwapStaticObjects(EngineIdx, Engine->ObjectCount - 1);

        // Remove velocities
        Engine->XVelocities.pop_back();
        Engine->YVelocities.pop_back();
    }

    // Set to dynamic if static
    void SetToDynamic(Vector2 Vel)
    {
        // Update internal var
        IsStatic = false;

        // Add to dynamic
        Engine->SwapStaticObjects(EngineIdx, Engine->DynamicObjectCount);

        // Add velocity
        Engine->XVelocities.push_back(Vel.x);
        Engine->YVelocities.push_back(Vel.y);

        // Update engine's vars
        Engine->DynamicObjectCount++;
    }

    // VELOCITY -----------------------------------------------------

    void SetVelocity(float XVel, float YVel);

    void SetVelocity(Vector2 Velocity);

    void SetVelocityX(float XVel);

    void SetVelocityY(float YVel);

    Vector2 GetVelocity();

    float GetVelocityX();

    float GetVelocityY();

    float GetSpeed();

    float GetSpeedSquared();

    bool IsMoving();

    void StopVelocity();



    void ReverseVelocity();

    void ReverseVelocityX();

    void ReverseVelocityY();

    // MULTIPLY -------------------------------------------------

    void MultiplyVelocity(float FactorX, float FactorY);

    void MultiplyVelocity(Vector2 Factor);

    void MultiplyVelocity(float Factor);

    // Individual
    void MultiplyXVelocity(float Factor);

    void MultiplyYVelocity(float Factor);

    // ADD ------------------------------------------------------

    void AddVelocity(float XVel, float YVel);

    void AddVelocity(Vector2 Vel);
    
    // DIVIDE ---------------------------------------------------

    void DivideVelocity(float XVel, float YVel);

    void DivideVelocity(Vector2 Vel);

    void DivideVelocityX(float Vel);

    void DivideVelocityY(float Vel);
    
    // POSITION -------------------------------------------------

    void SetPosition(float TargetX, float TargetY);

    void SetPosition(Vector2 TargetPos);

    void SetXPosition(float Target);

    void SetYPosition(float Target);

    Vector2 GetPosition();

    void ChangePosition(Vector2 Change);

    void ChangePosition(float ChangeX, float ChangeY);

    void ChangeXPosition(float X);

    void ChangeYPosition(float Y);

    // VISIBILITY --------------------------------------------------------------

    void Hide();

    void Show();

    void SetVisible(bool Vis);

    bool IsCurrentlyVisible();

    void ToggleVisibility();

    // Animation ------------------------------------------

    void SetAnimation(string AnimName);

    void SetFrameNum(uint32_t FrameNum);
};

}