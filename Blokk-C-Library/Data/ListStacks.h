#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefListStack(Type, Name) \
DefList(Type, Name) \
\
bool Name##_Push(Name *Stack, Type Item) { \
    return AddTo##Name(Stack, Item); \
} \
\
Type Name##_Pop(Name *Stack) { \
    const Type Result = GetFrom##Name(Stack, Stack->Size - 1); \
    DeleteFrom##Name(Name, Stack->Size - 1); \
    return Result; \
} \
\
Type Name##_Peek(Name *Stack) { \
    return GetFrom##Name(Stack, Stack->Size - 1); \
} \
\
int Name##_Size(const Name *Stack) { \
    return Stack->Size; \
}