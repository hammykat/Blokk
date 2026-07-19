#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef bool (*CompareFunction) (const void *, const void *);
#define DefList(Type, Name) \
typedef struct { \
    Type *Items; \
    size_t Size; \
    size_t Capacity; \
} Name; \
\
Type Name##_Get(const Name *Arr, size_t Idx) { \
    BLOKK_ReturnIfFalse(Idx >= Arr->Size, (Type){0}); \
    \
    return Arr->Items[Idx]; \
} \
\
Type Name##_GetPtr(const Name *Arr, size_t Idx) { \
    BLOKK_ReturnIfFalse(Idx >= Arr->Size, (Type){0}); \
    \
    return &Arr->Items[Idx]; \
} \
\
bool Name##_Add(Name *Arr, Type Item) { \
    const bool H = Name##_EnsureCapacity(Arr); \
    if(!H) {return false;} \
    \
    Arr->Items[Arr->Size++] = Item; \
    return true; \
} \
\
void Name##_Remove(Name *Arr, size_t Idx) { \
    BLOKK_ReturnIfFalse(Idx >= Arr->Size,); \
    \
    memmove(&Arr->Items[Idx], &Arr->Items[Idx + 1], (Arr->Size - Idx - 1) * sizeof(Type)); \
    Arr->Size--; \
} \
\
void Name##_RemoveSwap(Name *Arr, size_t Idx) { \
    BLOKK_ReturnIfFalse(Idx >= Arr->Size,); \
    \
    Arr->Items[Idx] = Arr->Items[Arr->Size - 1]; \
    Arr->Size--; \
} \
bool Name##_Insert(Name *Arr, size_t Idx, Type Item) { \
    BLOKK_ReturnIfFalse(Idx >= Arr->Size, false) \
    \
    const bool L = EnsureCapacity(Arr); \
    if(!L) {return false;} \
    \
    memmove(&Arr->Items[Idx + 1], &Arr->Items[Idx], (Arr->Size - Idx) * sizeof(Type)); \
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
    BLOKK_ReturnIfFalse(Idx >= Arr->Size,); \
    \
    Arr->Items[Idx] = Item; \
} \
\
void Name##_Clear(Name *Arr) { \
    Arr->Size = 0; \
} \
\
bool Name##_Reserve(Name *Arr, size_t Capacity) {   \
    BLOKK_ReturnIfFalse(Capacity < Arr->Capacity, false); \
    \
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
    BLOKK_ReturnIfFalse(!Name##_EnsureCertainCapacity(Arr, Length), false); \
    \
    memcpy(&Arr->Items[Arr->Size], Items, Length * sizeof(Type)); \
    Arr->Size += Length; \
    return true; \
} \
\
bool Name##_BulkDelete(Name *Arr, size_t StartIdx, size_t EndIdx) { \
    size_t Count = EndIdx - StartIdx + 1; \
    BLOKK_ReturnIfFalse(StartIdx >= Arr->Size || EndIdx >= Arr->Size, false); \
    \
    memmove(&Arr->Items[StartIdx], &Arr->Items[EndIdx + 1], (Arr->Size - StartIdx - Count) * sizeof(Type)); \
    Arr->Size -= EndIdx - StartIdx; \
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
} 

#define DefComplexList(Type, Name) \
DefList(Type, Name) \
bool Name##_Contains(const Name *Arr, Type Item, CompareFunction Compare) { \
    const size_t Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(size_t i = 0; i < Count; i++) { \
        if(Compare(Arr->Items[i], Item)) {return true;} \
    } \
    return false; \
} \
\
size_t Name##_Find(const Name *Arr, Type Item, CompareFunction Compare) { \
    const size_t Count = Arr->Size; \
    Type *Items = Arr->Items; \
    for(size_t i = 0; i < Count; i++) { \
        if(Compare(Arr->Items[i], Item)) {return i;} \
    } \
    return SIZE_MAX; \
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
    return SIZE_MAX; \
} \
\
}