#ifndef LEXER_H
#define LEXER_H

#pragma region KeywordLists
char *keywords[] = {
    "if",
    "while",
    "return",
    "int"};
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
#pragma endregion

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
    while (lexerIndex < strlen(sourceCode))
    {
        printf("Lex index: %d\n", lexerIndex);
        if (sourceCode[lexerIndex] == '\n')
        {
            lexerIndex++;
            workingLineCount++;
            continue;
        }
        if (sourceCode[lexerIndex] == ' ')
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
            if (strlen(sourceCode) - lexerIndex < strlen(keyword))
                continue;
            if (strncmp((&sourceCode[lexerIndex]), keyword, strlen(keyword)) == 0)
            {
                tokenLength = strlen(keyword);
                matchType = KEYWORD;
                printf("KEYWORD len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for operators
        for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
        {
            char *operator= operators[i];
            if (strlen(sourceCode) - lexerIndex < strlen(operator))
                continue;
            if (strncmp(&sourceCode[lexerIndex], operator, strlen(operator)) == 0)
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
            if (strlen(sourceCode) - lexerIndex < 1)
                continue;
            if (sourceCode[lexerIndex] == seperator)
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
            if (strlen(sourceCode) - lexerIndex < strlen(literal))
                continue;
            if (strncmp(&sourceCode[lexerIndex], literal, strlen(literal)) == 0)
            {
                tokenLength = strlen(literal);
                matchType = LITERAL;
                printf("LITERAL len: %d\n", tokenLength);
                goto allocateToken;
            }
        }
        // Check for string literals
        if ((sourceCode[lexerIndex]) == '\"')
        {
            while (lexerIndex + tokenLength < strlen(sourceCode))
            {
                tokenLength++;
                if (sourceCode[lexerIndex] == '\"')
                {
                    break;
                }
            }
            if (lexerIndex + tokenLength >= strlen(sourceCode))
            {
                SetError(error, SYNTAX_ERROR, LEXER, "Missing closing \'\"\'", 0);
                goto exit;
            }
            printf("LITERAL len: %d\n", tokenLength);

            matchType = LITERAL;
            goto allocateToken;
        }
        // Check for number literals
        if (IsNumber(sourceCode[lexerIndex]))
        {
            bool hasDecimal = false;
            bool hasNegative = false;
            while (lexerIndex + tokenLength < strlen(sourceCode))
            {
                tokenLength++;
                char nextChar = (sourceCode[lexerIndex + tokenLength]);
                if (!IsNumber(nextChar))
                {
                    if (nextChar == '-' && !hasNegative)
                    {
                        hasNegative = true;
                    }
                    else
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
            }
            printf("LITERAL len: %d\n", tokenLength);

            matchType = LITERAL;
            goto allocateToken;
        }

        // Check for identifiers
        if (IsLetter(sourceCode[lexerIndex]) || (sourceCode[lexerIndex] == '_'))
        {
            bool hasDecimal = false;
            bool hasNegative = false;
            while (lexerIndex + tokenLength < strlen(sourceCode))
            {
                tokenLength++;
                char nextChar = (sourceCode[lexerIndex + tokenLength]);
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
            strncpy(token->value, &sourceCode[lexerIndex], tokenLength);

            token->value[tokenLength] = '\0';
            token->lineNumber = workingLineCount;
            lexerIndex += tokenLength;
            PushList(output, token);

            printf("done token\n");
        }
    }

exit:
    return error;
}

#endif