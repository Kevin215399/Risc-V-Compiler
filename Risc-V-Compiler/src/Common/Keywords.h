#ifndef KEYWORDS_H
#define KEYWORDS_H

char *keywords[] = {
    "for",
    "while",
    "if",
    "else",
    "break",
    "return",
    "pinSet",
    "pinFlip",
    "pinMode",
    "print",
    "malloc",
    "memset",
    "memread",
    "free"
};

// Also update...
// enum IdentifierType (Common.h)
// Must match IdentifierType's order, for ParseType()
char *typewords[] = {
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "int8_t",
    "int16_t",
    "int",
    "float",
    
    "bool",
    "char",
    "void"};
char *operators[] = {
    "=",
    "<=",
    ">=",
    "<",
    ">",
    "==",
    "!=",

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
    "^=",

    "!",
    "~",
    "&",
    "*"
};
char seperators[] = {
    '{',
    '}',
    '(',
    ')',
    ';',
    '[',
    ']',
    ','
};
char *literals[] = {
    "true",
    "false",
    "NULL",
};

#endif