#include <utility>

#include "raylib.h"
#include "GameTypes.hpp"
#include "GameObject.hpp"

void ObjectManager::ProcessFieldUpdateCommand(FieldUpdate Command)
{
    switch(Command.Type) 
    {
        case CommandTypes::Destroy:
            
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

void ObjectManager::ProcessDoubleUpdateCommand(DoubleFieldUpdate Command)
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

void ObjectManager::ProcessAddCommands(ObjectCreationParams Fields)
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

        StaticObjectCount++;
    } 
    else // If creating a dynamic object
    {
        // Add position
        XPositions.push_back(Fields.Position.x);
        YPositions.push_back(Fields.Position.y);

        // Swap position to keep with dynamic object data
        std::swap(XPositions[DynamicObjectCount], XPositions[ObjectCount - 1]);
        std::swap(YPositions[DynamicObjectCount], YPositions[ObjectCount - 1]);

        // Set it's idx
        Fields.Object->EngineIdx = DynamicObjectCount;

        // Add velocity
        XVelocities.push_back(XVel);
        YVelocities.push_back(YVel);

        DynamicObjectCount++;
    }

    ObjectCount++;
}

void ObjectManager::SwapObjects(uint32_t Obj1, uint32_t Obj2)
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

