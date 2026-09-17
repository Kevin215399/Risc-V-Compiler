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
            if ((*top) - TERM_OFFSET == TYPE_TERM && token->type == TYPE)
                isMatch = true;
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
                    char *copiedChar = (char *)malloc(strlen(token->value) + 1);
                    strcpy(copiedChar, token->value);
                    AppendTreeFromRoot(output, &treeNavStack, copiedChar);
                }
            }

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
                    EmptyList(&stack);
                    EmptyList(tokens);
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
                EmptyList(&stack);
                EmptyList(tokens);
                SetError(error, SYNTAX_ERROR, SYNTAX_ANALYZER, "Unexpected syntax", 0);
                goto exit;
            }

            GeneralList *rule = &(((GrammarRule *)ListGetIndex(&grammarTable, ruleIndex))->stack);

            free(PopList(&stack));
            for (int i = rule->count - 1; i >= 0; i--)
            {
                char *newRule = (char *)ListGetIndex(rule, i);
                printf("Push rule %d\n", (int)(*newRule));
                if ((*newRule) >= 128)
                {
                    printf("%d\n", (*newRule) - TERM_OFFSET);
                    PushByte(&stack, (uint8_t)(*newRule));

                    uint8_t *nodeValue = (uint8_t *)malloc(sizeof(uint8_t));
                    (*nodeValue) = (uint8_t)(*newRule);
                    AppendTreeFromRoot(output, &treeNavStack, nodeValue);
                }
                if ((*newRule) < 128)
                {
                    printf("%s\n", newRule);
                    PushCharArr(&stack, newRule);

                    char *copiedChar = (char *)malloc(strlen(newRule) + 1);
                    strcpy(copiedChar, newRule);
                    AppendTreeFromRoot(output, &treeNavStack, copiedChar);
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
    }

exit:
    // printf("%d\n", (int)((TreeNode *)output->children.firstElement->content)->children.count);
    return error;
}

#endif