#include "../Debug/DebugSystem.h"
#include "../../include/raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

You'll see many BLOKK_ReturnIfFalse function calls
It is defined in Debug/DebugSystem.h

*I used multiline comments instead of single line
because it made the syntax highlighting be wierd and
highlight the \ at the end of the line green too,
which I didn't like:P

*/

typedef bool (*CompareFunction)(const void *, const void *);

#define DefList(Type, Name)                                                    \
    typedef struct                                                             \
    {                                                                          \
        /* Stores the items */                                                 \
        Type *Items;                                                           \
        \
        /* Stores the current amount of items */                               \
        size_t Size;                                                           \
        \
        /* Stores the max capacity of items */ \
        size_t Capacity;                                                       \
    } Name;                                                                    \
    \
    \
    \
    /* Function declarations */                                                \
    Type Name##_Get(const Name *Arr, size_t Idx);                              \
    Type *Name##_GetPtr(const Name *Arr, size_t Idx);                          \
    bool Name##_Add(Name *Arr, Type Item);                                     \
    void Name##_Remove(Name *Arr, size_t Idx);                                 \
    void Name##_RemoveSwap(Name *Arr, size_t Idx);                             \
    bool Name##_Insert(Name *Arr, size_t Idx, Type Item);                      \
    bool Name##_Init(Name *Arr);                                               \
    void Name##_Destroy(Name *Arr);                                            \
    bool Name##_IsEmpty(const Name *Arr);                                      \
    void Name##_Set(Name *Arr, size_t Idx, Type Item);                         \
    void Name##_Clear(Name *Arr);                                              \
    bool Name##_Reserve(Name *Arr, size_t Capacity);                           \
    void Name##_Reverse(Name *Arr);                                            \
    bool Name##_BulkAdd(Name *Arr, Type *Items, size_t Length);                \
    bool Name##_BulkDelete(Name *Arr, size_t StartIdx, size_t EndIdx);         \
    bool Name##_EnsureCapacity(Name *Arr);                                     \
    bool Name##_EnsureCertainCapacity(Name *Arr, size_t Length);               \
    \
    \
    \
    /* Function definitions */                                                 \
    \
    \
    /* Get an item at a specific index - O(1) */                               \
    Type Name##_Get(const Name *Arr, size_t Idx)                               \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, (Type){0});                      \
        \
        /* Get item, return it */                                              \
        return Arr->Items[Idx];                                                \
    }                                                                          \
    \
    \
    /* Get a pointer to an item at a specific index - O(1) */                  \
    Type *Name##_GetPtr(const Name *Arr, size_t Idx)                           \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, NULL);                           \
        \
        /* Get the pointer, return it */                                       \
        return &Arr->Items[Idx];                                               \
    }                                                                          \
    \
    \
    /* Add an item to the back of the list - O(1) */                           \
    bool Name##_Add(Name *Arr, Type Item)                                      \
    {                                                                          \
        /* Make sure the list has enough space */                              \
        const bool H = Name##_EnsureCapacity(Arr);                             \
        if (!H) return false; /* Return is it fails to get space */            \
        \
        /* Set the last item to the target item */                             \
        Arr->Items[Arr->Size++] = Item;                                        \
        \
        /* Return */                                                           \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Remove an item from  a specific index, retain                           \
    order by shifting the entire list one to the left - O(n) */                \
    void Name##_Remove(Name *Arr, size_t Idx)                                  \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, );                               \
        \
        /* Shift the items after one to the left */                            \
        size_t Size = (Arr->Size - Idx - 1) * sizeof(Type); /* Helper var */   \
        memmove(&Arr->Items[Idx], &Arr->Items[Idx + 1], Size);                     \
        \
        /* Decrement size */                                                   \
        Arr->Size--;                                                           \
    }                                                                          \
    \
    \
    /* Remove an idex by replacing it with the last index - O(1) */            \
    void Name##_RemoveSwap(Name *Arr, size_t Idx)                              \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, );                               \
        \
        /* Set the item to the last index */                                   \
        Arr->Items[Idx] = Arr->Items[Arr->Size - 1];                           \
        \
        /* Decrement size */                                                   \
        Arr->Size--;                                                           \
    }                                                                          \
    \
    \
    /* Insert an object into the list and retain the order by shifting the list */ \
    /* Best case: O(1), Worst case: O(n) */                                    \
    bool Name##_Insert(Name *Arr, size_t Idx, Type Item)                       \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, false);                          \
        \
        /* Make sure there's enough space */                                   \
        const bool L = Name##_EnsureCapacity(Arr);                             \
        if (!L) return false; /* Return if failed to allocate */               \
            \
        /* Shift the indexes after one to the right */                         \
        size_t Size = (Arr->Size - Idx) * sizeof(Type);                        \
        memmove(&Arr->Items[Idx + 1], &Arr->Items[Idx], Size);                 \
        \
        /* Set the index to the value */                                       \
        Arr->Items[Idx] = Item;                                                \
        \
        Arr->Size++; /* Increment list size */                                 \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Initialize the list, allocate space, assign vars */                     \
    bool Name##_Init(Name *Arr)                                                \
    {                                                                          \
        /* Assign starting values */                                           \
        Arr->Size = 0;                                                         \
        Arr->Capacity = 16;                                                    \
        \
        /* Allocate space */                                                   \
        Arr->Items = malloc(Arr->Capacity * sizeof(Type));                     \
        if (Arr->Items == NULL)                                                \
        { /* Return false if failed */                                         \
            Arr->Capacity = 0;                                                 \
            return false;                                                      \
        }                                                                      \
        \
        /* Return true */                                                      \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Destroy the list - O(1) */                                              \
    void Name##_Destroy(Name *Arr)                                             \
    {                                                                          \
        /* Free items */                                                       \
        free(Arr->Items);                                                      \
        Arr->Items = NULL;                                                     \
        \
        /* Set appropriate fields to 0 */ \
        Arr->Size = 0;                                                         \
        Arr->Capacity = 0;                                                     \
    }                                                                          \
    \
    \
    /* Check if the list is empty - O(1) */                                    \
    bool Name##_IsEmpty(const Name *Arr) { return Arr->Size == 0; }            \
    \
    \
    /* Set (replace) an item in the list - O(1) */                             \
    void Name##_Set(Name *Arr, size_t Idx, Type Item)                          \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Idx >= Arr->Size, );                               \
        \
        /* Set the item */                                                     \
        Arr->Items[Idx] = Item;                                                \
    }                                                                          \
    \
    \
    /* Clear all the items of the array - O(1) */                              \
    void Name##_Clear(Name *Arr) { Arr->Size = 0; }                            \
    \
    \
    /* Reserve space for the list - O(n) */                                    \
    bool Name##_Reserve(Name *Arr, size_t Capacity)                            \
    {                                                                          \
        /* Safety check */                                                     \
        BLOKK_ReturnIfFalse(Capacity < Arr->Capacity, false);                  \
        \
        /* Reallocate the list with enough space */ \
        Type *NewItems = realloc(Arr->Items, Capacity * sizeof(Type));         \
        if (NewItems == NULL) return false;                                    \
        \
        /* Update appropriate fields */ \
        Arr->Capacity = Capacity;                                              \
        Arr->Items = NewItems;                                                 \
        \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Reverse the list (flip it) - O(n) */                                    \
    void Name##_Reverse(Name *Arr)                                             \
    {                                                                          \
        for (size_t i = 0; i < Arr->Size / 2; i++)                             \
        {                                                                      \
            Type Temp = Arr->Items[i];                                         \
            Arr->Items[i] = Arr->Items[Arr->Size - 1 - i];                     \
            Arr->Items[Arr->Size - 1 - i] = Temp;                              \
        }                                                                      \
    }                                                                          \
    \
    \
    /* Add a large amount of objects to the list at once - O(m), m is the amount of items added */ \
    bool Name##_BulkAdd(Name *Arr, Type *Items, size_t Length)                 \
    {                                                                          \
        /* Safety check */ \
        BLOKK_ReturnIfFalse(!Name##_EnsureCertainCapacity(Arr, Length), false);\
        \
        /* Copy the items into the list */                                     \
        memcpy(&Arr->Items[Arr->Size], Items, Length * sizeof(Type));          \
        \
        /* Update size */ \
        Arr->Size += Length;                                                   \
        \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Delete a large amount of items from the list - O(m), m is the amount of items after the endIdx */ \
    bool Name##_BulkDelete(Name *Arr, size_t StartIdx, size_t EndIdx)          \
    {                                                                          \
        size_t Count = EndIdx - StartIdx + 1; /* Helper var */                 \
        \
        /* Safety check */ \
        BLOKK_ReturnIfFalse(StartIdx >= Arr->Size || EndIdx >= Arr->Size, false); \
        \
        size_t Size = (Arr->Size - StartIdx - Count) * sizeof(Type);           \
        memmove(&Arr->Items[StartIdx], &Arr->Items[EndIdx + 1], Size);         \
        Arr->Size -= Count;                                                    \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Make sure there's room for one more item - O(1) or O(n) */ \
    bool Name##_EnsureCapacity(Name *Arr)                                      \
    {                                                                          \
        if (Arr->Size >= Arr->Capacity) /* Check if there's enough space */    \
        {                                                                      \
            /* Double the capacity */                                          \
            size_t NewCapacity = Arr->Capacity * 2;                            \
            \
            /* Reallocate the list to make it fit the size */                  \
            Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type));  \
            /* Return false if reallocation failed */                          \
            if (NewItems == NULL) return false;                                \
            \
            /* Update approproate fields */ \
            Arr->Items = NewItems;                                             \
            Arr->Capacity = NewCapacity;                                       \
            \
            /* Return true, it succeeded */                                    \
            return true;                                                       \
        }                                                                      \
        return true;                                                           \
    }                                                                          \
    \
    \
    /* Make sure there is room for a certain number of items - O(1) or O(n) */ \
    bool Name##_EnsureCertainCapacity(Name *Arr, size_t Length)                \
    {                                                                          \
        if (Arr->Capacity - Arr->Size < Length) /* Check if there's enough space */ \
        {                                                                      \
            /* Make anough room */ \
            size_t NewCapacity = Arr->Capacity + Length;                       \
            \
            /* Reallocate the list to make it fit the size */                  \
            Type *NewItems = realloc(Arr->Items, NewCapacity * sizeof(Type));  \
            /* Return false if reallocation failed */                          \
            if (NewItems == NULL) return false;                                \
            \
            /* Update approproate fields */                                    \
            Arr->Items = NewItems;                                             \
            Arr->Capacity = NewCapacity;                                       \
            \
            /* Return true, it succeeded */                                    \
            return true;                                                       \
        }                                                                      \
        \
        /* Return true, it had enough space */                                 \
        return true;                                                           \
    }
