#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DefStack(Type, Name)                                \
DefList(Type, Name)                                         \
InitList##Name(Name)                                        \
\
bool PushTo##Name(Name *Stack, Type Item) {                 \
    return AddTo##Name(Stack, Item);                        \
}                                                           \
\
Type PopFrom##Name(Name *Stack) {                           \
    const Result = GetFrom##Name(Stack, Stack->Size - 1);   \
    DeleteFrom##Name(Name, Stack->Size - 1);                \
    return Result                                           \
}                                                           \
\
void PeekFrom##Name(Name *Stack) {                          \
    return GetFrom##Name(Stack, Stack->Size - 1);           \
}                                                           \
\
