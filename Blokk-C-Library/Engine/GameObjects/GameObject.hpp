#pragma once

#include "raylib.h"
#include <vector>
#include <cctype>
#include <cmath>
#include <queue>
#include "GameTypes.hpp"
#include "EngineSystems.hpp"
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
    void SetToVisible()
    {
        // Update local var
        IsVisible = true;

        // Get into visible
        EngineObjects->IntoVisible.push(EngineVelIdx);
    }

    // Set to invisible
    void SetToInvisible()
    {
        // Update local var
        IsVisible = false;

        // Remove from visible
        EngineObjects->FromVisible.push(EngineVelIdx);
    }

    // VELOCITY -----------------------------------------------------

    // Set
    void SetVelocity(float XVel, float YVel) 
    {
        // If being set to static
        if(XVel == 0 && YVel == 0) {

            // If static
            if(IsStatic) 
            {
                // Do nothing
                return;
            } 
            else // If dynamic
            {
                // Set to static
                SetToStatic();
            }
        } 
        else // If being set to dynamic 
        {
            // If static
            if(IsStatic) 
            {
                // Set to dynamic
                SetToDynamic(Vector2{XVel, YVel});
            }
            else // if dynamic
            {
                // Set X and Y
                UpdateEngineData_Double(
                    CommandTypes::Set,
                    &EngineObjects->XVelocities,
                    &EngineObjects->YVelocities,
                    XVel,
                    YVel,
                    EngineVelIdx
                );
            }
        }
    }
    void SetVelocity(Vector2 Velocity) {
        SetVelocity(Velocity.x, Velocity.y);
    }

    void SetVelocityX(float XVel) 
    {
        // If being set to static
        if(XVel == 0) 
        {
            // If static
            if(IsStatic) 
            {
                // Do nothing
                return;
            } 
            else // If dynamic
            {
                // If being set to static
                if(GetVelocityY() == 0) 
                {
                    // Set to static
                    SetToStatic();
                }
                else // If staying dynamic
                {
                    // Set X 
                    UpdateEngineData(
                        CommandTypes::Set,
                        &EngineObjects->XVelocities,
                        EngineVelIdx, XVel
                    );
                }
            }
        } 
        else // If being set to dynamic 
        {
            // If static
            if(IsStatic) 
            {
                // Set to dynamic with vel
                SetToDynamic(Vector2(XVel, 0));
            }
            else // If dynamic
            {
                // Set X 
                UpdateEngineData(
                    CommandTypes::Set,
                    &EngineObjects->XVelocities,
                    EngineVelIdx, XVel
                );
            }
        }
    }

    void SetVelocityY(float YVel) 
    {
        // If being set to static
        if(YVel == 0) 
        {
            // If static
            if(IsStatic) 
            {
                // Do nothing
                return;
            } 
            else // If dynamic
            {
                // If being set to static
                if(GetVelocityX() == 0) 
                {
                    // Set to static
                    SetToStatic();
                }
                else // If staying dynamic
                {
                    // Set X 
                    UpdateEngineData(
                        CommandTypes::Set,
                        &EngineObjects->YVelocities,
                        EngineVelIdx, YVel
                    );
                }
            }
        } 
        else // If being set to dynamic 
        {
            // If static
            if(IsStatic) 
            {
                // Set to dynamic with vel
                SetToDynamic(Vector2(YVel, 0));
            }
            else // If dynamic
            {
                // Set X 
                UpdateEngineData(
                    CommandTypes::Set,
                    &EngineObjects->YVelocities,
                    EngineVelIdx, YVel
                );
            }
        }
    }

    // Get
    Vector2 GetVelocity() 
    {
        // If the obejct is static
        if(IsStatic)
        {
            // Give the static velocity (Always 0)
            return Vel_ZERO;
        }
        else // If the object is dynamic
        {
            // Get the velocity from the manager
            return {
                EngineObjects->XVelocities[EngineVelIdx], 
                EngineObjects->YVelocities[EngineVelIdx]
            };
        }
    }
    float GetVelocityX() 
    {
        // If the object is static
        if(IsStatic)
        {
            return 0;
        }
        else // If the object is dynamic
        {
            // Get the vel from the manager's data
            return EngineObjects->XVelocities[EngineVelIdx];
        }
    }
    float GetVelocityY() {
        // If the object is static
        if(IsStatic)
        {
            return 0;
        }
        else // If the object is dynamic
        {
            // Get the vel from the manager's data
            return EngineObjects->YVelocities[EngineVelIdx];
        }
    }

    // Get the speed
    float GetSpeed() 
    {
        // If static return 0, not moving
        if(IsStatic) return 0;

        float XVel = EngineObjects->XVelocities[EngineVelIdx];
        float YVel = EngineObjects->YVelocities[EngineVelIdx];
        return sqrt(XVel * XVel + YVel * YVel);
    }
    float GetSpeedSquared() 
    {
        // If static return 0, not moving
        if(IsStatic) return 0;

        float XVel = EngineObjects->XVelocities[EngineVelIdx];
        float YVel = EngineObjects->YVelocities[EngineVelIdx];
        return XVel * XVel + YVel * YVel;
    }

    // If not moving, vel != 0
    bool IsMoving() 
    {
        return !IsStatic;
    }

    // Stop, set to 0
    void StopVelocity() {
        // Remove it's idx if present
        if(IsStatic)
        {
            return;
        }
        else // If dynamic
        {
            // Set to static
            SetToStatic();
        }
        EngineObjects->XVelocities[EngineVelIdx] = 0;
    }

    // Reverse
    void ReverseVelocity() 
    {
        // Reverse X
        UpdateEngineData(CommandTypes::Multiply, 
            &EngineObjects->XVelocities, EngineVelIdx, -1);
        // Reverse Y
        UpdateEngineData(CommandTypes::Multiply, 
            &EngineObjects->YVelocities, EngineVelIdx, -1);
    }
    void ReverseVelocityX() {
        UpdateEngineData(CommandTypes::Multiply, 
            &EngineObjects->XVelocities, EngineVelIdx, -1);
    }
    void ReverseVelocityY() {
        UpdateEngineData(CommandTypes::Multiply, 
            &EngineObjects->YVelocities, EngineVelIdx, -1);
    }

    // Multiply ------------------------------------------------

    void MultiplyVelocity(float FactorX, float FactorY) 
    {
        // If being set to static
        if(FactorX == 0 && FactorY == 0) 
        {
            // If it was already static
            if(IsStatic) 
            {
                // Do nothing
                return;
            }
            else // If it was dynamic
            {
                // Set to static
                SetToStatic();
            }
        }
        else // If being set to dynamic
        {
            // If it was static
            if(IsStatic)
            {
                // Do nothing, 0 * x = 0
                return;
            }
            else // If it was dynamic
            {
                // Multiply X
                UpdateEngineData(CommandTypes::Multiply, 
                    &EngineObjects->XVelocities, EngineVelIdx, FactorX);

                // Multiply Y
                UpdateEngineData(CommandTypes::Multiply, 
                    &EngineObjects->YVelocities, EngineVelIdx, FactorY);
            }
        }
    }
    void MultiplyVelocity(Vector2 Factor) {
        MultiplyVelocity(Factor.x, Factor.y);
    }
    void MultiplyVelocity(float Factor) {
        MultiplyVelocity(Factor, Factor);
    }

    // Individual
    void MultiplyXVelocity(float Factor) {
        MultiplyVelocity(Factor, 1);
    }
    void MultiplyYVelocity(float Factor) {
        MultiplyVelocity(1, Factor);
    }

    // Add --------------------------------------------------

    void AddVelocity(float XVel, float YVel) 
    {
        // Don't do anything if not changing
        if(XVel == 0 && YVel == 0) return;

        // If the object is static
        if(IsStatic)
        {
            // Set to dynamic
            SetToDynamic(Vector2{XVel, YVel});
        }
        else // If the object is dynamic
        {
            // Add to X vel
            UpdateEngineData(CommandTypes::Multiply, 
                &EngineObjects->XVelocities, EngineVelIdx, XVel);

            // Add to Y Vel
            UpdateEngineData(CommandTypes::Multiply, 
                &EngineObjects->YVelocities, EngineVelIdx, YVel);   
        }
    }
    void AddVelocity(Vector2 Vel) {
        AddVelocity(Vel.x, Vel.y);
    }

    // Subtract -----------------------------------------------

    void SubtractVelocity(float XVel, float YVel) 
    {
        // Don't do anything if not changing
        if(XVel == 0 && YVel == 0) return;

        if(IsStatic)
        {
            // Set to dynamic
            SetToDynamic(Vector2{-XVel, -YVel});
        }
        else // If dynamic
        {
            // Remove from X vel
            UpdateEngineData(CommandTypes::Subtract, 
                &EngineObjects->XVelocities, EngineVelIdx, XVel);

            // Remove from Y Vel
            UpdateEngineData(CommandTypes::Subtract, 
                &EngineObjects->YVelocities, EngineVelIdx, YVel);
        }
    }
    void SubtractVelocity(Vector2 Vel) {
        SubtractVelocity(Vel.x, Vel.y);
    }

    // Divide -----------------------------------------------------

    void DivideVelocity(float XVel, float YVel) 
    {
        // Avoid bugs with dividing by 0
        if(XVel == 0 || YVel == 0 || IsStatic) return;

        // Divide from X vel
        UpdateEngineData(CommandTypes::Divide, 
            &EngineObjects->XVelocities, EngineVelIdx, XVel);

        // Divide from Y Vel
        UpdateEngineData(CommandTypes::Divide, 
            &EngineObjects->YVelocities, EngineVelIdx, YVel);
    }
    void DivideVelocity(Vector2 Vel) {
        DivideVelocity(Vel.x, Vel.y);
    }

    void DivideVelocityX(float Vel) {
        DivideVelocity(Vel, 1);
    }

    // POSITION --------------------------------------------------------------------------

    // Set
    void SetPosition(float TargetX, float TargetY) 
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
    void SetPosition(Vector2 TargetPos) {
        SetPosition(TargetPos.x, TargetPos.y);
    }

    void SetXPosition(float Target) 
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
    void SetYPosition(float Target) 
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
    Vector2 GetPosition() 
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
    void ChangePosition(Vector2 Change) 
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
    void ChangePosition(float ChangeX, float ChangeY) 
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

    void ChangeXPosition(float X) 
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
    void ChangeYPosition(float Y) 
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

    // VISIBILITY --------------------------------------------------------------

    // TODO: Remake visibility to fit the engine design
    /* void Hide() 
    {
        // Remove it's idx if present
        RemoveVisIdx();

        // Update local var
        IsVisible = false;
    }

    void Show() 
    {
        // If is already visible
        if(IsVisible)
        {
            // Do nothing
            return;
        }
        else // If hidden
        {
            
        }
    }

    void SetVisible(bool Vis) 
    {
        // If the Idx was present and it needs to be
        if(!IsVisible && Vis) {
            // Add it's Idx
            EngineObjects->ValidVisibleIdxs.push_back(EngineVelIdx);
        }
        // Update local var
        IsVisible = Vis;
    }

    bool IsVisible() {
        return IsVisible;
    }

    void ToggleVisibility() 
    {
        RemoveVisIdx();

        // Reverse local var
        IsVisible = !IsVisible;
    } */

} GameObject;

