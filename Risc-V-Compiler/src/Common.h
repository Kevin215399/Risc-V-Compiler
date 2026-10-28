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

#define NO_DEBUG 0
#define PRINT_ONLY 1
#define PRINT_AND_DELAY 2
#define DEBUG_MODE PRINT_ONLY

#if DEBUG_MODE != NO_DEBUG

#define debugPrintf(...) printf("DEBUG: " __VA_ARGS__)
#define debugPrint(x) printf("%s\n", x)

#if DEBUG_MODE == PRINT_AND_DELAY
#define delay(x) delay(x)
#else
#define delay(x) \
    do           \
    {            \
    } while (0)
#endif

#else

#define debugPrintf(...) \
    do                   \
    {                    \
    } while (0)
#define debugPrint(x) \
    do                \
    {                 \
    } while (0)
#define delay(x) \
    do           \
    {            \
    } while (0)
    
#endif

typedef enum StackType
{
    STACK_VAR,
    RETURN_VALUE,
    PARAMETER,
    REG_STATE,
    STACK_POINTER,
    STR_LITERAL
} StackType;

typedef struct StackPointer
{
    char *name;
    uint32_t offset;
    StackType type;
} StackPointer;

typedef enum TokenType
{
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OPERATOR,
    LITERAL,
    TYPE,
    NONE,
    CFG_RULE,
    STACK_POINTER_TOKEN
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
    uint32_t labelNumber;
} Identifier;

typedef enum Registers
{
    X0,
    SP,
    GP,
    A0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    S0,
    S1,
    S2
} Registers;
char *registerNames[] = {
    "x0",
    "sp",
    "gp",
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "a6",
    "a7",
    "t0",
    "t1",
    "t2",
    "t3",
    "t4",
    "t5",
    "t6",
    "s0",
    "s1",
    "s2"};

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
IdentifierType TypeOfString(char *input)
{
    long value = 0;
    bool isInteger = sscanf(input, "%ld", &value);
    if (isInteger)
    {
        if (value < 0)
        {
            return INT;
        }
        if (value > 2147483647)
        {
            return UINT32_T;
        }
        return INT;
    }
    if (strcmp(input, "false") == 0)
    {
        return BOOL;
    }
    if (strcmp(input, "true") == 0)
    {
        return BOOL;
    }
    return CHAR;
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
int UintLength(uint32_t value)
{
    int numberLength = 1;
    if (value != 0)
    {
        numberLength = (int)(log10(value)) + 1;
    }
    return numberLength;
}

#pragma endregion
