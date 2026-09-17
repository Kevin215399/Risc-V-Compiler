#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "Common.h"
#include "SyntaxAnalyzer.h"

typedef struct TypeMarker
{
    IdentifierType type;
    uint8_t pointerDepth;
    TreeNode *linkedNode;
    FuncOrVar funcOrVar;
} TypeMarker;

#define NULL_DEPTH 255

void AddTypeMarker(GeneralList *stack, TreeNode *link, IdentifierType type, uint8_t pointerDepth, FuncOrVar funcOrVar)
{
    TypeMarker *newMarker = (TypeMarker *)malloc(sizeof(TypeMarker));
    newMarker->linkedNode = link;
    newMarker->type = type;
    newMarker->pointerDepth = pointerDepth;
    newMarker->funcOrVar = funcOrVar;
    PushList(stack, newMarker);
    printf("Flagged node as %d\n", type);
}
TypeMarker *ReadMarkerOnNode(GeneralList *stack, TreeNode *node)
{
    for (int i = 0; i < stack->count; i++)
    {
        TypeMarker *compareMarker = (TypeMarker *)ListGetIndex(stack, i);
        if (compareMarker->linkedNode == node)
        {
            return compareMarker;
        }
    }
    return NULL;
}

void CopyMarker(GeneralList *stack, TreeNode *target, TreeNode *source)
{
    TypeMarker *originalMarker = ReadMarkerOnNode(stack, source);
    if (originalMarker != NULL)
    {
        // Copy the marker
        AddTypeMarker(stack, target, originalMarker->type, originalMarker->pointerDepth, originalMarker->funcOrVar);
        printf("Copied marker from child\n");
    }
}

uint8_t GetPointerDepth(TreeNode *parent)
{
    uint8_t count = 0;
    TreeNode *currentNode = parent;
    while (currentNode->children.count > 0)
    {
        count++;
        currentNode = (TreeNode *)ListGetIndex(&currentNode->children, 0);
    }
    count--;
    printf("Pointer depth: %d\n", (int)count);
    return count;
}

