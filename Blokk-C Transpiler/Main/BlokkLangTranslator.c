#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "Blokk-C-Library/Data/Lists.h"
#include "BlokkLanguage.h"
#include "Types.h"


// Code
int main() {

    // TODO: Get the right path of the project, add support for mac, windows and linux
    const char* Path = "XYZ";

    LanguageParser Parser = {
        .CurrentCharData = {0, 0},
        .CurrentFileIdx = 0,
        .FileLength = 0,
        .File = NULL,

        .NextWord = NULL,
        .TempIdx = 0,
        .CurrentReadingIndex = 0,
        .TempIdx = 0
    };

    // Load the file
    Parser.File = LoadTextFile(Path, &Parser.FileLength);

    // Helper vars
    int *CurrentIdx = &Parser.CurrentFileIdx;
    bool Result;

    // Reading the file
    while(CurrentIdx < Parser.FileLength) {

        // Skip over whitespaces
        SkipUntilNextChar(Parser);

        // Parse sections
        if(Parser.File[*CurrentIdx] == '@') {
            Result = Parser.ParseSection(&Parser);
        }

        
    }
}

// Parsing sections
bool ParseSection(LanguageParser *Parser) { 

    // Skip until the next valid character
    SkipUntilNextChar(Parser);

    // Expect the closing brace
    bool Result = ExpectCharacter(Parser, '{');
    if(!Result) return false;

    // Get the end idx where the symbol is2
    int EndIdx = FindTextUntilSymbol(Parser);
    if(EndIdx - Parser->CurrentFileIdx < 5 || EndIdx != -1) {

        // Get the word after @ to get the section type
        Parser->NextWord = SliceTextFromFile(Parser->File, Parser->CurrentFileIdx, EndIdx);
        Cleanup(&Parser->NextWord); // Remove unneccessary characters (spaces, tabs, etc)

        // Get the section type
        int TokenType = CompareTokenToList(&CodeSections, Parser->NextWord);
        switch(TokenType) {
            case SectionImport:
                // TODO: Make importing section
                 break;
            case SectionCreate:
                Result = ParseSectionCreate(Parser);
                if(!Result) return false;
                break;
            case SectionAliases:
                // TODO: Make Aliases function
                break;
            default:
                // TODO: Get an unidentified word error
                SkipUntilSpecificChar(Parser, '}');
                return false;
        }
    }
}

// Parse the create section
bool ParseSectionCreate(LanguageParser *Parser) {
    int i = Parser->CurrentFileIdx; // Make a local looping var
    bool TempResult; // Temporary bool val

    // Make sure i starts at the start of the section
    SkipUntilNextChar(Parser);

    TempResult = ExpectSymbol(Parser, '{');
    if(!TempResult) return false;

    // Get the end of the section
    int EndIdx = FindTextInGroupingSymbol(Parser, '{', '}');
    if(EndIdx != -1) {

        // The loop
        bool Complete = false;
        while(!Complete) {

            // Skip over whiteSpaces
            SkipUntilNextChar(Parser);

            // Check if the loop should be completed
            if(IsCurrentChar(Parser, '}')) {
                Complete = true;
            }

            // Parse the variable declaration
            TempResult = ParseCreateDeclaration(Parser);

            // Verify if it worked, and if not stop the loop and skip
            if(!TempResult) {
                Complete = true;
                TempResult = SkipUntilSpecificChar(Parser, '}');
                if(TempResult) {
                    return false;
                }
            }
        }
    }

    return true;
}

// Parse a create section declaration
bool ParseCreateDeclaration(LanguageParser *Parser) {

    // Create a var to reuse for conditions
    bool Result;

    // Skip until the next character
    SkipUntilNextChar(Parser);

    // Expect a colon
    Result = ExpectCharacter(Parser, ':');
    if(!Result) return false; // If the character isn't there, stop

    // Create a temporary var
    int StartIdx = Parser->CurrentFileIdx;
    
    // Get where the colon is
    int EndIdx = GetNextSymbol(Parser);

    // Get the name of the declared var, using the colon and the currentIdx
    char *Name = SliceTextFromFile(Parser->File, StartIdx, EndIdx);
    CleanUp(Name); // Clean it up
    StartIdx = EndIdx + 1; // Move the var to the end

    // Expect the semicolon 
    Result = ExpectCharacter(Parser, ';');
    if(!Result) return false; // Return if it doesn't appear

    EndIdx = GetNextSymbol(Parser);
    // Get the type of the declared var, using the semicolon and the currentIdx
    char *Type = SliceTextFromFile(Parser->File, StartIdx, EndIdx);

    CreateVariableDeclaration(Name, Type);
    return true;
}

// Get the token key
int GetTokenKey(char *Token, int TokenLength) {
    int Result = CompareTokenToList(&BuiltInTypes, Token);
    if(Result == -1) {
        
    } else {
        return Result;
    }
}

