#ifndef LANGUAGE_H
#define LANGUAGE_H

#define CountOf(x) (sizeof(x)/sizeof(x[0]))

typedef struct {
    char *Name;
    int Type;
} TypeDefinition;

typedef struct {
    TypeDefinition *Items;
    int Count;
} TypeTable;

// Probably won't use this:P
enum TokenType {
    TokenIdentifier,
    TokenKeyword,
    TokenNumber,
    TokenString,

    TokenOperator,
    TokenSymbol,

    TokenEndOfFile
};

// Value Types
typedef enum {
    TypeNumber,
    TypeString,
    TypeBoolean,
    TypeCharacter
} BuiltInValueTypes;

// Keywords
typedef enum {
    KeywordFunction,
    KeywordReturn,

    KeywordIf,
    KeywordElse,

    KeywordFor,
    KeywordForEach,
    KeywordRepeat,
    KeywordWhile,

    KeywordBreak,
    KeywordContinue,

    KeywordVar,
    KeywordConst,

    KeywordStruct,
    KeywordEnum,

    KeywordTrue,
    KeywordFalse,

    KeywordPublic,
    KeywordPrivate
} KeywordType;

// Sections
typedef enum {
    SectionImport,
    SectionAliases,
    SectionCreate,
    SectionMain
} SectionType;

// Collections
typedef enum {
    CollectionArray,
    CollectionList, 
    CollectionDictionary,
    CollectionStack,
    CollectionQueue
} CollectionType;

//Function declarations
int CompareTokenToList(const TypeTable *Table, char *Token);

// External lists
extern TypeDefinition BuiltInTypes[];
extern TypeDefinition BuiltInKeyWords[];
extern TypeDefinition CodeSections[];
extern TypeDefinition CollectionTypes[];

#endif