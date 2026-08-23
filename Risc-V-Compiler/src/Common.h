#ifndef COMMON
#define COMMON
#include "tools/GeneralList.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>

extern char *sourceCode;

#pragma region Objects
typedef enum TokenType
{
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OPERATOR,
    LITERAL,
    NONE
} TokenType;

typedef enum ErrorCode
{
    NO_ERROR,

    SYNTAX_ERROR,
    UNEXPECTED_NULL,
    MALLOC_FAILURE
} ErrorCode;

typedef enum Location
{
    OTHER,
    LEXER,
    USER_SCRIPT
} Location;

typedef struct Token
{
    uint16_t lineNumber;
    TokenType type;
    char *value;
} Token;

typedef struct Error
{
    uint16_t line;
    ErrorCode errorCode;
    Location location;
    char *message;
} Error;
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
void SetError(Error *reference, ErrorCode error, Location location, char *message, uint16_t line)
{
    reference->errorCode = error;
    reference->location = location;
    reference->message = (char *)malloc(strlen(message) + 1);
    strcpy(reference->message, message);
    reference->line = line;
}
#pragma endregion

#endif