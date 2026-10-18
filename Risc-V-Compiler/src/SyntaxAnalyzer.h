#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H
#include "Common.h"
#include "CFG.h"
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

    GeneralList stack;
    InitializeList(&stack);

    GeneralList treeNavStack;
    InitializeList(&treeNavStack);

    // Push body non terminator
    Token *newToken = (Token *)malloc(sizeof(Token));
    newToken->lineNumber = 0;
    newToken->type = CFG_RULE;
    newToken->value = (char *)(uint8_t *)malloc(sizeof(uint8_t));
    (*newToken->value) = GLOBAL_BODY + TERM_OFFSET;
    PushList(&stack, newToken);
    uint32_t tokenIndex = 0;

    // Check for when buffer empty and stack can be epsilon

    while (stack.count > 0)
    {
        printf("Stack count: %d\n", stack.count);
        printf("tree map:");
        for (int i = 0; i < treeNavStack.count; i++)
        {
            printf("%d", (int)*(uint8_t *)ListGetIndex(&treeNavStack, i));
        }
        printf("\n");
        if (stack.count == 0 && tokens->count == 0)
        {
            printf("Parse success");
            break;
        }
        Token *stackToken = stack.lastElement->content;

        if (stackToken == NULL)
        {
            printf("NULL\n");
            sleep_ms(100);
            SetError(error, UNEXPECTED_NULL, SYNTAX_ANALYZER, "Lowkey, I don't even know what this means. Stacktoken is null", 0);
            goto exit;
        }
        uint8_t *top = stackToken->value;
        if (top == NULL)
        {
            printf("NULL\n");
            SetError(error, UNEXPECTED_NULL, SYNTAX_ANALYZER, "Lowkey, I don't even know what this means. Stacktoken is null", 0);
            goto exit;
        }
        // If epsilon then pop stack
        if ((*top) == '\0')
        {
            printf("epsilon\n");
            Token *delToken = (Token *)PopList(&stack);
            free(delToken->value);
            free(delToken);
            uint8_t *topValue = (uint8_t *)PeekList(&treeNavStack);
            // Go to the next left node
            while ((*topValue) == 0)
            {
                printf("tree pop up\n");
                free(PopList(&treeNavStack));
                if (treeNavStack.count == 0)
                {
                    // Reached root node
                    printf("tree root found, exitting\n");
                    break;
                }
                topValue = (uint8_t *)PeekList(&treeNavStack);
            }

            (*topValue)--;
            continue;
        }
        Token *token = (Token *)PeekListFirst(tokens);
        if (token == NULL)
        {
            goto CheckNonTerminator;
        }
        bool isMatch = false;
        isMatch = (*top) < 128 && (strcmp((char *)top, token->value) == 0);
        if ((*top) >= 128)
        {
            printf("top >= 128 \n");
            if ((*top) - TERM_OFFSET == IDENTIFIER_TERM && token->type == IDENTIFIER)
                isMatch = true;
            if ((*top) - TERM_OFFSET == LITERAL_TERM && token->type == LITERAL)
                isMatch = true;
            if ((*top) - TERM_OFFSET == TYPE_TERM && token->type == TYPE)
                isMatch = true;

            if ((*top) == EXPRESSION + TERM_OFFSET)
            {
                printf("Expression\n");
                uint8_t parenthesis = 0;
                GeneralList expressionTokens;
                InitializeList(&expressionTokens);

                while (token->value[0] != ';')
                {

                    if (token->value[0] == '(' || token->value[0] == '[')
                    {
                        parenthesis++;
                    }
                    if (token->value[0] == ')' || token->value[0] == ']')
                    {
                        if (parenthesis == 0)
                        {
                            break;
                        }
                        parenthesis--;
                    }
                    printf("expression scope, pop %s\n", token->value);
                    for (int i = 0; i < treeNavStack.count; i++)
                    {
                        printf("%d", *(uint8_t *)ListGetIndex(&treeNavStack, i));
                    }
                    AppendTreeFromRoot(output, &treeNavStack, (Token *)PopListFirst(tokens));

                    token = (Token *)PeekListFirst(tokens);
                }
                printf("Pop expression\n");

                isMatch = true;
            }
        }
        printf("Stack Top: %d\n", *top);

        if (isMatch)
        {
            if ((*top) >= 128)
            {
                if (
                    ((*top) - TERM_OFFSET == IDENTIFIER_TERM && token->type == IDENTIFIER) ||
                    ((*top) - TERM_OFFSET == LITERAL_TERM && token->type == LITERAL) ||
                    ((*top) - TERM_OFFSET == TYPE_TERM && token->type == TYPE))
                {
                    printf("literal, appending child as literal");
                    AppendTreeFromRoot(output, &treeNavStack, PopListFirst(tokens));
                }
            }
            else
            {
                Token *delToken = (Token *)PopListFirst(tokens);
                free(delToken->value);
                free(delToken);
            }

            printf("Match, pop: %s\n", token->value);
            printf("popping token...");
            Token *delToken = (Token *)PopList(&stack);
            free(delToken->value);
            free(delToken);
            printf("popped\n");

            uint8_t *topValue = (uint8_t *)PeekList(&treeNavStack);
            printf("sibling index count: %d\n", (int)*topValue);

            // Go to the next left node
            while ((*topValue) == 0)
            {
                printf("tree pop up\n");
                free(PopList(&treeNavStack));
                if (treeNavStack.count == 0)
                {
                    // Reached root node
                    printf("tree root found, exitting\n");
                    goto exit;
                }
                topValue = (uint8_t *)PeekList(&treeNavStack);
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
                printf("finding match for %s\n", token->value);
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
                    SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
                    goto exit;
                }
            }

            printf("Finding rule %d, %d\n", matchingTerm, (*top) - TERM_OFFSET);
            uint16_t ruleIndex = FindRuleInCFG(matchingTerm, (*top) - TERM_OFFSET);

            if (ruleIndex == (uint16_t)(-1))
            {
                printf("ErrorC no matching rule\n");
                for (int i = 0; i < tokens->count; i++)
                {
                    printf("%s\n", ((Token *)ListGetIndex(tokens, i))->value);
                }
                SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
                goto exit;
            }

            GeneralList *rule = &(((GrammarRule *)ListGetIndex(&grammarTable, ruleIndex))->stack);

            Token *delToken = (Token *)PopList(&stack);
            free(delToken->value);
            free(delToken);
            for (int i = rule->count - 1; i >= 0; i--)
            {
                char *newRule = (char *)ListGetIndex(rule, i);
                printf("Push rule %d\n", (int)(*newRule));
                if ((*newRule) >= 128)
                {
                    printf("%d\n", (*newRule) - TERM_OFFSET);
                    // Push it to the analyzer stack
                    newToken = (Token *)malloc(sizeof(Token));
                    newToken->lineNumber = token->lineNumber;
                    newToken->type = CFG_RULE;
                    newToken->value = (uint8_t *)malloc(sizeof(uint8_t));
                    (*newToken->value) = (uint8_t)(*newRule);
                    PushList(&stack, newToken);

                    // Push it to the AST
                    newToken = (Token *)malloc(sizeof(Token));
                    newToken->lineNumber = token->lineNumber;
                    newToken->type = CFG_RULE;
                    newToken->value = (uint8_t *)malloc(sizeof(uint8_t));
                    (*newToken->value) = (uint8_t)(*newRule);
                    AppendTreeFromRoot(output, &treeNavStack, newToken);
                }
                if ((*newRule) < 128)
                {
                    printf("%s\n", newRule);

                    // Push it to the analyzer stack
                    newToken = (Token *)malloc(sizeof(Token));
                    newToken->lineNumber = token->lineNumber;
                    newToken->type = NONE;
                    newToken->value = (char *)malloc(strlen(newRule) + 1);
                    strcpy(newToken->value, newRule);
                    PushList(&stack, newToken);

                    // Push it to the AST
                    newToken = (Token *)malloc(sizeof(Token));
                    newToken->lineNumber = token->lineNumber;
                    newToken->type = NONE;
                    newToken->value = (char *)malloc(strlen(newRule) + 1);
                    strcpy(newToken->value, newRule);
                    AppendTreeFromRoot(output, &treeNavStack, newToken);
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
            SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
            goto exit;
        }
    }

    return error;
exit:
    // printf("%d\n", (int)((TreeNode *)output->children.firstElement->content)->children.count);
    for (int i = 0; i < stack.count; i++)
    {
        Token *delToken = (Token *)PopList(&stack);
        free(delToken->value);
        free(delToken);
    }
    for (int i = 0; i < tokens->count; i++)
    {
        Token *delToken = (Token *)PopList(tokens);
        free(delToken->value);
        free(delToken);
    }
    ClearTree(output);
    return error;
}

#endif