#include "GameObject.hpp"

// Set
void GameObject::SetVelocity(float XVel, float YVel) 
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
                EngineIdx
            );
        }
    }
}
void GameObject::SetVelocity(Vector2 Velocity) {
    SetVelocity(Velocity.x, Velocity.y);
}

void GameObject::SetVelocityX(float XVel) 
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
                UpdateEngineData<float>(
                    CommandTypes::Set,
                    &EngineObjects->XVelocities,
                    EngineIdx, XVel
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
            UpdateEngineData<float>(
                CommandTypes::Set,
                &EngineObjects->XVelocities,
                EngineIdx, XVel
            );
        }
    }
}

void GameObject::SetVelocityY(float YVel) 
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
                UpdateEngineData<float>(
                    CommandTypes::Set,
                    &EngineObjects->YVelocities,
                    EngineIdx, YVel
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
            UpdateEngineData<float>(
                CommandTypes::Set,
                &EngineObjects->YVelocities,
                EngineIdx, YVel
            );
        }
    }
}

// Get
Vector2 GameObject::GetVelocity() 
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
            EngineObjects->XVelocities[EngineIdx], 
            EngineObjects->YVelocities[EngineIdx]
        };
    }
}
float GameObject::GetVelocityX() 
{
    // If the object is static
    if(IsStatic)
    {
        return 0;
    }
    else // If the object is dynamic
    {
        // Get the vel from the manager's data
        return EngineObjects->XVelocities[EngineIdx];
    }
}
float GameObject::GetVelocityY() {
    // If the object is static
    if(IsStatic)
    {
        return 0;
    }
    else // If the object is dynamic
    {
        // Get the vel from the manager's data
        return EngineObjects->YVelocities[EngineIdx];
    }
}

// Get the speed
float GameObject::GetSpeed() 
{
    // If static return 0, not moving
    if(IsStatic) return 0;

    float XVel = EngineObjects->XVelocities[EngineIdx];
    float YVel = EngineObjects->YVelocities[EngineIdx];
    return sqrt(XVel * XVel + YVel * YVel);
}
float GameObject::GetSpeedSquared() 
{
    // If static return 0, not moving
    if(IsStatic) return 0;

    float XVel = EngineObjects->XVelocities[EngineIdx];
    float YVel = EngineObjects->YVelocities[EngineIdx];
    return XVel * XVel + YVel * YVel;
}

// If not moving, vel != 0
bool GameObject::IsMoving() 
{
    return !IsStatic;
}

// Stop, set to 0
void GameObject::StopVelocity() {
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
}

// Reverse
void GameObject::ReverseVelocity() 
{
    // If static do nothing
    if(IsStatic) return;

    // Reverse X
    UpdateEngineData<float>(CommandTypes::Multiply, 
        &EngineObjects->XVelocities, EngineIdx, -1);
    // Reverse Y
    UpdateEngineData<float>(CommandTypes::Multiply, 
        &EngineObjects->YVelocities, EngineIdx, -1);
}
void GameObject::ReverseVelocityX() 
{
    // If static do nothing
    if(IsStatic) return;

    UpdateEngineData<float>(CommandTypes::Multiply, 
        &EngineObjects->XVelocities, EngineIdx, -1);
}
void GameObject::ReverseVelocityY() 
{
    // If static do nothing
    if(IsStatic) return;

    UpdateEngineData<float>(CommandTypes::Multiply, 
        &EngineObjects->YVelocities, EngineIdx, -1);
}

// Multiply ------------------------------------------------

void GameObject::MultiplyVelocity(float FactorX, float FactorY)
{
    // Return if 0
    if(IsStatic)
        return;

    // Find new X and Y
    float NewX = GetVelocityX() * FactorX;
    float NewY = GetVelocityY() * FactorY;

    if(NewX == 0 && NewY == 0)
    {
        SetToStatic();
        return;
    }

    // Update data
    UpdateEngineData_Double(
        CommandTypes::Multiply,
        &EngineObjects->XVelocities,
        &EngineObjects->YVelocities,
        FactorX,
        FactorY,
        EngineIdx
    );
}
void GameObject::MultiplyVelocity(Vector2 Factor) {
    MultiplyVelocity(Factor.x, Factor.y);
}
void GameObject::MultiplyVelocity(float Factor) {
    MultiplyVelocity(Factor, Factor);
}

// Individual
void GameObject::MultiplyXVelocity(float Factor) {
    MultiplyVelocity(Factor, 1);
}
void GameObject::MultiplyYVelocity(float Factor) {
    MultiplyVelocity(1, Factor);
}

// Add --------------------------------------------------

void GameObject::AddVelocity(float XVel, float YVel) 
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
        // Add 
        UpdateEngineData_Double(CommandTypes::Add, 
            &EngineObjects->XVelocities, 
            &EngineObjects->YVelocities,
            XVel, YVel,
            EngineIdx
        ); 
    }
}
void GameObject::AddVelocity(Vector2 Vel) {
    AddVelocity(Vel.x, Vel.y);
}

// Divide -----------------------------------------------------

void GameObject::DivideVelocity(float XVel, float YVel) 
{
    // Avoid bugs with dividing by 0
    if(XVel == 0 || YVel == 0 || IsStatic) return;

    // Divide 
    UpdateEngineData_Double(CommandTypes::Divide, 
        &EngineObjects->XVelocities, 
        &EngineObjects->YVelocities,
        XVel, YVel, EngineIdx
    );
}
void GameObject::DivideVelocity(Vector2 Vel) {
    DivideVelocity(Vel.x, Vel.y);
}

void GameObject::DivideVelocityX(float Vel) {
    DivideVelocity(Vel, 1);
}
void GameObject::DivideVelocityY(float Vel) {
    DivideVelocity(1, Vel);
}
