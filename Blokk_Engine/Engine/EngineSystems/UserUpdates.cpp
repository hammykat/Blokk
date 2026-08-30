#include <utility>
#include <vector>

#include "raylib.h"
#include "GameTypes.hpp"
#include "GameObject.hpp"

namespace Blokk {

void ObjectManager::DestroyObject(uint32_t ObjIdx) 
{
    // If object is static
    if(ObjIdx >= DynamicObjectCount)
    {
        // Swap with the last item
        SwapStaticObjects(ObjIdx, ObjectCount - 1);

        // Remove the object
        XPositions.pop_back();
        YPositions.pop_back();
        ObjectInstances.pop_back();

        StaticObjectCount--;
    }
    else // If the object is dynamic 
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
    }

    ObjectCount--;
}

template <ConfiguredUpdateType T>
void ObjectManager::ProcessFieldUpdateCommand(FieldUpdate<T> Command)
{
    switch(Command.Type) 
    {
        case CommandTypes::Destroy:
            DestroyObject(Command.Idx);
            break;

        case CommandTypes::Set:
            (*Command.Vector)[Command.Idx] = Command.Value;
            break;

        case CommandTypes::Subtract:
            (*Command.Vector)[Command.Idx] -= Command.Value;
            break;

        case CommandTypes::Add:
            (*Command.Vector)[Command.Idx] += Command.Value;
            break;

        case CommandTypes::Multiply:
            (*Command.Vector)[Command.Idx] *= Command.Value;
            break;

        case CommandTypes::Divide:
            (*Command.Vector)[Command.Idx] /= Command.Value;
    }
}

void ObjectManager::ProcessDoubleUpdateCommand(DoubleFieldUpdate<float> Command)
{
    switch(Command.Type) 
    {
        case CommandTypes::Set:
            (*Command.XVector)[Command.Idx] = Command.XValue;
            (*Command.YVector)[Command.Idx] = Command.YValue;
            break;

        case CommandTypes::Subtract:
            (*Command.XVector)[Command.Idx] -= Command.XValue;
            (*Command.YVector)[Command.Idx] -= Command.YValue;
            break;

        case CommandTypes::Add:
            (*Command.XVector)[Command.Idx] += Command.XValue;
            (*Command.YVector)[Command.Idx] += Command.YValue;
            break;

        case CommandTypes::Multiply:
            (*Command.XVector)[Command.Idx] *= Command.XValue;
            (*Command.YVector)[Command.Idx] *= Command.YValue;
            break;

        case CommandTypes::Divide:
            (*Command.XVector)[Command.Idx] /= Command.XValue;
            (*Command.YVector)[Command.Idx] /= Command.YValue;
            break;

        default:
            break;
    }
}

void ObjectManager::ProcessAddCommand(ObjectCreationParams Fields)
{
    float XVel = Fields.Velocity.x;
    float YVel = Fields.Velocity.y;

    // If creating a static object
    if(XVel == 0 && YVel == 0)
    {
        Fields.Object->EngineIdx = XPositions.size();

        // Add position
        XPositions.push_back(Fields.Position.x);
        YPositions.push_back(Fields.Position.y);
        
        // Add instance pointer
        ObjectInstances.push_back(Fields.Object);

        StaticObjectCount++;
    } 
    else // If creating a dynamic object
    {
        // Add position
        XPositions.push_back(Fields.Position.x);
        YPositions.push_back(Fields.Position.y);

        // Add instance pointer
        ObjectInstances.push_back(Fields.Object);

        if(StaticObjectCount > 0)
        {
            // Swap position with first static member to keep with dynamic object data
            SwapStaticObjects(DynamicObjectCount, ObjectCount - 1);
        }

        // Add velocity
        XVelocities.push_back(XVel);
        YVelocities.push_back(YVel);

        DynamicObjectCount++;
    }

    ObjectCount++;
}

void ObjectManager::SwapStaticObjects(uint32_t Obj1, uint32_t Obj2)
{
    // Swap X and Y
    std::swap(XPositions[Obj1], XPositions[Obj2]);
    std::swap(YPositions[Obj1], YPositions[Obj2]);

    // Swap instance pointers
    std::swap(ObjectInstances[Obj1], ObjectInstances[Obj2]);

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


    // Swap instance pointers
    std::swap(ObjectInstances[Obj1], ObjectInstances[Obj2]);

    // Update object's internal vars
    ObjectInstances[Obj1]->EngineIdx = Obj1;
    ObjectInstances[Obj2]->EngineIdx = Obj2;
}

}