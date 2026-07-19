#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameTypes.h"

Objects* CreateObjectArrays(size_t Count) {

    Objects WorldObjects;

    // Calculate sizes
    size_t PositionSize = Count * sizeof(Vector2);
    size_t VelocitySize = Count * sizeof(Vector2);
    size_t CollisionsSize = Count * sizeof(Collider);
    size_t AnimationsSize = Count * sizeof(ObjectAnimations);

    // Allocate space for arrays
    void *Memory = malloc(PositionSize + VelocitySize + CollisionsSize + AnimationsSize);
    if(Memory == NULL) return NULL;

    // Calculate positions and assign
    WorldObjects.Position = Memory;
    WorldObjects.Velocity = Memory + PositionSize;
    WorldObjects.Collisions = Memory + PositionSize + VelocitySize;
    WorldObjects.Animations = Memory + PositionSize + VelocitySize + CollisionsSize;

    // Allocate space for valid indexes
    int Size = Count * sizeof(int);
    void *IndexMemory = malloc(Size * 4);
    GameObjectValidIndexes *ValidIdx = &WorldObjects.ValidIndexes;

    // Calculate positions and assign
    ValidIdx->ValidVisibleCount = IndexMemory;
    ValidIdx->ValidVelCount = IndexMemory + Size;
    ValidIdx->ValidCollisionCount = IndexMemory + (Size * 2);
    ValidIdx->ValidAnimCount = IndexMemory + (Size * 3);

    // Return it
    return &WorldObjects;
}

// Load the game objects
Objects* LoadObjects(GameObjectList *WorldObjects) {
    int Count = WorldObjects->Size;
    Objects *GameObjects = CreateObjectArrays(Count);
    GameObjectValidIndexes *ValidIndexes = &GameObjects->ValidIndexes;

    GameObject *CurrentObject;
    for(int i = 0; i < Count; i++) {

        CurrentObject = &WorldObjects->Items[i];

        // Add it to all the lists
        GameObjects->Position[i] = CurrentObject->Position;
        GameObjects->Velocity[i] = CurrentObject->Velocity;
        GameObjects->Collisions[i] = CurrentObject->Collisions;
        GameObjects->Animations[i] = CurrentObject->Animations;

        // Add it to the right index lists

        // Visible
        if(!CurrentObject->IsVisible) {
            ValidIndexes->Visible[ValidIndexes->ValidVisibleCount] = i;
            ValidIndexes->ValidVisibleCount++;
        }

        // Position
        if(!CurrentObject->IsStatic) {
            ValidIndexes->Velocity[ValidIndexes->ValidVelCount] = i;
            ValidIndexes->ValidVelCount++;
        }

        // Collisions
        if(!CurrentObject->IsCollidable) {
            ValidIndexes->Visible[ValidIndexes->ValidCollisionCount] = i;
            ValidIndexes->ValidCollisionCount++;
        }

        // Animations
        if(!CurrentObject->IsAnimated) {
            ValidIndexes->Animations[ValidIndexes->ValidAnimCount] = i;
            ValidIndexes->ValidAnimCount++;
        }
    }

    return GameObjects;
}

void UpdatePositions(Vector2 *PosArray, Vector2 *VelArray, int *ValidIdx, int Count) {
    for(int i = 0; i < Count; i++) {

        // Get helper vars
        int Idx = ValidIdx[i];
        Vector2 *Position = &PosArray[Idx];
        Vector2 *Velocity = &VelArray[Idx];
        
        // Update position
        Position->x += Velocity->x;
        Position->y += Velocity->y;
    }
}


