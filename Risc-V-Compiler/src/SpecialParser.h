#ifndef SPECIAL_PARSER_H
#define SPECIAL_PARSER_H

#include "Common.h"
#include "SemanticAnalyzer.h"
typedef struct
{
    char *operator;
    uint8_t precedence;
} OperatorPrecedence;

typedef struct
{
    Token *token;
    uint8_t precedence;
} PrecedenceWrapper;

typedef enum
{
    NO_CAST,
    CAST_TYPE,
    CAST_POINTER,
} Cast;

// when an unkown token is found, set to this precedence
#define UNKOWN_PRECEDENCE 13

OperatorPrecedence OPERATOR_PRECEDENT_LIST[] = {
    {"&", 4},

    {"%", 3},
    {"^", 3},
    {"*", 3},
    {"/", 3},

    {"-", 2},
    {"+", 2},

    {">=", 1},
    {"<=", 1},
    {">", 1},
    {"<", 1},

    {"==", 1},
    {"!=", 1},

};
#define OPERATOR_PRECEDENT_COUNT sizeof(OPERATOR_PRECEDENT_LIST) / sizeof(OperatorPrecedence)

typedef struct FunctionCall
{
    uint8_t paramatersLeft;
    Identifier *templateFunction;
}FunctionCall;

// Function will destroy input list
Error *ShuntYard(GeneralList *tokens, GeneralList *variableStack, GeneralList *outputStack)
{
    printf("ShuntYard\n");
    Error *error = (Error *)malloc(sizeof(Error));
    memset(error, 0, sizeof(Error));
    error->errorCode = NO_ERROR;

    if (tokens == NULL)
    {
        SetError(error, UNEXPECTED_NULL, SPECIAL_PARSER, "Missing Tokens reference", 0);
        goto exit;
    }

    GeneralList operatorStack;
    InitializeList(&operatorStack);

    // Special handling for type casts
    Cast castPhase = NO_CAST;

    GeneralList functionCalls;
    InitializeList(&functionCalls);

    while (tokens->count > 0)
    {
        printf("loop start, %d left\n", tokens->count);
        Token *currentToken = (Token *)PopListFirst(tokens);
        switch (currentToken->type)
        {
        case SEPERATOR:
        {
            if (castPhase != NO_CAST)
            {
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Cannot do expressions within a cast, use (type pointer)variable", 0);
                goto exit;
            }
            Token *peekNext = (Token *)ListGetIndex(tokens, 0);
            if (currentToken->value[0] == ')')
            {
                // If the operator is parenthesis, pop off all of the previous operations until you get to the matching one
                printf("parenthesis\n");
                while (
                    operatorStack.count > 0 &&
                    ((PrecedenceWrapper *)PeekList(&operatorStack))->token->value[0] != '(')
                {
                    PrecedenceWrapper *poppedOpp = (PrecedenceWrapper *)PopList(&operatorStack);
                    PushList(outputStack, poppedOpp->token);
                    free(poppedOpp);

                    printf("moved within parenthesis to operand stack\n");
                }

                PrecedenceWrapper *poppedOpp = (PrecedenceWrapper *)PopList(&operatorStack);
                free(poppedOpp);
                free(currentToken);
                castPhase = NO_CAST;
            }
            else if (currentToken->value[0] == '(')
            {
                PrecedenceWrapper *wrapped = (PrecedenceWrapper *)malloc(sizeof(PrecedenceWrapper));
                wrapped->token = currentToken;
                wrapped->precedence = 0;
                PushList(&operatorStack, wrapped);
                if (peekNext == NULL)
                {
                    SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Missing closing parenthesis", 0);
                    goto exit;
                }
                else
                {
                    if (peekNext->type == TYPE)
                    {
                        castPhase = CAST_TYPE;
                    }
                }
            }
        }
        break;
        case OPERATOR:
        {
            if (castPhase == CAST_POINTER && currentToken->value[0] == '*')
            {
                PushList(outputStack, currentToken);
                break;
            }
            if (castPhase != NO_CAST)
            {
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Cannot do math within a cast, use (type pointer)variable", 0);
                goto exit;
            }
            printf("operator token\n");
            PrecedenceWrapper *wrapped = (PrecedenceWrapper *)malloc(sizeof(PrecedenceWrapper));
            wrapped->token = currentToken;

            bool foundOperator = false;
            for (int i = 0; i < OPERATOR_PRECEDENT_COUNT; i++)
            {
                if (strcmp(OPERATOR_PRECEDENT_LIST[i].operator, currentToken->value) == 0)
                {
                    wrapped->precedence = OPERATOR_PRECEDENT_LIST[i].precedence;
                    foundOperator = true;
                    break;
                }
            }
            if (!foundOperator)
            {
                printf("not recognized");
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Operator not recognized", 0);
                goto exit;
            }
            while (operatorStack.count > 0 && ((PrecedenceWrapper *)PeekList(&operatorStack))->precedence >= wrapped->precedence)
            {
                printf("pop opp\n");
                PrecedenceWrapper *poppedOpp = (PrecedenceWrapper *)PopList(&operatorStack);
                PushList(outputStack, poppedOpp->token);
                free(poppedOpp);
            }
            PushList(&operatorStack, wrapped);
        }
        break;
        case LITERAL:
        {
            if (castPhase != NO_CAST)
            {
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Literals not allowed within a cast", 0);
                goto exit;
            }
            printf("literal token\n");
            PushList(outputStack, currentToken);
        }
        break;
        case IDENTIFIER:
        {
            if (castPhase != NO_CAST)
            {
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Symbols are not allowed within a cast", 0);
                goto exit;
            }
            Identifier *symbol = FindSymbol(variableStack, currentToken->value);
            if (symbol == NULL)
            {
                SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Symbol not defined in current scope", 0);
                goto exit;
            }
            PushList(outputStack, currentToken);
        }
        break;
        case TYPE:
            if (castPhase == CAST_TYPE)
            {
                PushList(outputStack, currentToken);
            }
            SetError(error, SYNTAX_ERROR, SPECIAL_PARSER, "Types can only be used within casts", 0);
            goto exit;

            break;

        default:
        {
            printf("type none?\n");
        }
        break;
        }
    }
    while (operatorStack.count > 0)
    {
        printf("pop opp\n");
        PrecedenceWrapper *poppedOpp = (PrecedenceWrapper *)PopList(&operatorStack);
        PushList(outputStack, poppedOpp->token);
        free(poppedOpp);
    }

exit:
    return error;
}

#endif