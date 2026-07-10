#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefList(Type, Name) \
typedef struct { \
    Type *Items; \
    int Size; \
    int Capacity; \
} Name; \
\
Type Name##_Get(const Name *Arr, int Idx) { \
    return Arr->Items[Idx]; \
} \
\
bool Name##_Add(Name *Arr, Type Item) { \
    const bool H = EnsureListCapacity(Arr); \
    if(!H) {return false;} \
    \
    Arr->Items[Arr->Size++] = Item; \
    return true; \
} \
\
void Name##_Remove(Name *Arr, int Idx) { \
    for (int i = Idx; i < Arr->Size - 1; i++) { \
        Arr->Items[i] = Arr->Items[i + 1]; \
    } \
    Arr->Size--; \
} \
\
void Name##_RemoveSwap(Name *Arr, int Idx) { \
    Arr->Items[Idx] = Arr->Items[Arr->Size - 1]; \
    Arr->Size--; \
} \
bool Name##_Insert(Name *Arr, int Idx, Type Item) { \
    if(Idx < 0 || Idx > Arr->Size) { \
        return false; \
    } \
    const bool L = EnsureListCapacity(Arr); \
    if(!L) {return false;} \
    \
    for(int i = Arr->Size; i > Idx; i--) { \
        Arr->Items[i] = Arr->Items[i - 1]; \
    } \
    Arr->Items[Idx] = Item; \
    Arr->Size++; \
    return true; \
} \
\
bool Name##_Init(Name *Arr) { \
    Arr->Size = 0; \
    Arr->Capacity = 5; \
    Arr->Items = malloc(5 * sizeof(Type)); \
    if(Arr->Items == NULL) { \
        Arr->Capacity = 0; \
        return false; \
    } \
    return true; \
} \
\
void Name##_Destroy(Name *Arr) { \
    free(Arr->Items); \
    Arr->Items = NULL; \
    Arr->Size = 0; \
    Arr->Capacity = 0; \
} \
\
bool Name##_IsEmpty(const Name *Arr) { \
    return Arr->Size == 0; \
} \
\
void Name##_Set(Name*Arr, int Idx, Type Item) { \
    Arr->Items[Idx] = Item; \
} \
\
void Name##_Clear(Name *Arr) { \
    Arr->Size = 0; \
} \
\
bool Name##_Reserve(Name *Arr, int Capacity) {   \
    Type *NewItems = realloc(Arr->Items, Capacity * sizeof(Type)); \
    if(NewItems == NULL) { \
        return false; \
    } \
    Arr->Capacity = Capacity; \
    Arr->Items = NewItems; \
    return true; \
} \
\
bool Name##_Contains(const Name *Arr, Type Item) { \
    const int Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(int i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return true;} \
    } \
    return false; \
} \
\
int Name##_Find(const Name *Arr, Type Item) { \
    const int Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(int i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return i;} \
    } \
    return -1; \
} \
\
void Name##_Reverse(Name *Arr) { \
    for (int i = 0; i < Arr->Size / 2; i++) { \
        Type Temp = Arr->Items[i]; \
        Arr->Items[i] = Arr->Items[Arr->Size - 1 - i]; \
        Arr->Items[Arr->Size - 1 - i] = Temp; \
    } \
} \
\
bool EnsureListCapacity(Name *Arr) { \
    if(Arr->Size >= Arr->Capacity) { \
        int NewCapacity = Arr->Capacity * 2; \
        Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Arr->Items = NewItems; \
        Arr->Capacity = NewCapacity; \
        return true; \
    } \
}
