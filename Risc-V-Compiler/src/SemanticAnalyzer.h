#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "Common.h"
#include "SyntaxAnalyzer.h"

Error *SemanticAnylsis(TreeNode *AST)
{
    printf("semantic analysis\n");
    Error *error = (Error *)malloc(sizeof(Error));
    memset(error, 0, sizeof(Error));
    error->errorCode = NO_ERROR;

    GeneralList exploreMap;
    InitializeList(&exploreMap);

    GeneralList variableStack;
    InitializeList(&variableStack);

    GeneralList tableIndexices;
    InitializeList(&tableIndexices);

    if (AST == NULL)
    {
        SetError(error, UNEXPECTED_NULL, SEMANTIC_ANALYZER, "Missing AST reference", 0);
        goto exit;
    }

    TreeNode *currentNode = AST;

    printf("Initialized semantic analysis\n");

    // go to first node that is not root.
    if (AST->children.count == 0)
    {
        goto exit;
    }
    PushByte(&exploreMap, currentNode->children.count - 1);
    currentNode = (TreeNode *)PeekList(&currentNode->children);

    while (true)
    {
        printf("semantic loop, map: ");
        for (int i = 0; i < exploreMap.count; i++)
        {
            printf("%d", (int)*((uint8_t *)ListGetIndex(&exploreMap, i)));
        }
        printf("\n");

        char *value = currentNode->content;
        if (value == NULL || value[0] == '\0')
        {
            // Reached epsilon, backtrack
            // Seems odd, probably due to a bad CFG
            printf("epsilon value, backtracking. May need to fix CFG\n");
            goto backtrack;
        }

        if (*((uint8_t *)value) < TERM_OFFSET)
        {
            printf("%s\n", value);
            if (value[0] == '{')
            {
                // Open a new context;
                printf("opened a context\n");
                PushByte(&tableIndexices, variableStack.count);
            }
            if (value[0] == '}')
            {
                // Close a context;
                printf("closed a context\n");
                if (tableIndexices.count == 0)
                {
                    SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "Missing matching '{'", 0);
                    goto exit;
                }
                uint8_t *startIndex = PopList(&tableIndexices);
                while (variableStack.count > *startIndex)
                {
                    Identifier *variable = PopList(&variableStack);
                    free(variable->name);
                    free(variable);
                }
                free(startIndex);
            }
        }
        else
        {
            printf("%d\n", (int)(*(uint8_t *)value));
            uint8_t termValue = *(uint8_t *)value - TERM_OFFSET;
            if (termValue == IDENTIFIER_TERM)
            {
                bool isError = true;
                char *symbol = (char *)((TreeNode *)PeekList(&currentNode->children))->content;
                for (int i = 0; i < variableStack.count; i++)
                {
                    char *checkSymbol = ((Identifier *)ListGetIndex(&variableStack, i))->name;
                    printf("comparing to symbol: %s\n", checkSymbol);
                    if (strcmp(checkSymbol, symbol) == 0)
                    {
                        printf("Found matching symbol, no error\n");
                        isError = false;
                        break;
                    }
                }
                if (isError)
                {
                    printf("couldn't find symbol %s\n", symbol);
                    SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol is not defined in current scope", 0);
                    goto exit;
                }
            }
        }
        if (*((uint8_t *)value) == TERM_OFFSET + DECLARATION)
        {
            TreeNode *typeNode = (TreeNode *)ListGetIndex(&currentNode->children, 2);
            typeNode = (TreeNode *)ListGetIndex(&typeNode->children, 0);

            TreeNode *nameNode = (TreeNode *)ListGetIndex(&currentNode->children, 1);
            nameNode = (TreeNode *)ListGetIndex(&nameNode->children, 0);

            Identifier *newVar = (Identifier *)malloc(sizeof(Identifier));

            newVar->name = malloc(strlen((char *)nameNode->content) + 1);
            strcpy(newVar->name, (char *)nameNode->content);

            newVar->type = ParseType((char *)typeNode->content);
            PushList(&variableStack, newVar);
            printf("pushed var to symbol table\n");
        }

        if (currentNode->children.count == 0)
        {
        backtrack:
            uint8_t *topValue = (uint8_t *)PeekList(&exploreMap);
            // Go to the next left node
            while ((*topValue) == 0)
            {
                free(PopList(&exploreMap));
                if (exploreMap.count == 0)
                {
                    // Reached root node
                    printf("tree root found, exitting\n");
                    goto exit;
                }
                topValue = (uint8_t *)PeekList(&exploreMap);
            }

            (*topValue)--;
            currentNode = TreeGetNode(AST, &exploreMap);
        }
        else
        {
            PushByte(&exploreMap, currentNode->children.count - 1);
            currentNode = (TreeNode *)PeekList(&currentNode->children);
        }
    }

exit:
    // Need to add list deinitialization
    return error;
}

#endif