Identifier *FindSymbol(GeneralList *variableStack, char *symbol)
{
    bool isError = true;
    for (int i = 0; i < variableStack->count; i++)
    {
        Identifier *checkSymbol = (Identifier *)ListGetIndex(variableStack, i);
        printf("comparing to symbol: %s\n", checkSymbol->name);
        if (strcmp(checkSymbol->name, symbol) == 0)
        {
            printf("Found matching symbol, no error\n");
            return checkSymbol;
        }
    }
    return NULL;
}

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

    GeneralList typeMarkers;
    InitializeList(&typeMarkers);

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
                char *symbol = (char *)((TreeNode *)PeekList(&currentNode->children))->content;
                Identifier *variable = FindSymbol(&variableStack, symbol);
                if (variable != NULL)
                {
                    printf("Found matching symbol, no error\n");
                    AddTypeMarker(&typeMarkers, currentNode, variable->type, variable->pointerDepth, variable->funcOrVar);
                }
                else
                {
                    printf("couldn't find symbol %s\n", symbol);
                    SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol is not defined in current scope", 0);
                    goto exit;
                }
            }
            if (termValue == LITERAL_TERM)
            {
                char *symbol = (char *)((TreeNode *)PeekList(&currentNode->children))->content;
                if (strcmp(symbol, "true") == 0)
                {
                    AddTypeMarker(&typeMarkers, currentNode, UINT8_T, 0, NEITHER);
                    printf("true literal\n");
                }
                else if (strcmp(symbol, "false") == 0)
                {
                    AddTypeMarker(&typeMarkers, currentNode, UINT8_T, 0, NEITHER);
                    printf("false literal\n");
                }
                else if (strcmp(symbol, "NULL") == 0)
                {
                    AddTypeMarker(&typeMarkers, currentNode, VOID, NULL_DEPTH, NEITHER);
                    printf("NULL literal\n");
                }
                else if (symbol[0] == '\"')
                {
                    AddTypeMarker(&typeMarkers, currentNode, UINT8_T, 1, NEITHER);
                    printf("string literal\n");
                }
                else if (IsNumber(symbol[0]) || symbol[0] == '.')
                {
                    if (strstr(symbol, ".") != NULL)
                    {
                        float value = 0;
                        int success = sscanf(symbol, "%f", &value);
                        if (success == 0)
                        {
                            SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "Unkown literal", 0);
                            goto exit;
                        }
                        AddTypeMarker(&typeMarkers, currentNode, FLOAT, 0, NEITHER);
                        printf("float literal\n");
                    }
                    else
                    {
                        int value = 0;
                        int success = sscanf(symbol, "%d", &value);
                        if (success == 0)
                        {
                            SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "Unkown literal", 0);
                            goto exit;
                        }

                        if (value > 2147483647)
                        {
                            AddTypeMarker(&typeMarkers, currentNode, UINT32_T, 0, NEITHER);
                        }
                        else
                        {
                            AddTypeMarker(&typeMarkers, currentNode, INT, 0, NEITHER);
                        }
                        printf("integer literal\n");
                    }
                }
            }

            if (termValue == TYPE_TERM)
            {
                TreeNode *node = (TreeNode *)PeekList(&currentNode->children);
                if (node == NULL)
                {
                typeRuleMissingType:
                    SetError(error, AST_STRUCTURE, SEMANTIC_ANALYZER, "TYPE rule missing type", 0);
                    goto exit;
                }

                char *type = (char *)node->content;
                if (type == NULL)
                    goto typeRuleMissingType;

                AddTypeMarker(&typeMarkers, currentNode, ParseType(type), 0, NEITHER);
                printf("Type term added marker\n");
            }
        }
        if (*((uint8_t *)value) == TERM_OFFSET + DECLERATION)
        {
            TreeNode *typeNode = (TreeNode *)ListGetIndex(&currentNode->children, 4);
            typeNode = (TreeNode *)ListGetIndex(&typeNode->children, 0);

            TreeNode *nameNode = (TreeNode *)ListGetIndex(&currentNode->children, 2);
            nameNode = (TreeNode *)ListGetIndex(&nameNode->children, 0);

            if (FindSymbol(&variableStack, (char *)nameNode->content) != NULL)
            {
                SetError(error, DUPLICATE, SEMANTIC_ANALYZER, "Symbol was already defined earlier", 0);
                goto exit;
            }

            Identifier *newVar = (Identifier *)malloc(sizeof(Identifier));

            newVar->name = malloc(strlen((char *)nameNode->content) + 1);
            strcpy(newVar->name, (char *)nameNode->content);

            newVar->type = ParseType((char *)typeNode->content);

            TreeNode *pointerNode = (TreeNode *)ListGetIndex(&currentNode->children, 3);

            newVar->pointerDepth = GetPointerDepth(pointerNode);

            TreeNode *decrest = (TreeNode *)ListGetIndex(&currentNode->children, 0);
            TreeNode *funcOrVar = (TreeNode *)PeekList(&decrest->children);
            if (*((uint8_t *)funcOrVar->content) == FUNC_DEC_REST)
            {
                newVar->funcOrVar = FUNCTION;
            }
            else
            {
                newVar->funcOrVar = VARIABLE;
            }

            PushList(&variableStack, newVar);
            printf("pushed %s to symbol table\n", newVar->name);
        }
        if (*((uint8_t *)value) == TERM_OFFSET + FUNCTION_PARAMS)
        {
            if (currentNode->children.count == 5)
            {
                TreeNode *typeNode = (TreeNode *)ListGetIndex(&currentNode->children, 4);
                typeNode = (TreeNode *)ListGetIndex(&typeNode->children, 0);

                TreeNode *nameNode = (TreeNode *)ListGetIndex(&currentNode->children, 2);
                nameNode = (TreeNode *)ListGetIndex(&nameNode->children, 0);

                if (FindSymbol(&variableStack, (char *)nameNode->content) != NULL)
                {
                    SetError(error, DUPLICATE, SEMANTIC_ANALYZER, "Symbol was already defined earlier", 0);
                    goto exit;
                }

                Identifier *newVar = (Identifier *)malloc(sizeof(Identifier));

                newVar->name = malloc(strlen((char *)nameNode->content) + 1);
                strcpy(newVar->name, (char *)nameNode->content);

                newVar->type = ParseType((char *)typeNode->content);

                TreeNode *pointerNode = (TreeNode *)ListGetIndex(&currentNode->children, 3);

                newVar->pointerDepth = GetPointerDepth(pointerNode);

                newVar->funcOrVar = VARIABLE;

                PushList(&variableStack, newVar);
                printf("pushed %s to symbol table\n", newVar->name);
            }
        }

        if (currentNode->children.count == 0)
        {
        backtrack:
            uint8_t *topValue = (uint8_t *)PeekList(&exploreMap);
            // Go to the next left node
            while ((*topValue) == 0)
            {
                // Go up
                free(PopList(&exploreMap));
                currentNode = currentNode->parent;
                printf("exploring UP: ");

                if (exploreMap.count == 0)
                {
                    // Reached root node
                    printf("tree root found, exitting\n");
                    goto exit;
                }
                topValue = (uint8_t *)PeekList(&exploreMap);
                // "drag" types upwards if they exisSt
                uint8_t term = (*((uint8_t *)currentNode->content) - TERM_OFFSET);
                printf("%d\n", term);
                if (term == ATOM)
                {
                    printf("backtracked to atom, checking children\n");
                    TreeNode *child = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                    CopyMarker(&typeMarkers, currentNode, child);
                }
                else if (term == EQUALITY)
                {
                    printf("backtracked to EQUALITY, checking children\n");
                    goto operatorCopy;
                }
                else if (term == ADDITION)
                {
                    printf("backtracked to ADDITION, checking children\n");
                    goto operatorCopy;
                }
                else if (term == MULTIPLY)
                {
                    printf("backtracked to MULTIPLY, checking children\n");
                    goto operatorCopy;
                }
                else if (term == EQUALITYREST)
                {
                    printf("backtracked to EQUALITYREST, checking children\n");
                    goto operatorCopy;
                }
                else if (term == ADDITIONREST)
                {
                    printf("backtracked to ADDITIONREST, checking children\n");
                    goto operatorCopy;
                }
                else if (term == MULTIPLYREST)
                {
                    printf("backtracked to MULTIPLYREST, checking children\n");
                operatorCopy:
                    if (currentNode->children.count >= 2)
                    {
                        TreeNode *first = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TreeNode *last = (TreeNode *)ListGetIndex(&currentNode->children, 1);
                        TypeMarker *firstMarker = ReadMarkerOnNode(&typeMarkers, first);
                        TypeMarker *lastMarker = ReadMarkerOnNode(&typeMarkers, last);
                        if (firstMarker == NULL && lastMarker != NULL)
                        {
                            CopyMarker(&typeMarkers, currentNode, last);
                            printf("operation is epsilon, dragging\n");
                        }
                        else if (firstMarker != NULL &&
                                 lastMarker != NULL &&
                                 firstMarker->type == lastMarker->type &&
                                 firstMarker->pointerDepth == 0 &&
                                 lastMarker->pointerDepth == 0 &&
                                 firstMarker->funcOrVar != FUNCTION &&
                                 lastMarker->funcOrVar != FUNCTION)
                        {
                            CopyMarker(&typeMarkers, currentNode, last);
                            printf("operation valid, dragging\n");
                        }
                        else
                        {
                            printf("operation invalid\n");
                            SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Incompatible types", 0);
                            goto exit;
                        }
                    }
                }
                else if (term == ASSIGN)
                {
                    printf("Validating assignment\n");
                    TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                    TreeNode *destination = (TreeNode *)ListGetIndex(&currentNode->children, 1);
                    if (*((uint8_t *)source->children.firstElement->content) == FUNC_DEC_REST + TERM_OFFSET)
                    {
                        // It is actually a function call
                        //  do nothing
                    }
                    else
                    {
                        TypeMarker *sourceType = ReadMarkerOnNode(&typeMarkers, source);
                        TypeMarker *destinationType = ReadMarkerOnNode(&typeMarkers, destination);
                        if (sourceType == NULL ||
                            destinationType == NULL ||
                            sourceType->funcOrVar == FUNCTION ||
                            destinationType->funcOrVar == FUNCTION ||
                            sourceType->pointerDepth != destinationType->pointerDepth ||
                            sourceType->type != destinationType->type)
                        {
                            printf("assignment invalid\n");
                            SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Incompatible types", 0);
                            goto exit;
                        }
                    }
                }
                else if (term == ASSIGN_REST)
                {
                    printf("backtracked to ASSIGN_REST, checking children\n");
                    TreeNode *operation = (TreeNode *)ListGetIndex(&currentNode->children, 1);
                    TreeNode *child = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                    if (*((uint8_t *)operation->content) != '=')
                    {
                        TypeMarker *type = ReadMarkerOnNode(&typeMarkers, child);
                        if (type->type > FLOAT || type->pointerDepth > 0)
                        {
                            printf("assign_rest invalid\n");
                            SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Incompatible types", 0);
                            goto exit;
                        }
                    }
                    CopyMarker(&typeMarkers, currentNode, child);
                }
                else if (term == EXPRESSION)
                {
                    printf("backtracked to EXPRSSION, checking children\n");
                    TreeNode *child = (TreeNode *)PeekList(&currentNode->children);
                    TypeMarker *childMarker = ReadMarkerOnNode(&typeMarkers, child);
                    if (childMarker == NULL)
                    {
                        printf("expression invalid\n");
                        SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Expression is empty", 0);
                        goto exit;
                    }
                    CopyMarker(&typeMarkers, currentNode, child);
                }
                else if (term == UNARY_ATOM)
                {
                    printf("unary backtrack\n");
                    TreeNode *lastChild = (TreeNode *)PeekList(&currentNode->children);
                    uint8_t *first = (uint8_t *)lastChild->content;
                    if /*atom Drag*/ (*first == ATOM + TERM_OFFSET)
                    {
                        // Only has child of type atom
                        printf("atom drag\n");
                        CopyMarker(&typeMarkers, currentNode, lastChild);
                    }
                    if /*cast unary*/ (*first == '(')
                    {
                        // Only has child of type atom
                        printf("cast drag\n");
                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        CopyMarker(&typeMarkers, currentNode, source);
                    }
                    if /*reference*/ (*first == '&')
                    {
                        printf("Ampersand\n");

                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TypeMarker *originalMarker = ReadMarkerOnNode(&typeMarkers, source);
                        if (originalMarker != NULL)
                        {
                            if (originalMarker->funcOrVar != VARIABLE)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "\"&\" must be used on variables", 0);
                                goto exit;
                            }
                            if (originalMarker->pointerDepth == NULL_DEPTH)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot reference a NULL value", 0);
                                goto exit;
                            }
                            printf("referencing, %d, %d, %d\n", (int)originalMarker->type, (int)originalMarker->pointerDepth, originalMarker->funcOrVar);
                            // Copy the marker
                            AddTypeMarker(&typeMarkers, currentNode, originalMarker->type, originalMarker->pointerDepth + 1, NEITHER);
                            printf("Copied marker from child\n");
                        }
                        else
                        {
                            printf("nothing to reference\n");
                        }
                    }
                    if /*dereference*/ (*first == '*')
                    {
                        printf("dereference\n");

                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TypeMarker *originalMarker = ReadMarkerOnNode(&typeMarkers, source);
                        if (originalMarker != NULL)
                        {
                            if (originalMarker->pointerDepth == 0)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot dereference a non-pointer", 0);
                                goto exit;
                            }
                            if (originalMarker->pointerDepth == NULL_DEPTH)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot dereference a NULL value", 0);
                                goto exit;
                            }

                            if (originalMarker->funcOrVar == FUNCTION)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot dereference a function", 0);
                                goto exit;
                            }
                            // Copy the marker
                            AddTypeMarker(&typeMarkers, currentNode, originalMarker->type, originalMarker->pointerDepth - 1, NEITHER);
                            printf("Copied marker from child\n");
                        }
                    }
                    if /*Not*/ (*first == '!')
                    {
                        printf("Not\n");

                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TypeMarker *originalMarker = ReadMarkerOnNode(&typeMarkers, source);
                        if (originalMarker != NULL)
                        {
                            if (originalMarker->funcOrVar == FUNCTION)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot not a function", 0);
                                goto exit;
                            }
                            if (originalMarker->pointerDepth > 0)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Not can not be used on pointers, derefence first", 0);
                                goto exit;
                            }
                            if (originalMarker->type != BOOL)
                            {
                                SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Not cannot be used on types that are not bool. Use \"~\" for integers", 0);
                                goto exit;
                            }

                            // Copy the marker
                            AddTypeMarker(&typeMarkers, currentNode, originalMarker->type, originalMarker->pointerDepth, NEITHER);
                            printf("Copied marker from child\n");
                        }
                    }
                    if /*Unary Subtract*/ (*first == '-')
                    {
                        printf("Unary Subtract\n");

                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TypeMarker *originalMarker = ReadMarkerOnNode(&typeMarkers, source);
                        if (originalMarker != NULL)
                        {
                            if (originalMarker->funcOrVar == FUNCTION)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot unary subtract a function", 0);
                                goto exit;
                            }
                            if (originalMarker->pointerDepth > 0)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Unary subtraction can not be used on pointers, derefence first", 0);
                                goto exit;
                            }
                            if (originalMarker->type == BOOL || originalMarker->type == CHAR || originalMarker->type == VOID)
                            {
                                SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Unary subtraction can not be used on non-integer types", 0);
                                goto exit;
                            }
                            // Copy the marker
                            AddTypeMarker(&typeMarkers, currentNode, originalMarker->type, originalMarker->pointerDepth, NEITHER);
                            printf("Copied marker from child\n");
                        }
                    }
                    if /*negate*/ (*first == '~')
                    {
                        printf("negate\n");

                        TreeNode *source = (TreeNode *)ListGetIndex(&currentNode->children, 0);
                        TypeMarker *originalMarker = ReadMarkerOnNode(&typeMarkers, source);
                        if (originalMarker != NULL)
                        {
                            if (originalMarker->funcOrVar == FUNCTION)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Cannot negate a function", 0);
                                goto exit;
                            }
                            if (originalMarker->pointerDepth > 0)
                            {
                                SetError(error, POINTERS, SEMANTIC_ANALYZER, "Negate can not be used on pointers, derefence first", 0);
                                goto exit;
                            }
                            if (originalMarker->type == BOOL || originalMarker->type == CHAR || originalMarker->type == VOID)
                            {
                                SetError(error, TYPE_MISMATCH, SEMANTIC_ANALYZER, "Negate can not be used on non-integer types", 0);
                                goto exit;
                            }
                            // Copy the marker
                            AddTypeMarker(&typeMarkers, currentNode, originalMarker->type, originalMarker->pointerDepth, NEITHER);
                            printf("Copied marker from child\n");
                        }
                    }
                }
                if (term == CAST_OR_EXPR)
                {
                    printf("reached cast/expr\n");

                    TreeNode *lastChild = (TreeNode *)PeekList(&currentNode->children);
                    uint8_t *first = (uint8_t *)lastChild->content;

                    if /*Type cast*/ (*first == TYPE_TERM + TERM_OFFSET)
                    {
                        printf("Type cast\n");
                        TreeNode *castType = (TreeNode *)ListGetIndex(&currentNode->children, 3);
                        CopyMarker(&typeMarkers, currentNode, castType);
                    }
                    if /*Parenthesis expression*/ (*first == EXPRESSION + TERM_OFFSET)
                    {
                        printf("Parenthesis expression\n");
                        TreeNode *expression = (TreeNode *)ListGetIndex(&currentNode->children, 1);
                        CopyMarker(&typeMarkers, currentNode, expression);
                    }
                }
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