#ifndef ERROR_FORMATTER_H
#define ERROR_FORMATTER_H
#include "Common.h"

// found in common.h
int IntLength(int value);

// Error codes
typedef enum ErrorCode
{
    NO_ERROR,

    SYNTAX_ERROR,
    UNEXPECTED_NULL,
    MALLOC_FAILURE,
    UNDEFINED,
    AST_STRUCTURE,
    TYPE_MISMATCH,
    POINTERS,
    DUPLICATE
} ErrorCode;

typedef enum Location
{
    OTHER,
    LEXER,
    SYNTAX_ANALYZER,
    SEMANTIC_ANALYZER,
} Location;

typedef struct Error
{
    uint16_t line;
    ErrorCode errorCode;
    Location location;
    char *message;
} Error;

const char *ERR_CODE_NAMES[] = {"Success", "Syntax Error", "Unexpected Null", "Malloc failure :( bad bad bad", "Undefined value", "AST Structure", "Type mismatch", "Pointer ettiquite", "Duplicate symbol"};
const char *ERR_LOC_NAMES[] = {"OTHER", "LEXER", "SYNTAX ANALYZER", "SEMANTIC ANALYZER"};

void SetError(Error *reference, ErrorCode error, Location location, char *message, uint16_t line)
{
    reference->errorCode = error;
    reference->location = location;
    reference->message = (char *)malloc(strlen(message) + 1);
    strcpy(reference->message, message);
    reference->line = line;
}
char *FormatError(Error *error)
{
    char *output = NULL;
    uint8_t errorLength = 0;
    if (error->errorCode == NO_ERROR)
    {
        errorLength = strlen(ERR_CODE_NAMES[0]);
        output = (char *)malloc(errorLength);
        strcpy(output, ERR_CODE_NAMES[0]);
        return output;
    }

    errorLength = strlen(ERR_LOC_NAMES[(int)error->location]);
    errorLength += strlen(": ");
    errorLength += strlen(ERR_CODE_NAMES[(int)error->errorCode]);
    errorLength += strlen(": ");
    errorLength += strlen(error->message);
    errorLength += strlen(", at line ");
    errorLength += IntLength(error->line);
    errorLength += 1;

    output = (char *)malloc(errorLength);
    sprintf(output, "%s: %s: %s, at line %d",
            ERR_LOC_NAMES[(int)error->location],
            ERR_CODE_NAMES[(int)error->errorCode],
            error->message,
            (int)error->line);
    return output;
}
void PrintError(Error* error){
    char* message = FormatError(error);
    printf("%s\n",message);
    free(message);
}
#endif