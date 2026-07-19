#include "BlokkLanguage.h"

int CompareTokenToList(const TypeTable *Table, char *Token) {
    const int Count = Table->Count;
    for(int i = 0; i < Count; i++) {
        TypeDefinition *Item = &Table->Items[i];
        if(StringsEqual(Item[i].Name, Token)) {
            return Item[i].Type;
        }
    }
    return -1;
}

TypeDefinition BuiltInCollectionTypesData[] = {
    {"Array", CollectionArray},
    {"List", CollectionList},
    {"Dictionary", CollectionDictionary},
    {"Queue", CollectionQueue},
    {"Stack", CollectionStack}
};
TypeTable CollectionTypes = {
    .Items = BuiltInCollectionTypesData,
    .Count = CountOf(BuiltInCollectionTypesData)
};


TypeDefinition CodeSectionsData[] = {
    {"Import", SectionImport},
    {"Aliases", SectionAliases},
    {"Create", SectionCreate},
    {"Main", SectionMain}
};      
TypeTable CodeSections = {
    .Items = CodeSectionsData,
    .Count = CountOf(CodeSectionsData)
};

TypeDefinition BuiltInKeyWords[] = {
    {"True", KeywordTrue},
    {"False", KeywordFalse},

    {"Struct", KeywordStruct},
    {"Enum", KeywordEnum},

    {"Var", KeywordVar},
    {"Const", KeywordConst},

    {"Function", KeywordFunction},
    {"Public", KeywordPublic},
    {"Private", KeywordPrivate},

    {"Continue", KeywordContinue},
    {"Break", KeywordBreak},

    {"For", KeywordFor},
    {"While", KeywordWhile},
    {"Repeat", KeywordRepeat},
    {"ForEach", KeywordForEach},

    {"If", KeywordIf},
    {"Else", KeywordElse}
};
TypeTable CodeSections = {
    .Items = CodeSectionsData,
    .Count = CountOf(CodeSectionsData)
};

TypeDefinition BuiltInTypesData[] = {
    {"Number", TypeNumber},
    {"String", TypeString},
    {"Boolean", TypeBoolean},
    {"Character", TypeCharacter}
};
TypeTable BuiltInTypes = {
    .Items = BuiltInTypesData,
    .Count = CountOf(BuiltInTypesData)
};