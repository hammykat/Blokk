#include "DebugSystem.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// UNFINISHED
#define DefDict(Type, Name) \
typedef struct { \
    char *Key; \
    Type Value; \
} Name##Entry; \
\
DefList(Name##Entry, Name##Bucket) \
\
typedef struct { \
    Name##Bucket *Buckets; \
    int BucketCount; \
    int Count; \
} Name##Dict; \
\
Type GetFromDict##Name(Name *Dict, int Idx) { \
    return Dict->Items[Idx]; \
} \
\
bool AddTo##Name(Name *Dict, Type Item) { \
    if(Dict->Size >= Dict->Capacity) { \
        Dict->Capacity = Dict->Capacity > 0 ? Dict->Capacity * 2 : 4; \
        Type *NewItems = realloc(Dict->Items, Dict->Capacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Dict->Items = NewItems; \
    } \
\
    Dict->Items[Dict->Size++] = Item; \
    return true; \
} \
\
void RemoveFrom##Name(Name *Dict, int Idx) { \
    for (int i = Idx; i < Dict->Size - 1; i++) { \
        Dict->Items[i] = Dict->Items[i + 1]; \
    } \
    Dict->Size--; \
} \
\
void RemoveSwapFrom##Name(Name *Dict, int Idx) { \
    Dict->Items[Idx] = Dict->Items[Dict->Size - 1]; \
    Dict->Size--; \
} \
bool InsertInto##Name(Name *Dict, Type Item, int Idx) { \
    if(Idx < 0 || Idx > Dict->Size) { \
        return false; \
    } \
    if(Dict->Size >= Dict->Capacity) {            \
        int NewCapacity = Dict->Capacity > 0 ? Dict->Capacity * 2 : 4; \
        Type *NewItems = realloc(Dict->Items, NewCapacity * sizeof(Type)); \
        if(NewItems == NULL) { \
            return false; \
        } \
        Dict->Items = NewItems; \
        Dict->Capacity = NewCapacity; \
    } \
    for(int i = Dict->Size; i > Idx; i--) { \
        Dict->Items[i] = Dict->Items[i - 1]; \
    } \
    Dict->Items[Idx] = Item; \
    Dict->Size++; \
    return true; \
} \
\
bool Init##Name(Name *Dict) { \
    Dict->Size = 0; \
    Dict->Capacity = 5; \
    Dict->Items = malloc(5 * sizeof(Type)); \
    if(Dict->Items == NULL) { \
        Dict->Capacity = 0; \
        return false; \
    } \
    return true; \
} \
\
void Destroy##Name(Name *Dict) { \
    free(Dict->Items); \
    Dict->Items = NULL; \
    Dict->Size = 0; \
    Dict->Capacity = 0; \
} \
\
bool Name##IsEmpty(Name *Dict) { \
    return Dict->Size == 0; \
} \
\
void SetIn##Name(Name*Dict, int Idx, Type Item) { \
    Dict->Items[Idx] = Item; \
} \
\
void Clear##Name(Name *Dict) { \
    Dict->Size = 0; \
} \
\
bool Reserve##Name(Name *Dict, int Capacity) {   \
    Type *NewItems = realloc(Dict->Items, Capacity * sizeof(Type)); \
    if(NewItems == NULL) { \
        return false; \
    } \
    Dict->Capacity = Capacity; \
    Dict->Items = NewItems; \
    return true; \
} \
\
bool Name##Contains(Name *Dict, Type Item) { \
    const int Count = Dict->Size; \
    Type *Items = Dict->Items; \
    for(int i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return true;} \
    } \
} \
\
int FindIn##Name(Name *Dict, Type Item) { \
    const int Count = Dict->Size; \
    Type *Items = Dict->Items; \
    for(int i = 0; i < Count; i++) { \
        if(Items[i] == Item) {return i;} \
    } \
    return -1; \
} \
\
void Reverse##Name(Name *Dict) { \
    for (int i = 0; i < Dict->Size / 2; i++) { \
        Type Temp = Dict->Items[i]; \
        Dict->Items[i] = Dict->Items[Dict->Size - 1 - i]; \
        Dict->Items[Dict->Size - 1 - i] = Temp; \
    } \
}