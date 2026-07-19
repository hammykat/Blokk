#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "BlokkLangTranslator.c"
#include "ErrorSystem.h"

// Load the file, return it's chars and get it's length
char *LoadTextFile(const char *Path, int *Length) {
    FILE *File = fopen(Path, "rb");
    if (File == NULL) {
        return NULL;
    }

    // Get file size
    fseek(File, 0, SEEK_END);
    long FoundLength = ftell(File);
    if(FoundLength == -1L) {
        fclose(File);
        return NULL;
    }

    *Length = ftell(File);
    rewind(File);

    // Allocate memory (+1 for '\0')
    char *Text = malloc(*Length + 1);
    if (Text == NULL) {
        fclose(File);
        return NULL;
    }

    // Read file
    fread(Text, 1, *Length, File);
    Text[*Length] = '\0';

    fclose(File);

    return Text;
}



// Comparing multiple characters
bool StringsEqual(char *String1, char *String2) {
    if(String1 == NULL || String2 == NULL) return false;
    const size_t Length = strlen(String1);
    if(Length != strlen(String2)) return false;
    for(size_t i = 0; i < Length; i++) {
        if(String1[i] != String2[i]) return false;
    }
    return true;
}



// Get text from one index to another in a file
char* SliceTextFromFile(char *File, int StartIdx, int EndIdx) {
    const int Size = (EndIdx - StartIdx + 1) * sizeof(char);
    char *Result = malloc(Size + 1);
    if(Result == NULL) return NULL;
    memcpy(Result, &File[StartIdx], Size);
    Result[Size] = '\0';
    return Result;
}



// Find the text until a symbol
int FindTextUntilSymbol(char *File, int Start, char Symbol, int FileLength) {
    int CurrentIdx = Start;
    while(CurrentIdx < FileLength - 1) {
        CurrentIdx++;
        if(File[CurrentIdx] == Symbol) return CurrentIdx;
    }
    return -1;
}



// Check if the character is a white space
bool IsWhiteSpace(char TChar) {
    if(TChar == ' ' || TChar == '\n' || TChar == '\t' || TChar == '\r' || TChar == '\v' || TChar == '\f') return true;
    return false;
}



// Check if the character is a newline, return number of newline characters
int IsNewLine(char *File, int Idx) {
    char TChar = File[Idx];
    if(TChar == '\r') { // Handle \r
        if(File[Idx + 1] == '\n') { // Handle \r\n
            return 2;
        }
        return 1;
    } else if (TChar == '\n') { // Handle \n
        return 1;
    }
    return 0;
}



// Find if the character is a letter
bool IsLetter(char Char) {
    if((Char >='A' && Char <= 'Z') || (Char >='a' && Char <= 'z')) {
        return true;
    }
    return false;
}



// Check if the current char is a specific one
bool IsCurrentChar(LanguageParser *Parser, char Char) {
    if(Parser->File[Parser->CurrentFileIdx] == Char) return true;
    return false;
}



// Find the text inside a grouping symbol, eg. (), {}, [] etc
int FindTextInGroupingSymbol(LanguageParser *Parser, char StartSymbol, char EndSymbol) {

    // Initialize vars
    int Current = Parser->CurrentFileIdx + 1; 
    int GroupingVar = 1; 
    char CurrentChar;

    // The loop
    long Length = Parser->FileLength;
    while(Current < Length) {

        // Get the current char
        CurrentChar = Parser->File[Current]; 
        Current++; // Increment the var

        if(CurrentChar == EndSymbol) { // Check if the current var is the ending symbol
            GroupingVar--; // Decrement it

            // If it's less than one, return it
            if(GroupingVar <= 0) { 
                return Current;
            }
        } else if(CurrentChar == StartSymbol) { // Check if the current var is the starting symbol
            GroupingVar++; // Increment it
        }
    }
    return -1;
}



// Clean up a string from whitespaces
void CleanUpName(char *Name) {

    // Initializing vars
    int Count = strlen(Name);
    int CharCount = 0;

    // The loop
    for(int i = 0; i < Count; i++) {

        // Check if the char isn't a whitespace, if so, count it
        if(!IsWhiteSpace(Name[i])) {
            Name[CharCount] = Name[i];
            CharCount++;
        }
    }
    Name[CharCount] = '\0';
}



// Expect the next character to be something
bool ExpectSymbol(LanguageParser *Parser, char Char) {
    SkipUntilNextChar(Parser);
    int NextSymbol = FindNextSymbol(Parser);
    if(NextSymbol != -1 && Parser->File[NextSymbol] == Char) {
        return true;
    } else {
        SkipUntilPosition(Parser, NextSymbol);
        Error ResultError = {
            .CharExpected = {
                .Expected = Char,
                .Got = Parser->File[NextSymbol],
                .Place = (CharacterData){
                    .Line = Parser->CurrentLineNum,
                    .Number = Parser->CurrentLineCharNum
                }
            }
        };
        ErrorLogList_Add(&Parser->ErrorLog, ResultError);
        return false;
    }
}



// Get the next char in the file
void SkipUntilNextChar(LanguageParser *Parser) {
    int z = Parser->CurrentFileIdx;
    while(Parser->File[z] != '\0' && IsWhiteSpace(Parser->File[z])) {
        int Var = IsNewLine(Parser->File, z);
        if(Var > 0) {
            Parser->CurrentLineNum++;
            Parser->CurrentLineCharNum = 0;
            z += Var;
        } else {
            Parser->CurrentLineCharNum++;
            z++;
        }
    }
    Parser->CurrentFileIdx = z;
}



// Get the next symbol
int FindNextSymbol(LanguageParser *Parser) {
    int i = Parser->CurrentFileIdx;
    char CurrentChar = Parser->File[i];
    while (CurrentChar != '\0') {
        CurrentChar = Parser->File[i];
        if (!IsLetter(CurrentChar) && !IsWhiteSpace(CurrentChar)) {
            return i;
        }
        i++;
    }
    return -1;
}



// Skip until a certain position while updating the vars
void SkipUntilPosition(LanguageParser *Parser, int Idx) {
    if (Idx < Parser->CurrentFileIdx) {
        return;
    }

    int CurrentIdx = Parser->CurrentFileIdx;

    while (CurrentIdx < Idx && Parser->File[CurrentIdx] != '\0') {
        int Var = IsNewLine(Parser->File, CurrentIdx);
        if (Var > 0) {
            Parser->CurrentLineNum++;
            Parser->CurrentLineCharNum = 0;
            CurrentIdx += Var;
        } else {
            Parser->CurrentLineCharNum++;
            CurrentIdx++;
        }
    }

    Parser->CurrentFileIdx = CurrentIdx;
}



// Skip until a specific char
bool SkipUntilSpecificChar(LanguageParser *Parser, char Char) {
    int i = Parser->CurrentFileIdx;

    // The loop
    while(true) {

        i++; // Increment

        // Make sure it doesn't go over file length
        if(i >= Parser->FileLength) {
            return false;
        }

        // Check if the current char is the right one
        if(Parser->File[i] == Char) {

            // SKip until the idx of the char
            SkipUntilPosition(Parser, i);
            Parser->CurrentFileIdx = i; // Update var
            return true; // End function
        }
    }
}