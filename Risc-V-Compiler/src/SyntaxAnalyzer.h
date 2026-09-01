#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H
#include "Common.h"

#pragma region CFG

#define TERMINATORS 21
#define NONTERMINATORS 18
const char *NonTerms[] = {"IDENTIFIER", "TYPE", "LITERAL", "\0", "while", "if", "}", ")", ";", "(", "<", ">", "<=", ">=", "!=", "==", "+", "-", "*", "/", "=", "void", "char", "int", "bool", "float"};
#define BODY 0
#define LINE 1
#define DECLARATION 2
#define DECREST 3
#define WHILE 4
#define IF 5
#define ASSIGN 6
#define EXPRESSION 7
#define EQUALITY 8
#define EQUALITYREST 9
#define ADDITION 10
#define ADDITIONREST 11
#define MULTIPLY 12
#define MULTIPLYREST 13
#define ATOM 14
#define TYPE 15
#define IDENTIFIER_TERM 16
#define LITERAL_TERM 17

GeneralList grammarTable[NONTERMINATORS][TERMINATORS];
#define TERM_OFFSET 128

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
uint8_t GetTermIndex(char *term)
{
    uint8_t x = 255;
    for (int i = 0; i < sizeof(NonTerms) / sizeof(NonTerms[0]); i++)
    {
        if (strcmp(NonTerms[i], term) == 0)
        {
            x = i;
            break;
        }
    }
    return x;
}
void AddTerm(char *term, uint8_t y, char *rule)
{
    uint8_t x = GetTermIndex(term);
    if (x == 255)
        return;

    PushCharArr(&grammarTable[y][x], rule);
}
void AddNonTerm(char *term, uint8_t y, uint8_t nonterm)
{
    uint8_t x = GetTermIndex(term);
    if (x == 255)
        return;
    PushByte(&grammarTable[y][x], nonterm);
}
void SetupCFG()
{
    for (int y = 0; y < NONTERMINATORS; y++)
    {
        for (int x = 0; x < TERMINATORS; x++)
        {
            InitializeList(&grammarTable[y][x]);
        }
    }
    // Body
    AddNonTerm("IDENTIFIER", BODY, TERM_OFFSET + LINE);
    AddNonTerm("IDENTIFIER", BODY, TERM_OFFSET + BODY);
    AddNonTerm("TYPE", BODY, TERM_OFFSET + LINE);
    AddNonTerm("TYPE", BODY, TERM_OFFSET + BODY);
    AddNonTerm("while", BODY, TERM_OFFSET + LINE);
    AddNonTerm("while", BODY, TERM_OFFSET + BODY);
    AddNonTerm("if", BODY, TERM_OFFSET + LINE);
    AddNonTerm("if", BODY, TERM_OFFSET + BODY);
    AddTerm("\0", BODY, "");
    AddTerm("}", BODY, "");

    // Line
    AddTerm(")", LINE, "");
    AddTerm(";", LINE, "");
    AddTerm("}", LINE, "");
    AddNonTerm("IDENTIFIER", LINE, TERM_OFFSET + ASSIGN);
    AddNonTerm("TYPE", LINE, TERM_OFFSET + DECLARATION);
    AddNonTerm("while", LINE, TERM_OFFSET + WHILE);
    AddNonTerm("if", LINE, TERM_OFFSET + IF);

    // Declaration
    AddNonTerm("TYPE", DECLARATION, TERM_OFFSET + TYPE);
    AddNonTerm("TYPE", DECLARATION, TERM_OFFSET + IDENTIFIER_TERM);
    AddNonTerm("TYPE", DECLARATION, TERM_OFFSET + DECREST);

    // Dec Rest
    AddTerm(";", DECREST, ";");

    AddTerm("(", DECREST, "(");
    AddTerm("(", DECREST, ")");
    AddTerm("(", DECREST, "{");
    AddNonTerm("(", DECREST, TERM_OFFSET + BODY);
    AddTerm("(", DECREST, "}");

    AddTerm("=", DECREST, "=");
    AddNonTerm("=", DECREST, TERM_OFFSET + EXPRESSION);
    AddTerm("=", DECREST, ";");

    // While
    AddTerm("while", WHILE, "while");
    AddTerm("while", WHILE, "(");
    AddNonTerm("while", WHILE, TERM_OFFSET + EXPRESSION);
    AddTerm("while", WHILE, ")");
    AddTerm("while", WHILE, "{");
    AddNonTerm("while", WHILE, TERM_OFFSET + BODY);
    AddTerm("while", WHILE, "}");

    // If
    AddTerm("if", IF, "if");
    AddTerm("if", IF, "(");
    AddNonTerm("if", IF, TERM_OFFSET + EXPRESSION);
    AddTerm("if", IF, ")");
    AddTerm("if", IF, "{");
    AddNonTerm("if", IF, TERM_OFFSET + BODY);
    AddTerm("if", IF, "}");

    // Assign
    AddNonTerm("IDENTIFIER", ASSIGN, TERM_OFFSET + IDENTIFIER_TERM);
    AddTerm("IDENTIFIER", ASSIGN, "=");
    AddNonTerm("IDENTIFIER", ASSIGN, TERM_OFFSET + EXPRESSION);
    AddTerm("IDENTIFIER", ASSIGN, ";");

    // Expression
    AddNonTerm("IDENTIFIER", EXPRESSION, TERM_OFFSET + EQUALITY);
    AddNonTerm("LITERAL", EXPRESSION, TERM_OFFSET + EQUALITY);

    // Equality
    AddNonTerm("IDENTIFIER", EQUALITY, TERM_OFFSET + ADDITION);
    AddNonTerm("IDENTIFIER", EQUALITY, TERM_OFFSET + EQUALITYREST);
    AddNonTerm("LITERAL", EQUALITY, TERM_OFFSET + ADDITION);
    AddNonTerm("LITERAL", EQUALITY, TERM_OFFSET + EQUALITYREST);

    // Equality rest
    AddTerm(")", EQUALITYREST, "");
    AddTerm(";", EQUALITYREST, "");

    AddTerm("<", EQUALITYREST, "<");
    AddNonTerm("<", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm("<", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    AddTerm(">", EQUALITYREST, ">");
    AddNonTerm(">", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm(">", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    AddTerm("<=", EQUALITYREST, "<=");
    AddNonTerm("<=", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm("<=", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    AddTerm(">=", EQUALITYREST, ">=");
    AddNonTerm(">=", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm(">=", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    AddTerm("!=", EQUALITYREST, "!=");
    AddNonTerm("!=", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm("!=", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    AddTerm("==", EQUALITYREST, "==");
    AddNonTerm("==", EQUALITYREST, TERM_OFFSET + ADDITION);
    AddNonTerm("==", EQUALITYREST, TERM_OFFSET + EQUALITYREST);

    // Addition
    AddNonTerm("IDENTIFIER", ADDITION, TERM_OFFSET + MULTIPLY);
    AddNonTerm("IDENTIFIER", ADDITION, TERM_OFFSET + ADDITIONREST);
    AddNonTerm("LITERAL", ADDITION, TERM_OFFSET + MULTIPLY);
    AddNonTerm("LITERAL", ADDITION, TERM_OFFSET + ADDITIONREST);

    // Addition rest
    AddTerm("<", ADDITIONREST, "");
    AddTerm(">", ADDITIONREST, "");
    AddTerm("<=", ADDITIONREST, "");
    AddTerm(">=", ADDITIONREST, "");
    AddTerm("!=", ADDITIONREST, "");
    AddTerm("==", ADDITIONREST, "");

    AddTerm(")", ADDITIONREST, "");
    AddTerm(";", ADDITIONREST, "");

    AddTerm("+", ADDITIONREST, "+");
    AddNonTerm("+", ADDITIONREST, TERM_OFFSET + MULTIPLY);
    AddNonTerm("+", ADDITIONREST, TERM_OFFSET + ADDITIONREST);

    AddTerm("-", ADDITIONREST, "-");
    AddNonTerm("-", ADDITIONREST, TERM_OFFSET + MULTIPLY);
    AddNonTerm("-", ADDITIONREST, TERM_OFFSET + ADDITIONREST);

    // Multiply
    AddNonTerm("IDENTIFIER", MULTIPLY, TERM_OFFSET + ATOM);
    AddNonTerm("IDENTIFIER", MULTIPLY, TERM_OFFSET + MULTIPLYREST);
    AddNonTerm("LITERAL", MULTIPLY, TERM_OFFSET + ATOM);
    AddNonTerm("LITERAL", MULTIPLY, TERM_OFFSET + MULTIPLYREST);

    // Multiply rest
    AddTerm("<", MULTIPLYREST, "");
    AddTerm(">", MULTIPLYREST, "");
    AddTerm("<=", MULTIPLYREST, "");
    AddTerm(">=", MULTIPLYREST, "");
    AddTerm("!=", MULTIPLYREST, "");
    AddTerm("==", MULTIPLYREST, "");

    AddTerm("+", MULTIPLYREST, "");
    AddTerm("-", MULTIPLYREST, "");

    AddTerm(")", MULTIPLYREST, "");
    AddTerm(";", MULTIPLYREST, "");

    AddTerm("*", MULTIPLYREST, "*");
    AddNonTerm("*", MULTIPLYREST, TERM_OFFSET + MULTIPLY);
    AddNonTerm("*", MULTIPLYREST, TERM_OFFSET + MULTIPLYREST);

    AddTerm("/", MULTIPLYREST, "/");
    AddNonTerm("/", MULTIPLYREST, TERM_OFFSET + MULTIPLY);
    AddNonTerm("/", MULTIPLYREST, TERM_OFFSET + MULTIPLYREST);

    // Atom
    AddNonTerm("IDENTIFIER", ATOM, TERM_OFFSET + IDENTIFIER_TERM);
    AddNonTerm("LITERAL", ATOM, TERM_OFFSET + LITERAL_TERM);

    // TYPE
    AddTerm("void", TYPE, "void");
    AddTerm("int", TYPE, "int");
    AddTerm("float", TYPE, "float");
    AddTerm("bool", TYPE, "bool");
    AddTerm("char", TYPE, "char");
}

#pragma endregion
// Function is destructive to input tokens
Error *AnalyzeSyntax(GeneralList *tokens, TreeNode *output)
{
    Error *error = (Error *)malloc(sizeof(Error));
    memset(error, 0, sizeof(Error));
    error->errorCode = NO_ERROR;

    if (output == NULL)
    {
        SetError(error, UNEXPECTED_NULL, SYNTAX_ANALYZER, "Missing output tree reference", 0);
        goto exit;
    }

    TreeNode *tree = output;

    GeneralList stack;
    InitializeList(&stack);

    GeneralList treeNavStack;
    InitializeList(&treeNavStack);

    // Push body non terminator
    PushByte(&stack, TERM_OFFSET + 0);
    uint32_t tokenIndex = 0;

    // Check for when buffer empty and stack can be epsilon

    while (stack.count > 0)
    {
        if (stack.count == 0 && tokens->count == 0)
        {
            printf("Parse success");
            break;
        }
        uint8_t *top = PeekList(&stack);
        // If epsilon then pop stack
        if ((*top) == '\0')
        {
            free(PopList(&stack));
            uint8_t *topValue = (uint8_t *)PeekList(&treeNavStack);
            if ((*topValue) == 0)
            {
                // Go to the next left node
                while ((*topValue) == 0)
                {
                    free(PopList(&treeNavStack));
                    if (treeNavStack.count == 0)
                    {
                        // Reached root node
                        printf("tree root found, exitting\n");
                        goto exit;
                    }
                    topValue = (uint8_t *)PeekList(&treeNavStack);
                }
            }
            (*topValue)--;
            continue;
        }
        Token *token = (Token *)ListGetIndex(tokens, 0);
        if (token == NULL)
        {
            goto CheckNonTerminator;
        }
        bool isMatch = false;
        isMatch = (*top) < 128 && (strcmp((char *)top, token->value) == 0);
        if ((*top) >= 128)
        {
            if ((*top) - TERM_OFFSET == IDENTIFIER_TERM && token->type == IDENTIFIER)
                isMatch = true;
            if ((*top) - TERM_OFFSET == LITERAL_TERM && token->type == LITERAL)
                isMatch = true;
        }
        printf("Stack Top: %d\n", *top);

        if (isMatch)
        {
            printf("Match, pop: %s\n", token->value);
            free(PopList(&stack));
            free(PopListFirst(tokens));

            uint8_t *topValue = (uint8_t *)PeekList(&treeNavStack);
            if ((*topValue) == 0)
            {
                // Go to the next left node
                while ((*topValue) == 0)
                {
                    free(PopList(&treeNavStack));
                    if (treeNavStack.count == 0)
                    {
                        // Reached root node
                        printf("tree root found, exitting\n");
                        goto exit;
                    }
                    topValue = (uint8_t *)PeekList(&treeNavStack);
                }
            }
            (*topValue)--;
        }
        else if ((*top) >= 128)
        {
        CheckNonTerminator:
            uint8_t matchingTerm = 255;
            if (token == NULL)
            {
                matchingTerm = GetTermIndex("\0");
            }
            else
            {
                matchingTerm = GetTermIndex(token->value);
                if (token->type == IDENTIFIER)
                {
                    printf("Term is identifier\n");
                    matchingTerm = GetTermIndex("IDENTIFIER");
                }
                if (token->type == LITERAL)
                {
                    printf("Term is literal\n");
                    matchingTerm = GetTermIndex("LITERAL");
                }
                if (matchingTerm == 255)
                {
                    printf("ErrorA\n");
                    EmptyList(&stack);
                    EmptyList(tokens);
                    SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
                    goto exit;
                }
            }

            printf("Finding rule %d, %d\n", matchingTerm, (*top) - TERM_OFFSET);
            GeneralList *rule = &grammarTable[(*top) - TERM_OFFSET][matchingTerm];

            if (rule->count == 0)
            {
                printf("ErrorC\n");
                for (int i = 0; i < tokens->count; i++)
                {
                    printf("%s\n", ((Token *)ListGetIndex(tokens, i))->value);
                }
                EmptyList(&stack);
                EmptyList(tokens);
                SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
                goto exit;
            }
            free(PopList(&stack));
            for (int i = rule->count - 1; i >= 0; i--)
            {
                char *newRule = (char *)ListGetIndex(rule, i);
                printf("Push rule\n");
                if ((*newRule) >= 128)
                {
                    printf("%d\n", (*newRule) - TERM_OFFSET);
                    PushByte(&stack, (uint8_t)(*newRule));

                    uint8_t *nodeValue = (uint8_t *)malloc(sizeof(uint8_t));
                    (*nodeValue) = (uint8_t)(*newRule);
                    AppendTreeFromRoot(tree, &treeNavStack, nodeValue);
                }
                if ((*newRule) < 128)
                {
                    printf("%s\n", newRule);
                    PushCharArr(&stack, newRule);

                    char *copiedChar = (char *)malloc(strlen(newRule) + 1);
                    strcpy(copiedChar, newRule);
                    AppendTreeFromRoot(tree, &treeNavStack, copiedChar);
                }
            }
            if (rule->count > 0)
            {
                // Move navigator to the last element
                PushByte(&treeNavStack, rule->count - 1);
            }
            printf("OK\n");
        }
        else
        {
            printf("ErrorB\n");
            EmptyList(&stack);
            EmptyList(tokens);
            SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
            goto exit;
        }
        sleep_ms(100);
    }

exit:
    return error;
}

#endif