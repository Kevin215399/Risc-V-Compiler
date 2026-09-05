#ifndef KEYWORDS_H
#define KEYWORDS_H

char *keywords[] = {
    "if",
    "while",
    "return",
};

// Also update...
// enum IdentifierType (Common.h)
// Must match IdentifierType's order, for ParseType()
char *typewords[] = {
    "int",
    "float",
    "char",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "bool",
    "char"};
char *operators[] = {
    "=",
    "<=",
    ">=",
    "<",
    ">",

    "+",
    "-",
    "*",
    "/",
    "%%",

    "+=",
    "-=",
    "*=",
    "/=",
    "%%=",
};
char seperators[] = {
    '{',
    '}',
    '(',
    ')',
    ';',
};
char *literals[] = {
    "true",
    "false",
    "NULL",
};

#endif