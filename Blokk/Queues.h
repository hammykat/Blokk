#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefQueue(Type, Name) \
typedef struct { \
    Type *Items; \
    int Start; \
    int End; \
    int Capacity; \
} Name; \
\
Type Name##_Get(Name *Stack, int Idx) { \
    const int TargetIdx = Stack->Start + Idx; \
    BLOKK_ReturnIfFalse(TargetIdx > Stack->End, (Type){0}); \
    \
    return Stack->Items[TargetIdx]; \
} \
\
bool Name##_Enqueue(Name *Stack, Type Item) { \
    if(Stack->End + 1 >= Stack->Capacity) { \
        Stack->Capacity *= 2; \
        Type *NewItems = realloc(Stack->Items, Stack->Capacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Stack->Items = NewItems; \
    } \
    \
    Stack->End++; \
    Stack->Items[Stack->End] = Item; \
    return true; \
} \
\
void Name##_Remove(Name *Stack, int Idx) { \
    BLOKK_ReturnIfFalse(Idx < Stack->Start || Idx > Stack->End,); \
    \
    const int Start = Stack->Start; const int End = Stack->End; \
    for (int i = Idx; i <= End - 1; i++) { \
        Stack->Items[Start + i] = Stack->Items[Start + i + 1]; \
    } \
    Stack->End--; \
} \
\
void Name##_RemoveSwap(Name *Stack, int Idx) { \
    const int TargetIdx = Stack->Start + Idx; \
    BLOKK_ReturnIfFalse(TargetIdx < Stack->End,); \
    \
    Stack->Items[Idx] = Stack->Items[Stack->End]; \
    Stack->End--; \
} \
\
bool Name##_Insert(Name *Stack, Type Item, int Idx) { \
    BLOKK_ReturnIfFalse(Idx < Stack->Start || Idx > Stack->End, false); \
    \
    if(Stack->End >= Stack->Capacity) {            \
        int NewCapacity = Stack->Capacity * 2; \
        Type *NewItems = realloc(Stack->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Stack->Items = NewItems; \
        Stack->Capacity = NewCapacity; \
    } \
    \
    for(int i = Stack->End; i > Idx; i--) { \
        Stack->Items[i] = Stack->Items[i - 1]; \
    } \
    Stack->Items[Idx] = Item; \
    Stack->End++; \
    return true; \
} \
\
bool Name##_Init(Name *Stack) { \
    Stack->Start = 0; \
    Stack->End = -1; \
    Stack->Capacity = 5; \
    Stack->Items = malloc(5 * sizeof(Type)); \
    if(Stack->Items == NULL) { \
        Stack->Capacity = 0; \
        return false; \
    } \
    return true; \
} \
\
void Name##_Destroy(Name *Stack) { \
    free(Stack->Items); \
    Stack->Items = NULL; \
    Stack->Start = 0; \
    Stack->End = -1; \
    Stack->Capacity = 0; \
} \
\
bool Name##_IsEmpty(Name *Stack) { \
    return Stack->End == -1; \
} \
\
void Name##_SetIn(Name*Stack, int Idx, Type Item) { \
    Stack->Items[Stack->Start + Idx] = Item; \
} \
\
void Name##_Clear(Name *Stack) { \
    Stack->Start = 0; \
    Stack->End = -1; \
} \
\
bool Name##_Reserve(Name *Stack, int Capacity) {   \
    Type *NewItems = realloc(Stack->Items, Capacity * sizeof(Type)); \
    if(NewItems == NULL) { \
        return false; \
    } \
    Stack->Capacity = Capacity; \
    Stack->Items = NewItems; \
    return true; \
} \
\
bool Name##_Contains(Name *Stack, Type Item) { \
    const int Start = Stack->Start; const int End = Stack->End; \
    Type *Items = Stack->Items; \
    for(int i = Start; i <= End; i++) { \
        if(Items[i] == Item) {return true;} \
    } \
    return false; \
} \
\
int Name##_Find(Name *Stack, Type Item) { \
    const int Start = Stack->Start; const int End = Stack->End; \
    Type *Items = Stack->Items; \
    for(int i = Start; i <= End; i++) { \
        if(Items[i] == Item) {return i;} \
    } \
    return -1; \
} \
\
void Name##_Reverse(Name *Stack) { \
    CleanUp##Name(Stack); \
    const int End = Stack->End; \
    for (int i = 0; i <= End / 2; i++) { \
        Type Temp = Stack->Items[i]; \
        Stack->Items[i] = Stack->Items[Stack->End - 1 - i]; \
        Stack->Items[End - i] = Temp; \
    } \
} \
\
Type Name##_PeekFront(Name *Stack) { \
    return Stack->Items[Stack->Start]; \
} \
\
Type Name##_PeekBack(Name *Stack) { \
    return Stack->Items[Stack->End]; \
} \
\
Type Name##_Dequeue(Name *Stack) { \
    Type Result = Stack->Items[Stack->Start]; \
    Stack->Start++; \
    return Result; \
} \
\
void Name##_CleanUp(Name *Stack) { \
    Type *Items = Stack->Items; \
    int End = Stack->End; int Start = Stack->Start; \
    Type *NewItems = malloc((End - Start + 1) * sizeof(Type)); \
    for(int i = Start; i <= End; i++) { \
        NewItems[i - Start] = Items[i]; \
    } \
    free(Stack->Items); \
    Stack->Items = NewItems; \
    Stack->Start = 0; \
    Stack->End = End - Start; \
}
