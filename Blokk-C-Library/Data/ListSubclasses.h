#include "Lists.h"

/* Complex lists can store structs */
#define DefComplexList(Type, Name)                                             \
    \
    /* Define the basic list */                                                \
    DefList(Type, Name)                                                        \
    \
    \
    /* Function declarations */ \
    bool Name##_Contains(const Name *Arr, Type Item, CompareFunction Compare); \
    size_t Name##_Find(const Name *Arr, Type Item, CompareFunction Compare);   \
    \
    \
    /* Check if an item in the list exists (linear scan), O(m), m is which index it is */ \
    bool Name##_Contains(const Name *Arr, Type Item, CompareFunction Compare)  \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        Type *Items = Arr->Items;                                              \
        \
        /* Loop through the list */                                            \
        for (size_t i = 0; i < Count; i++)                                     \
        {                                                                      \
            /* Compare each item to the target item */                         \
            if (Compare(&Items[i], &Item)) return true;                   \
        }                                                                      \
        \
        /* Return false if it wasn't found */                                  \
        return false;                                                          \
    }                                                                          \
    \
    \
    /* Find the index of an item in the list (linear scan), O(m), m is which index it is */ \
    size_t Name##_Find(const Name *Arr, Type Item, CompareFunction Compare)    \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        Type *Items = Arr->Items;                                              \
        \
        /* Loop through the list */                                            \
        for (size_t i = 0; i < Count; i++)                                     \
        {                                                                      \
            /* Compare each item to the target item */                         \
            if (Compare(&Items[i], &Item)) return i;                           \
        }                                                                      \
        \
        /* Return SIZE_MAX if it doesn't exist */                              \
        return SIZE_MAX;                                                       \
    }

// Simple lists can't store structs and can only store direct types
#define DefSimpleList(Type, Name)                                              \
    \
    \
    /* Define the basic list */                                                \
    DefList(Type, Name)                                                        \
    \
    \
    /* Function declarations */ \
    bool Name##_Contains(const Name *Arr, Type Item);                          \
    size_t Name##_Find(const Name *Arr, Type Item);                            \
    \
    \
    /* Function definitions */ \
    bool Name##_Contains(const Name *Arr, Type Item)                           \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        Type *Items = Arr->Items;                                              \
        \
        /* Loop through the list */                                            \
        for (size_t i = 0; i < Count; i++)                                     \
        {                                                                      \
            /* Compare each item to the target item */                         \
            if (Items[i] == Item) return true;                                 \
        }                                                                      \
        \
        /* Return false if it wasn't found */                                  \
        return false;                                                          \
    }                                                                          \
    \
    \
    /* Find the index of an item in the list (linear scan), O(m), m is which index it is */ \
    size_t Name##_Find(const Name *Arr, Type Item)                             \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        Type *Items = Arr->Items;                                              \
        \
        /* Loop through the list */                                            \
        for (size_t i = 0; i < Count; i++)                                     \
        {                                                                      \
            /* Compare each item to the target item */                         \
            if (Items[i] == Item) return i;                                    \
        }                                                                      \
        \
        /* Return SIZE_MAX if it doesn't exist */                              \
        return SIZE_MAX;                                                       \
    }

/* An optimized simple int list only stores integers and is extremely fast
by using effecient algorithms */ 
#define DefOptimizedIntList(Name)                                              \
    \
    \
    /* Define the basic list */                                                \
    DefList(int, Name)                                                        \
    \
    typedef struct { \
        size_t Start; \
        size_t Mid; \
        size_t End; \
    } BinarySearchRange; \
    \
    /* Function declarations */ \
    bool Name##_Contains(const Name *Arr, Type Item);                          \
    size_t Name##_Find(const Name *Arr, Type Item);                            \
    void Name##_MergeSort(const Name *Arr);                                    \
    void Name##_BubbleSort(const Name *Arr);                                   \
    \
    \
    /* Function definitions */ \
    \
    /* Find the index of a value, use binary search */ \
    size_t Name##_Find(const Name *Arr, Type Item)                             \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        BinarySearchRange CurrentRange = {0, 0, Arr->Size};                    \
        \
        /* Check if the target item is the last item */ \
        if(Arr->Items[Arr->Size - 1] == Item) { \
            return Arr->Size - 1; \
        } \
        \
        /* Loop through the list, implement binary search */                   \
        while (CurrentRange.Start < CurrentRange.End)                          \
        {                                                                      \
            CurrentRange.Mid = CurrentRange.Start + (CurrentRange.End - CurrentRange.Start) / 2;                           \
            size_t MidVal = Arr->Items[CurrentRange.Mid];             \
            \
            /* Conditions */                                                   \
            if(MidVal == Item) /* Check if the val at the index is the target */ \
            { \
                return CurrentRange.Mid; \
                \
            } else if(MidVal < Item) /* Check if the val at the index is less than the target */ \
            { \
                /* Search the upper half */ \
                CurrentRange.Start = CurrentRange.Mid + 1; /* Move Start to the midpoint of the Start and End */ \
                \
            } else /* If the val at the index is greater than the target */ \
            { \
                /* Start searching the lower quadrant */ \
                CurrentRange.End = Mid; \
            } \
        }                                                                      \
        \
        /* Return SIZE_MAX if it wasn't found */                               \
        return SIZE_MAX;                                                       \
    }                                                                          \
    \
    \
    bool Name##_Contains(const Name *Arr, Type Item)                           \
    {                                                                          \
        /* Helper vars */                                                      \
        const size_t Count = Arr->Size;                                        \
        BinarySearchRange CurrentRange = {0, 0, Arr->Size};                    \
        \
        /* Check if the target item is the last item */ \
        if(Arr->Items[Arr->Size - 1] == Item) { \
            return Arr->Size - 1; \
        } \
        \
        /* Loop through the list, implement binary search */                   \
        while (CurrentRange.Start < CurrentRange.End)                          \
        {                                                                      \
            CurrentRange.Mid = CurrentRange.Start +                            \
                (CurrentRange.End - CurrentRange.Start) / 2;                   \
            size_t MidVal = Arr->Items[CurrentRange.Mid];                      \
            \
            /* Conditions */                                                   \
            if(MidVal == Item) /* Check if the val at the index is the target */ \
            { \
                return true; \
                \
            } else if(MidVal < Item) /* Check if the val at the index is less than the target */ \
            { \
                /* Search the upper quadrant */ \
                CurrentRange.Start = CurrentRange.Mid + 1; /* Move Start to the midpoint of the Start and End */ \
                \
            } else /* If the val at the index is greater than the target */ \
            { \
                /* Start searching the lower quadrant */ \
                CurrentRange.End = Mid; \
            } \
        }                                                                      \
        \
        /* Return false if it wasn't found */                                  \
        return false;                                                          \
    } \
    \
    void Name##_ExchangeSort(Name *Arr) { \
        int TempVal; \
        int *Items = &Arr->Items; \
        size_t Size = Arr->Size; \
        for(int i = 0; i < Size - i - 1; i++) { \
            for(int u = 0; u < Size; u++) { \
                \
                if(Items[i] < Items[u]) { \
                    TempVal = Items[i]; \
                    Items[i] = Items[u]; \
                    Items[u] = TempVal; \
                } \
            } \
        } \
    }

