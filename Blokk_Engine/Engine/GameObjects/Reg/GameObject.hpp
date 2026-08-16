#pragma once

#include "raylib.h"
#include <vector>
#include <cctype>
#include <cmath>
#include <queue>
#include "GameTypes.hpp"
#include "EngineClassData.hpp"
#include "ObjectUpdateStructs.hpp"

using namespace std;

#define Vel_ZERO Vector2{0, 0}

class ObjectManager;
class GameObject;

class GameObject {

public:
    friend class ObjectManager;
    static inline ObjectManager *EngineObjects;

    // Remembers where it is located in the engine's data
    size_t EngineVelIdx;
    size_t EngineVisIdx;

    // Cache the update for faster updates
    queue<FieldUpdate>& EngineUpdates;
    queue<DoubleFieldUpdate>& EngineDoubleUpdates;
    string CurrentAnim;

    // States
    bool IsStatic;
    bool IsVisible;
    bool IsAnimated;
    bool IsCollidable;

    GameObject(ObjectCreationParams CP = {{0, 0}, {0, 0}}) :
        EngineUpdates(EngineObjects->FieldUpdateCommands),
        EngineDoubleUpdates(EngineObjects->DoubleFieldUpdateCommands),
        IsStatic(CP.Velocity.x == 0 && CP.Velocity.y == 0),
        EngineVelIdx((IsStatic)? EngineObjects->StaticXPositions.size() : EngineObjects->DynamicXPositions.size()),
        EngineVisIdx(SIZE_MAX),
        IsVisible(false)
    {
        EngineObjects->Creations.push(CP);
    }

    // HELPERS -----------------------------------------------------

    void UpdateEngineData(
        CommandTypes Type, 
        vector<float> *Values, 
        size_t Idx, float OtherVal
    ) {
        EngineUpdates.push(
            (FieldUpdate) {
                Type,
                Values,
                Idx,
                OtherVal
            }
        );
    }
    
    void UpdateEngineData_Double(
        CommandTypes Type, 
        vector<float> *XVals, 
        vector<float> *YVals, 
        const float XUpdateVal,
        const float YUpdateVal,
        size_t Idx
    ) {
        EngineDoubleUpdates.push(
            DoubleFieldUpdate{
                Type,
                XVals, 
                YVals,
                XUpdateVal,
                YUpdateVal,
                Idx
            }
        );
    }

    // Set to static
    void SetToStatic()
    {
        // Update internal var
        IsStatic = true;

        // Remove from vel list
        EngineObjects->IntoStatic.push(EngineVelIdx);
    }

    // Set to dynamic
    void SetToDynamic(Vector2 Vel)
    {
        // Update internal var
        IsStatic = false;

        // Add to vel list
        EngineObjects->IntoDynamic.push(
            DynamicRegisterInfo{Vel, EngineVelIdx}
        );
    }

    // Set to visible
    void SetToVisible() // Assumes that the object was invisible before
    {
        // Update local var
        IsVisible = true;

        // Get into visible
        EngineObjects->IntoVisible.push(EngineVisIdx);
    }

    // Set to invisible
    void SetToInvisible() // Assumes that the object was visible before
    {
        // Update local var
        IsVisible = false;

        // Remove from visible
        EngineObjects->FromVisible.push(EngineVisIdx);
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

    // SUBTRACT -------------------------------------------------

    void SubtractVelocity(float XVel, float YVel);

    void SubtractVelocity(Vector2 Vel);

    // DIVIDE ---------------------------------------------------

    void DivideVelocity(float XVel, float YVel);

    void DivideVelocity(Vector2 Vel);

    void DivideVelocityX(float Vel);

    
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

    // Getter for visibility is the member `IsVisible` directly; remove method to avoid name clash.

    void ToggleVisibility();

    // Animation ------------------------------------------

    void SetAnimation(string AnimName)
    {

    }
};