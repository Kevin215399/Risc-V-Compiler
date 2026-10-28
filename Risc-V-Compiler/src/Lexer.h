#ifndef LEXER_H
#define LEXER_H

#include "Common.h"
#include "Common/Keywords.h"

Error *LexicalAnylsis(char *code, GeneralList *output)
{
    Error *error = (Error *)malloc(sizeof(Error));
    memset(error, 0, sizeof(Error));
    error->errorCode = NO_ERROR;

    if (output == NULL)
    {
        SetError(error, UNEXPECTED_NULL, LEXER, "Missing output destination", 0);
        goto exit;
    }

    uint32_t lexerIndex = 0;
    uint16_t workingLineCount = 0;
    while (lexerIndex < strlen(code))
    {
        printf("Lex index: %d\n", lexerIndex);
        if (code[lexerIndex] == '\n')
        {
            lexerIndex++;
            workingLineCount++;
            continue;
        }
        if (code[lexerIndex] == ' ')
        {
            lexerIndex++;
            continue;
        }

        TokenType matchType = NONE;
        uint8_t tokenLength = 0;

        // Check for keywords
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            char *keyword = keywords[i];
            if (strlen(code) - lexerIndex < strlen(keyword))
                continue;
            if (strncmp((&code[lexerIndex]), keyword, strlen(keyword)) == 0)
            {
                tokenLength = strlen(keyword);
                matchType = KEYWORD;
                printf("KEYWORD len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for typewords
        for (int i = 0; i < sizeof(typewords) / sizeof(typewords[0]); i++)
        {
            char *type = typewords[i];
            if (strlen(code) - lexerIndex < strlen(type))
                continue;
            if (strncmp((&code[lexerIndex]), type, strlen(type)) == 0)
            {
                tokenLength = strlen(type);
                matchType = TYPE;
                printf("TYPE len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for operators
        for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
        {
            char *operator= operators[i];
            if (strlen(code) - lexerIndex < strlen(operator))
                continue;
            if (strncmp(&code[lexerIndex], operator, strlen(operator)) == 0)
            {
                tokenLength = strlen(operator);
                matchType = OPERATOR;
                printf("OPERATOR len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for Seperators
        for (int i = 0; i < sizeof(seperators) / sizeof(seperators[0]); i++)
        {
            char seperator = seperators[i];
            if (strlen(code) - lexerIndex < 1)
                continue;
            if (code[lexerIndex] == seperator)
            {
                tokenLength = 1;
                matchType = SEPERATOR;
                printf("SEPERATOR len: %d\n", tokenLength);
                goto allocateToken;
            }
        }

        // Check for literals
        for (int i = 0; i < sizeof(literals) / sizeof(literals[0]); i++)
        {
            char *literal = literals[i];
            if (strlen(code) - lexerIndex < strlen(literal))
                continue;
            if (strncmp(&code[lexerIndex], literal, strlen(literal)) == 0)
            {
                tokenLength = strlen(literal);
                matchType = LITERAL;
                printf("LITERAL len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for string literals
        if ((code[lexerIndex]) == '\"')
        {
            while (lexerIndex + tokenLength < strlen(code))
            {
                tokenLength++;
                if (code[lexerIndex + tokenLength] == '\"')
                {
                    break;
                }
            }
            if (lexerIndex + tokenLength >= strlen(code))
            {
                SetError(error, SYNTAX_ERROR, LEXER, "Missing closing \'\"\'", 0);
                goto exit;
            }
            tokenLength++;
            printf("LITERAL len: %d\n", tokenLength);

            matchType = LITERAL;
            goto allocateToken;
        }
        // Check for number literals
        if (IsNumber(code[lexerIndex]) || code[lexerIndex] == '.')
        {
            bool hasDecimal = false;
            while (lexerIndex + tokenLength < strlen(code))
            {
                tokenLength++;
                char nextChar = (code[lexerIndex + tokenLength]);
                if (!IsNumber(nextChar))
                {
                    if (nextChar == '.' && !hasDecimal)
                    {
                        hasDecimal = true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            printf("LITERAL len: %d\n", tokenLength);

            matchType = LITERAL;
            goto allocateToken;
        }

        // Check for identifiers
        if (IsLetter(code[lexerIndex]) || (code[lexerIndex] == '_'))
        {
            bool hasDecimal = false;
            bool hasNegative = false;
            while (lexerIndex + tokenLength < strlen(code))
            {
                tokenLength++;
                char nextChar = (code[lexerIndex + tokenLength]);
                if (!IsLetter(nextChar) && nextChar != '_' && !IsNumber(nextChar))
                {
                    break;
                }
            }
            printf("Identifier len: %d\n", tokenLength);

            matchType = IDENTIFIER;
            goto allocateToken;
        }

    allocateToken:
        if (matchType != NONE)
        {
            printf("CreateToken\n");

            Token *token = (Token *)malloc(sizeof(Token));
            token->type = matchType;
            token->value = (char *)malloc(tokenLength + 1);
            strncpy(token->value, &code[lexerIndex], tokenLength);

            token->value[tokenLength] = '\0';
            token->lineNumber = workingLineCount;
            lexerIndex += tokenLength;
            PushList(output, token);

            printf("done token\n");
        }
    }

    // Replace compiler directives;
    for (int i = 0; i < output->count; i++)
    {
        Token *token = (Token *)ListGetIndex(output, i);
        if (token->type != IDENTIFIER)
            continue;
        printf("%s is identifier\n", token->value);
        for (int x = 0; x < sizeof(compilerDirectives) / sizeof(CompilerDirective); x++)
        {
            if (strcmp(token->value, compilerDirectives[x].source) == 0)
            {
                printf("%s is match\n", compilerDirectives[x].source);
                free(token->value);
                token->value = (char *)malloc(strlen(compilerDirectives[x].replacement) + 1);
                strcpy(token->value, compilerDirectives[x].replacement);
                token->type = LITERAL;
                printf("new Value: %s\n", token->value);
                break;
            }
        }
    }

exit:
    return error;
}

#endif