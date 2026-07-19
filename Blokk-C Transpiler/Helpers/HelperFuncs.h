#ifndef HELPERFUNCS_H
#define HELPERFUNCS_H

#include <stdbool.h>
#include "Types.h"

// Function declarations
char *LoadTextFile(const char *Path, int *Length);
bool StringsEqual(char *String1, char *String2);
char *SliceTextFromFile(char *File, int StartIdx, int EndIdx);
int FindTextUntilSymbol(char *File, int Start, char Symbol, int FileLength);
bool IsWhiteSpace(char TChar);
int FindTextInGroupingSymbol(LanguageParser *Parser, char StartSymbol, char EndSymbol);
void CleanUpName(char *Name);
bool IsLetter(char Char);
int IsNewLine(char *File, int Idx);
bool IsCurrentChar(LanguageParser *Parser, char Char);
bool ExpectSymbol(LanguageParser *Parser, char Char);
void SkipUntilNextChar(LanguageParser *Parser);
int FindNextSymbol(LanguageParser *Parser);
void SkipUntilPosition(LanguageParser *Parser, int Idx);
bool SkipUntilSpecificChar(LanguageParser *Parser, char Char);

#endif