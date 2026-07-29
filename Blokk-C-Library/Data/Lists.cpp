
template<typename Type>
class SimpleList 
{
    static_assert(
        std::is_trivially_copyable_v<Type>,
        "SimpleList only supports trivially copyable values"
    );

protected:
    Type *Items;
    size_t Size;
    size_t Capacity;

private:

    // Reserve space for the list
    bool Reserve(size_t TargetCapacity)
    {
        // Safety check
        BLOKK_ReturnIfFalse(TargetCapacity < Capacity, false);

        // Reallocate the list
        Type *NewItems = (Type*)realloc(Items, TargetCapacity * sizeof(Type));

        // Handle reallocation failiure
        if(NewItems == nullptr) {
            return false;
        }
    
        // Update the right fields
        Capacity = TargetCapacity;
        Items = NewItems;
    
        return true;
    }
        


    // Ensure that the list has enough space for another item 
    bool EnsureCapacity()
    {
        // Check if there isn't enough space
        if(Size >= Capacity)
        {
            // Double the capacity
            size_t NewCapacity = Capacity * 2;
        
            return Reserve(NewCapacity);
        }
    
        return true;
    }



    /* Ensure that the list has enough 
    space for a certain number of items */
    bool EnsureCertainCapacity(size_t Length)
    {
        // Check if the list can store the amount of items
        if(Capacity - Size < Length)
        {
            return Reserve(Size + Length);
        }
    
        return true;
    }

public:

    // Constructor
    SimpleList(size_t StartingCapacity = 16):
        Items(nullptr),
        Size(0),
        Capacity(StartingCapacity)
    {
        // Allocate space for the items
        Items = (Type*)malloc(Capacity * sizeof(Type));

        // Handle allocation failiure
        if(Items == nullptr) {
            Capacity = 0;
        }
    }
    

    // Destructor
    ~SimpleList()
    {
        // Free items to prevent memory leaks
        free(Items);
    }

    // Get a pointer to an item at an index
    Type* GetPtr(size_t Idx) 
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx >= Size, nullptr);

        return &Items[Idx];
    }


    // Get an item at an index
    Type Get(size_t Idx)
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx >= Size, (Type){0});

        return Items[Idx];
    }


    // Add an item to the back of the list
    bool Add(const Type& TargetItem) 
    {
        // Make sure the list has enough capacity
        const bool H = EnsureCapacity();
        if (!H) return false;
        
        // Set the item
        Items[Size++] = TargetItem;
        
        return true;
    }



    // Remove an item from the list
    void Remove(size_t Idx)
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx >= Size, );

        // Shift the entire array one to the left
        size_t Count = (Size - Idx - 1) * sizeof(Type); // Helper
        memmove(&Items[Idx], &Items[Idx + 1], Count);

        // Update the size
        Size--;
    }



    // Remove an index by swapping it with the last index
    void RemoveSwap(size_t Idx)
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx >= Size, );

        // Replace the item with the last item
        Items[Idx] = Items[Size - 1];

        // Update the size
        Size--;
    }



    // Insert an item at a specific index
    bool Insert(const Type& Item, size_t Idx)
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx > Size, false);

        // Make sure there's enough space
        const bool L = EnsureCapacity();
        if(!L) return false;

        // Helper
        Type* TargetItemPtr = GetPtr(Idx);

        // Shift the array right by one
        size_t Size = (Size - Idx) * sizeof(Type);
        memmove(GetPtr(Idx + 1), TargetItemPtr, Size);

        // Replace the target Idx with the item
        Items[Idx] = Item;

        // Update the size
        Size++;

        return true;
    }



    // Check if the list is empty - O(1)
    bool IsEmpty() 
    {
        return Size == 0;
    }



    // Replace an item in the list - O(1)
    void Set(const Type& Item, size_t Idx)
    {
        // Safety check
        BLOKK_ReturnIfFalse(Idx >= Size, );

        // Replace the value with the target
        Items[Idx] = Item;
    }



    // Set all the items of the list to a specific value
    void SetAll(const Type& Item)
    {
        // Helper
        int Count = Size;

        // Loop through the list
        for(int i = 0; i < Count; i++) 
        {
            // Set the item to the target
            Items[i] = Item;
        }
    }



    // Swap 2 indexes in a list
    void Swap(size_t FirstIdx, size_t SecondIdx)
    {
        Type TempVar = Items[FirstIdx];

        // Copy the second item to the first
        Items[FirstIdx] = Items[SecondIdx];

        // Copy the first item onto the second
        Items[SecondIdx] = TempVar;
    }



    // Clear the list - O(1)
    void Clear() 
    {
        Size = 0;
    }



    // Reverse the list
    void Reverse()
    {
        // Helper
        size_t Count = Size / 2;
    
        // Loop
        for(int i = 0; i < Count; i++)
        {
            // Swap the item from the start and end
            Swap(i, Size - i - 1);
        }
    }



    // Add a bunch of items
    bool BulkAdd(const Type* TargetItems, size_t ItemsLength)
    {
        if (!EnsureCertainCapacity(ItemsLength))
            return false;

        for (size_t i = 0; i < ItemsLength; i++)
            Items[Size + i] = TargetItems[i];

        Size += ItemsLength;
        return true;
    }



    // Delete a bunch of items
    bool BulkDelete(size_t StartIdx, size_t Length)
    {
        // Safety check
        BLOKK_ReturnIfFalse(StartIdx >= Size, false);

        // Shift the items after into their place
        size_t Count = (Size - StartIdx - Length) * sizeof(Type); // Helper
        memmove(&Items[StartIdx], &Items[StartIdx + Length], Count);

        // Update the size
        Size -= Length;

        return true;
    }
};