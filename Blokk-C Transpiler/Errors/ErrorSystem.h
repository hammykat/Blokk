#ifndef ERRORSYSTEM_H
#define ERRORSYSTEM_H

#include "Types.h"

typedef enum {
    CharExpected,
    UnidentifiedWord
} ErrorTypes;

typedef struct {
    char Expected, Got;
    CharacterData Place;
} CharExpectedError;

typedef struct {
    String Word;
    CharacterData StartPlace;
} UnidentifiedWordError;

typedef union {
    CharExpectedError CharExpected;
    UnidentifiedWordError UnidentifiedWord;
    ErrorTypes Type;
} Error;

String GetCharExpectedError(CharExpectedError *Error);
String GetUnidentifiedWordError(UnidentifiedWordError *Error);

#endif