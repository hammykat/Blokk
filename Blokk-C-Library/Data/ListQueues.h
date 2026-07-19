#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefListQueue(Type, Name) \
typedef struct { \
    Type *Items; \
    int Start; \
    int End; \
    int Size; \
    int Capacity; \
} Name; \
\
Type Name##_Get(Name *Queue, int Idx) { \
    const int TargetIdx = Queue->Start + Idx; \
    BLOKK_ReturnIfFalse(TargetIdx > Queue->End || TargetIdx < Queue->Start, (Type){0}); \
    \
    return Queue->Items[TargetIdx]; \
} \
\
void Name##_Remove(Name *Queue, int Idx) { \
    BLOKK_ReturnIfFalse(Idx < Queue->Start || Idx > Queue->End,); \
    \
    const int Start = Queue->Start; const int End = Queue->End; \
    for (int i = Idx; i <= End - 1; i++) { \
        Queue->Items[Start + i] = Queue->Items[Start + i + 1]; \
    } \
    Queue->End--; \
} \
\
void Name##_RemoveSwap(Name *Queue, int Idx) { \
    const int TargetIdx = Queue->Start + Idx; \
    BLOKK_ReturnIfFalse(TargetIdx < Queue->End,); \
    \
    Queue->Items[TargetIdx] = Queue->Items[Queue->End]; \
    Queue->End--; \
} \
\
bool Name##_Insert(Name *Queue, Type Item, int Idx) { \
    BLOKK_ReturnIfFalse(Idx < Queue->Start || Idx > Queue->End, false); \
    \
    if(Queue->End >= Queue->Capacity) {            \
        int NewCapacity = Queue->Capacity * 2; \
        Type *NewItems = realloc(Queue->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Queue->Items = NewItems; \
        Queue->Capacity = NewCapacity; \
    } \
    \
    for(int i = Queue->End; i > Idx; i--) { \
        Queue->Items[i] = Queue->Items[i - 1]; \
    } \
    Queue->Items[Idx] = Item; \
    Queue->End++; \
    return true; \
} \
\
bool Name##_Init(Name *Queue) { \
    Queue->Start = 0; \
    Queue->End = -1; \
    Queue->Size = 0; \
    Queue->Capacity = 5; \
    Queue->Items = malloc(5 * sizeof(Type)); \
    if(Queue->Items == NULL) { \
        Queue->Capacity = 0; \
        return false; \
    } \
    return true; \
} \
\
void Name##_Destroy(Name *Queue) { \
    free(Queue->Items); \
    Queue->Items = NULL; \
    Queue->Start = 0; \
    Queue->End = -1; \
    Queue->Capacity = 0; \
} \
\
bool Name##_IsEmpty(Name *Queue) { \
    return Queue->Start > Queue->End; \
} \
\
void Name##_SetIn(Name*Queue, int Idx, Type Item) { \
    Queue->Items[Queue->Start + Idx] = Item; \
} \
\
void Name##_Clear(Name *Queue) { \
    Queue->Start = 0; \
    Queue->End = -1; \
} \
\
bool Name##_Reserve(Name *Queue, int Capacity) {   \
    Type *NewItems = realloc(Queue->Items, Capacity * sizeof(Type)); \
    if(NewItems == NULL) { \
        return false; \
    } \
    Queue->Capacity = Capacity; \
    Queue->Items = NewItems; \
    return true; \
} \
\
bool Name##_Contains(Name *Queue, Type Item) { \
    const int Start = Queue->Start; const int End = Queue->End; \
    Type *Items = Queue->Items; \
    for(int i = Start; i <= End; i++) { \
        if(Items[i] == Item) {return true;} \
    } \
    return false; \
} \
\
int Name##_Find(Name *Queue, Type Item) { \
    const int Start = Queue->Start; const int End = Queue->End; \
    Type *Items = Queue->Items; \
    for(int i = Start; i <= End; i++) { \
        if(Items[i] == Item) {return i;} \
    } \
    return -1; \
} \
\
void Name##_Reverse(Name *Queue) { \
    CleanUp##Name(Queue); \
    const int End = Queue->End; \
    for (int i = 0; i <= End / 2; i++) { \
        Type Temp = Queue->Items[i]; \
        Queue->Items[i] = Queue->Items[Queue->End - 1 - i]; \
        Queue->Items[End - i] = Temp; \
    } \
} \
\
Type Name##_PeekFront(Name *Queue) { \
    return Queue->Items[Queue->Start]; \
} \
\
Type Name##_PeekBack(Name *Queue) { \
    return Queue->Items[Queue->End]; \
} \
\
Type Name##_Dequeue(Name *Queue) { \
    Type Result = Queue->Items[Queue->Start]; \
    Queue->Start++; \
    return Result; \
} \
\
bool Name##_Enqueue(Name *Queue, Type Item) { \
    if(Queue->End + 1 >= Queue->Capacity) { \
        Queue->Capacity *= 2; \
        Type *NewItems = realloc(Queue->Items, Queue->Capacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Queue->Items = NewItems; \
    } \
    \
    Queue->End++; \
    Queue->Items[Queue->End] = Item; \
    return true; \
} \
\
void Name##_Compact(Name *Queue) { \
    Type *Items = Queue->Items; \
    int Start = Queue->Start; \
    int End = Queue->End; \
    \
    Type *NewItems = malloc(Queue->Size * sizeof(Type)); \
    if(NewItems == NULL) return; \
    \
    if(End >= 0 && End < Start) { \
        memcpy(&NewItems, &Queue->Items[Start], (Queue->Capacity - Start + 1) * sizeof(Type)); \
        memcpy(&NewItems, &Queue->Items[0], (End - Start + 1) * sizeof(Type)); \
    } \
    \
    free(Queue->Items); \
    Queue->Items = NewItems; \
    Queue->Start = 0; \
    Queue->End = Count - 1; \
} \
\
void CircularBufferEnd(Name *Queue) { \
    if(!(Queue->Size < Queue->Capacity - 1)) { \
        Queue->End++; \
        return; \
    } else { \
        EnsureQueueCapacity(Queue); \
    } \
} \
\
bool EnsureQueueCapacity(Name *Queue) { \
    const int Count = Queue->End - Queue->Start + 1; \
    if(Queue->Size >= Queue->Capacity) { \
        int NewCapacity = Queue->Capacity * 2; \
        Type *NewItems = realloc(Queue->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Queue->Items = NewItems; \
        Queue->Capacity = NewCapacity; \
        return true; \
    } \
    return true; \
}