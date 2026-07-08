#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefList(Type, Name)                     \
typedef struct {                                \
    Type *Items;                                \
    int Size;                                   \
    int Capacity;                               \
} Name;                                         \
\
Type GetFrom##Name(Name *Arr, int Idx) {        \
    return Arr->Items[Idx];                     \
}                                               \
\
bool AddTo##Name(Name *Arr, Type Item) {        \
    if(Arr->Size >= Arr->Capacity) {            \
        Arr->Capacity = Arr->Capacity > 0 ? Arr->Capacity * 2 : 4; \
        Type *NewItems = realloc(Arr->Items, Arr->Capacity * sizeof(Type)); \
        if(NewItems == NULL) {                  \
            return false;                       \
        }                                       \
        Arr->Items = NewItems;                  \
    }                                           \
\
    Arr->Items[Arr->Size++] = Item;             \
    return true;                                \
}                                               \
\
void RemoveFrom##Name(Name *Arr, int Idx) {     \
    for (int i = Idx; i < Arr->Size - 1; i++) { \
        Arr->Items[i] = Arr->Items[i + 1];      \
    }                                           \
    Arr->Size--;                                \
}                                               \
\
void RemoveSwapFrom##Name(Name *Arr, int Idx) { \
    Arr->Items[Idx] = Arr->Items[Arr->Size - 1];\
    Arr->Size--;                                \
}                                               \
bool InsertInto##Name(Name *Arr, Type Item, int Idx) {  \
    if(Idx < 0 || Idx > Arr->Size) {            \
        return false;                           \
    }                                           \
    if(Arr->Size >= Arr->Capacity) {            \
        int NewCapacity = Arr->Capacity > 0 ? Arr->Capacity * 2 : 4; \
        Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) {                  \
            return false;                       \
        }                                       \
        Arr->Items = NewItems;                  \
        Arr->Capacity = NewCapacity;            \
    }                                           \
    for(int i = Arr->Size; i > Idx; i--) {      \
        Arr->Items[i] = Arr->Items[i - 1];      \
    }                                           \
    Arr->Items[Idx] = Item;                     \
    Arr->Size++;                                \
    return true;                                \
}                                               \
\
bool Init##Name(Name *Arr) {                    \
    Arr->Size = 0;                              \
    Arr->Capacity = 5;                          \
    Arr->Items = malloc(5 * sizeof(Type));      \
    if(Arr->Items == NULL) {                    \
        Arr->Capacity = 0;                      \
        return false;                           \
    }                                           \
    return true;                                \
}                                               \
\
void Destroy##Name(Name *Arr) { \
    free(Arr->Items);                           \
    Arr->Items = NULL;                          \
    Arr->Size - 0;                              \
    Arr->Capacity = 0;                          \
}
