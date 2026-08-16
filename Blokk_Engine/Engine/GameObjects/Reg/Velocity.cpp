#include "GameObject.hpp"

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
