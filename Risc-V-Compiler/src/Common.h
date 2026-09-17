#pragma once

#include <stdio.h>
#include "math.h"
#include "pico/stdlib.h"
#include <string.h>

#include "Common/GeneralList.h"
#include "Common/BinaryTree.h"
#include "Common/Keywords.h"

#include "ErrorFormatter.h"

extern char *sourceCode;

#pragma region Objects

typedef enum TokenType
{
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OPERATOR,
    LITERAL,
    TYPE,
    NONE
} TokenType;
typedef struct Token
{
    uint16_t lineNumber;
    TokenType type;
    char *value;
} Token;

typedef enum FuncOrVar
{
    NEITHER,
    FUNCTION,
    VARIABLE,
} FuncOrVar;

// Also update...
// Keywords.h
// Must match keywords' order, for ParseType()
// SemanticAnalyzer.h PromoteType()
typedef enum IdentifierType
{
    UINT8_T,
    UINT16_T,
    UINT32_T,
    INT8_T,
    INT16_T,
    INT,
    FLOAT,

    BOOL,
    CHAR,
    VOID
} IdentifierType;

typedef struct Identifier
{
    IdentifierType type;
    char *name;
    uint8_t pointerDepth;
    FuncOrVar funcOrVar;
    GeneralList parameters;
} Identifier;

#pragma endregion

#pragma region HelperFunctions

bool IsNumber(char input)
{
    if (input >= 48 && input <= 57)
        return true;
    return false;
}
bool IsLetter(char input)
{
    if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122))
    {
        return true;
    }
    return false;
}

void PushByte(GeneralList *list, uint8_t value)
{
    uint8_t *allocated = (uint8_t *)malloc(sizeof(uint8_t));
    *allocated = value;
    PushList(list, allocated);
}

void PushCharArr(GeneralList *list, char *value)
{
    char *allocated = (char *)malloc(strlen(value) + 1);
    strcpy(allocated, value);
    PushList(list, allocated);
}
IdentifierType ParseType(char *input)
{
    IdentifierType output = INT;
    bool error = true;
    for (int i = 0; i < sizeof(typewords) / sizeof(typewords[0]); i++)
    {
        if (strcmp(typewords[i], input) == 0)
        {
            output = (IdentifierType)i;
            error = false;
            break;
        }
    }
    if (error)
    {
        printf("!!!!!!!!! PARSETYPE() COULD NOT FIND MATCH FOR %s !!!!!!!\n", input);
    }
    return output;
}
int min(int a, int b)
{
    return a < b ? a : b;
}
int max(int a, int b)
{
    return a > b ? a : b;
}

int IntLength(int value)
{
    int numberLength = 1;
    if (abs(value) != 0)
    {
        numberLength = (int)(log10(abs(value))) + 1;
    }
    if (abs(value) != value)
    {
        numberLength += 1;
    }
    return numberLength;
}

#pragma endregion
