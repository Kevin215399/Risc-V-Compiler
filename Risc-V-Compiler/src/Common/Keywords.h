#ifndef KEYWORDS_H
#define KEYWORDS_H

typedef struct
{
    char *operator;
    uint8_t precedence;
    uint8_t parameters;
    bool unary;
} OperatorPrecedence;


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
    "==",
    "<=",
    ">=",
    "<",
    ">",
    "=",
    "!=",

    "||",
    "|",
    "&&",
    "&",

    "+=",
    "-=",
    "*=",
    "/=",
    "%%=",
    "^=",

    "+",
    "-",
    "*",
    "/",
    "%%",

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
OperatorPrecedence OPERATOR_PRECEDENT_LIST[] = {

    {"%%", 7, 2, false},
    {"*", 7, 2, false},
    {"/", 7, 2, false},

    {"-", 6, 2, false},
    {"+", 6, 2, false},

    {">=", 5, 2, false},
    {"<=", 5, 2, false},
    {">", 5, 2, false},
    {"<", 5, 2, false},

    {"==", 4, 2, false},
    {"!=", 4, 2, false},

    {"&&", 3, 2, false},
    {"&", 3, 2, false},

    {"^", 2, 2, false},

    {"||", 1, 2, false},
    {"|", 1, 2, false},

    {"u~", 8, 1, true},
    {"u!", 8, 1, true},
    {"u-", 8, 1, true}, // - unary
    {"u&", 8, 1, true}, // & unary
    {"u*", 8, 1, true}, // * unary
};
#define OPERATOR_PRECEDENT_COUNT sizeof(OPERATOR_PRECEDENT_LIST) / sizeof(OperatorPrecedence)


#endif