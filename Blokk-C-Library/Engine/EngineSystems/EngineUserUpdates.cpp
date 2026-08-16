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

void ObjectManager::ProcessDoubleUpdateCommands(DoubleFieldUpdate Command)
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

// TODO: Finish
void ObjectManager::ProcessAddCommands(ObjectCreationParams Fields)
{
    float XVel = Fields.Velocity.x;
    float YVel = Fields.Velocity.y;

    // If creating a static object
    if(XVel == 0 && YVel == 0)
    {
        // Add position to static
        StaticXPositions.push_back(Fields.Position.x);
        StaticYPositions.push_back(Fields.Position.y);
    } 
    else // If creating a dynamic object
    {
        // TODO: Update Object's interval vars, add velocity
        
        // Add position to static
        DynamicXPositions.push_back(Fields.Position.x);
        DynamicYPositions.push_back(Fields.Position.y);


    }
}
