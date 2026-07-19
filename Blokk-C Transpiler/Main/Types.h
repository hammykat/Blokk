#include "Lists.h"
#include "ErrorSystem.h"

#ifndef TYPES_H
#define TYPES_H

// Create debugging helpers
typedef struct {
    int Line, Number;
} CharacterData;

typedef union {
    CharExpectedError CharExpected;
    ErrorTypes ErrorType;
} ErrorLogEntry;

// Ranges
typedef struct {
    int Start, End;
} Range;

// Strings
typedef struct {
    char *Letters;
    int Length;
} String;

// Absract Syntax Tree
typedef struct {
    char *Name;
    int Type;
} VariableDeclaration;
typedef struct {
    char *Name;
    int ValueType;
    int CollectionType;
} CollectionDeclaration;

// Make list types
DefList(Error, ErrorList)
DefList(VariableDeclaration, VariableDeclarationList)

typedef bool (*ParseSectionFunction) (LanguageParser*);
typedef struct {
    CharacterData CurrentCharData;
    int CurrentFileIdx;
    int CurrentLineNum; // Current line
    int CurrentLineCharNum; // Current line character number
    int FileLength;
    char *File;

    Range CurrentReadingRange;
    char *NextWord;
    int CurrentReadingIndex;
    int TempIdx;

    ErrorList ErrorLog;
    VariableDeclarationList VariableDeclarations;

    ParseSectionFunction ParseSection;

    /*TODO: Link ParseSection function and others to 
    this struct via pointers */

} LanguageParser;

#endif