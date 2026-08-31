#include <utility>
#include <vector>

#include "raylib.h"
#include "GameTypes.hpp"
#include "GameObject.hpp"

namespace Blokk {

void ObjectManager::DestroyStaticObject(uint32_t ObjIdx)
{
    // Swap with the last item
    SwapStaticObjects(ObjIdx, ObjectCount - 1);

    // Remove the object
    XPositions.pop_back();
    YPositions.pop_back();
    ObjectInstances.pop_back();

    StaticObjectCount--;
    ObjectCount--;
}

void ObjectManager::DestroyDynamicObject(uint32_t ObjIdx) 
{
    // Swap with the last item
    SwapDynamicObjects(ObjIdx, DynamicObjectCount - 1);

    if(StaticObjectCount > 0)
    {
        // Swap with the last static item
        SwapStaticObjects(DynamicObjectCount - 1, ObjectCount - 1);
    }

    // Remove the object from the back
    XPositions.pop_back();
    YPositions.pop_back();
    XVelocities.pop_back();
    YVelocities.pop_back();
    ObjectInstances.pop_back();

    DynamicObjectCount--;
    ObjectCount--;
}

void ObjectManager::ProcessAddCommand(Vector2 Velocity, Vector2 Position, GameObject* Object, bool Visible)
{
    float XVel = Velocity.x;
    float YVel = Velocity.y;

    // If creating a static object
    if(XVel == 0 && YVel == 0)
    {
        Object->EngineIdx = XPositions.size();

        // Add position
        XPositions.push_back(Position.x);
        YPositions.push_back(Position.y);
        
        // Add instance pointer
        ObjectInstances.push_back(Object);

        // Add visibility
        IsVisible.push_back(Visible);

        StaticObjectCount++;
    } 
    else // If creating a dynamic object
    {
        // Add position
        XPositions.push_back(Position.x);
        YPositions.push_back(Position.y);

        // Add instance pointer
        ObjectInstances.push_back(Object);

        // Add visibility
        IsVisible.push_back(Visible);

        if(StaticObjectCount > 0)
        {
            // Swap position with first static member to keep with dynamic object data
            SwapStaticObjects(DynamicObjectCount, ObjectCount - 1);
        } 
        else {
            // Add index
            Object->EngineIdx = DynamicObjectCount;
        }

        // Add velocity
        XVelocities.push_back(XVel);
        YVelocities.push_back(YVel);

        DynamicObjectCount++;
    }

    ObjectCount++;
}


void ObjectManager::ProcessAddCommand(ObjectCreationParams Fields)
{
    ProcessAddCommand(Fields.Velocity, Fields.Position, Fields.Object, Fields.Visible);
}

void ObjectManager::SwapStaticObjects(uint32_t Obj1, uint32_t Obj2)
{
    // Swap X and Y
    std::swap(XPositions[Obj1], XPositions[Obj2]);
    std::swap(YPositions[Obj1], YPositions[Obj2]);

    // Swap instance pointers
    std::swap(ObjectInstances[Obj1], ObjectInstances[Obj2]);

    // Swap visibility
    std::swap(IsVisible[Obj1], IsVisible[Obj2]);

    // Update object's internal vars
    ObjectInstances[Obj1]->EngineIdx = Obj1;
    ObjectInstances[Obj2]->EngineIdx = Obj2;
}

void ObjectManager::SwapDynamicObjects(uint32_t Obj1, uint32_t Obj2)
{
    // Swap X and Y
    std::swap(XPositions[Obj1], XPositions[Obj2]);
    std::swap(YPositions[Obj1], YPositions[Obj2]);

    // Swap velocities
    std::swap(XVelocities[Obj1], XVelocities[Obj2]);
    std::swap(YVelocities[Obj1], YVelocities[Obj2]);

    // Swap visibility
    std::swap(IsVisible[Obj1], IsVisible[Obj2]);

    // Swap instance pointers
    std::swap(ObjectInstances[Obj1], ObjectInstances[Obj2]);

    // Update object's internal vars
    ObjectInstances[Obj1]->EngineIdx = Obj1;
    ObjectInstances[Obj2]->EngineIdx = Obj2;
}

}