#include "ErrorSystem.h"
#include "BlokkLangTranslator.c"

String GetCharExpectedError(CharExpectedError *Error) {

    // Make the error message
    int Length = snprintf(NULL, 0, 
    "Expected character '%c', got '%c' (At Line %d, character %d)",
    Error->Expected, Error->Got, Error->Place.Line, Error->Place.Number
    );

    // Create the buffer
    int Size = (Length + 1) * sizeof(char);
    char *Buffer = malloc(Size);
    if(Buffer == NULL) return (String){0};

    // Write the text to the buffer
    snprintf(Buffer, Size, 
    "Expected character '%c', got '%c' (At Line %d, character %d)",
    Error->Expected, Error->Got, Error->Place.Line, Error->Place.Number
    );

    // Store it
    String Result = {
        .Letters = Buffer,
        .Length = Length
    };

    // Return
    return Result;
}

// TODO: FINISH THIS
String GetUnidentifiedWordError(UnidentifiedWordError *Error) {
    // Make the error message
    // TODO: Add a 'did you mean xyz'? feature
    int Length = snprintf(NULL, 0, 
    "Unidentified word '%s' (At Line %d, character %d)",
    0
    );

    // Create the buffer
    int Size = (Length + 1) * sizeof(char);
    char *Buffer = malloc(Size);
    if(Buffer == NULL) return (String){0};

    // Write the text to the buffer
    snprintf(Buffer, Size, 
    "Unidentified word '%s' (At Line %d, character %d)",
    Error->Word.Letters, Error->StartPlace.Line, Error->StartPlace.Number
    );

    // Store it
    String Result = {
        .Letters = Buffer,
        .Length = Length
    };

    // Return
    return Result;
}