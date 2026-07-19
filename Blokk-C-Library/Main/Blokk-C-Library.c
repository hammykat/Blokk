#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameTypes.h"

Objects CreateObjectArrays(size_t Count) {

    Objects WorldObjects;

    // Calculate sizes
    size_t PositionSize = Count * sizeof(Vector2);
    size_t VelocitySize = Count * sizeof(Vector2);
    size_t CollisionsSize = Count * sizeof(Collider);
    size_t AnimationsSize = Count * sizeof(ObjectAnimations);
    size_t BoolValSize = Count * sizeof(bool);

    // Get one huge chunk of memory
    void *Memory = malloc(PositionSize + VelocitySize + CollisionsSize + AnimationsSize + (BoolValSize * 4));
    if(Memory == NULL) return (Objects){0};

    // Calculate positions and assign
    WorldObjects.Position = Memory;
    WorldObjects.Velocity = Memory + PositionSize;
    WorldObjects.Animations = Memory + PositionSize + VelocitySize;

    void *TempVar = Memory + PositionSize + VelocitySize + AnimationsSize;
    WorldObjects.IsStatic = TempVar;
    WorldObjects.IsVisible = TempVar + BoolValSize;
    WorldObjects.IsAnimated = TempVar + BoolValSize * 2;
    WorldObjects.IsCollidable = TempVar + BoolValSize * 3;

    // Return it
    return WorldObjects;
}

void GetValidIdx(Objects *WorldObjects, ValidIdx *Output, int Count) {
    int Count = WorldObjects->ObjectCount;

    ValidIdx ValidIndexes;
    void *ValidIndexesMemory = malloc((sizeof(int*) * 3) + (sizeof(size_t) * 3));

    ValidIndexes.Velocity = ValidIndexesMemory;
    ValidIndexes.ValidVelCount = ValidIndexesMemory + sizeof(int*);

    ValidIndexes.Collisions = ValidIndexesMemory + sizeof(int*) + sizeof(size_t);
    // TODO: Finish getting memory for these

    for(int i = 0; i < Count; i++) {

        if(WorldObjects->IsStatic[i]) {

        }
    }
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

