#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefList(Type, Name) \
typedef struct { \
    Type *Items; \
    size_t Size; \
    size_t Capacity; \
} Name; \
\
Type Name##_Get(const Name *Arr, size_t Idx) { \
    return Arr->Items[Idx]; \
} \
\
bool Name##_Add(Name *Arr, Type Item) { \
    const bool H = Name##_EnsureListCapacity(Arr); \
    if(!H) {return false;} \
    \
    Arr->Items[Arr->Size++] = Item; \
    return true; \
} \
\
void Name##_Remove(Name *Arr, size_t Idx) { \
    memmove(&Arr->Items[Idx], &Arr->Items[Idx + 1], (Arr->Size - Idx) * sizeof(Type)); \
    Arr->Size--; \
} \
\
void Name##_RemoveSwap(Name *Arr, size_t Idx) { \
    Arr->Items[Idx] = Arr->Items[Arr->Size - 1]; \
    Arr->Size--; \
} \
bool Name##_Insert(Name *Arr, size_t Idx, Type Item) { \
    if(Idx < 0 || Idx > Arr->Size) { \
        return false; \
    } \
    const bool L = EnsureListCapacity(Arr); \
    if(!L) {return false;} \
    \
    memmove(&Arr->Items[Idx + 1], &Arr->Items[Idx], (Arr->Size - Idx - 1) * sizeof(Type)); \
    \
    Arr->Items[Idx] = Item; \
    Arr->Size++; \
    return true; \
} \
\
bool Name##_Init(Name *Arr) { \
    Arr->Size = 0; \
    Arr->Capacity = 16; \
    Arr->Items = malloc(Arr->Capacity * sizeof(Type)); \
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
void Name##_Set(Name*Arr, size_t Idx, Type Item) { \
    Arr->Items[Idx] = Item; \
} \
\
void Name##_Clear(Name *Arr) { \
    Arr->Size = 0; \
} \
\
bool Name##_Reserve(Name *Arr, size_t Capacity) {   \
    Type *NewItems = realloc(Arr->Items, Capacity * sizeof(Type)); \
    if(NewItems == NULL) { \
        return false; \
    } \
    Arr->Capacity = Capacity; \
    Arr->Items = NewItems; \
    return true; \
} \
\
void Name##_Reverse(Name *Arr) { \
    for (size_t i = 0; i < Arr->Size / 2; i++) { \
        Type Temp = Arr->Items[i]; \
        Arr->Items[i] = Arr->Items[Arr->Size - 1 - i]; \
        Arr->Items[Arr->Size - 1 - i] = Temp; \
    } \
} \
\
bool Name##_BulkAdd(Name *Arr, Type *Items, size_t Length) { \
    if (!Name##_EnsureCertainCapacity(Arr, Length)) return false; \
    memcpy(&Arr->Items[Arr->Size], Items, Length * sizeof(Type)); \
    Arr->Size += Length; \
    return true; \
} \
\
bool Name##_EnsureCapacity(Name *Arr) { \
    if(Arr->Size >= Arr->Capacity) { \
        size_t NewCapacity = Arr->Capacity * 2; \
        Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Arr->Items = NewItems; \
        Arr->Capacity = NewCapacity; \
        return true; \
    } \
    return true; \
} \
\
bool Name##_EnsureCertainCapacity(Name *Arr, size_t Length) { \
    if(Arr->Capacity - Arr->Size < Length) { \
        size_t NewCapacity = Arr->Capacity + Length; \
        Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Arr->Items = NewItems; \
        Arr->Capacity = NewCapacity; \
        return true; \
    } \
    return true; \
} \
\

#define DefSimpleList(Type, Name) { \
DefList(Type, Name)\
bool Name##_Contains(const Name *Arr, Type Item) { \
    const size_t Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(size_t i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return true;} \
    } \
    return false; \
} \
\
size_t Name##_Find(const Name *Arr, Type Item) { \
    const size_t Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(size_t i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return i;} \
    } \
    return -1; \
} \
\
}