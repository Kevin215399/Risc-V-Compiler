#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "Common.h"
#include "SyntaxAnalyzer.h"
#include "ASMGenerator.h"
#include "SpecialParser.h"
#define NULL_DEPTH 255

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
        printf("comparing %s to symbol: %s\n", symbol, checkSymbol->name);
        if (strcmp(checkSymbol->name, symbol) == 0)
        {
            printf("Found matching symbol, no error\n");
            return checkSymbol;
        }
    }
    printf("not found\n");
    return NULL;
}
// Determines the current index of a node in relation to its siblings
uint8_t GetIndexFromSiblings(TreeNode *self)
{
    TreeNode *parent = self->parent;
    for (int i = 0; i < parent->children.count; i++)
    {
        TreeNode *checkSibling = (TreeNode *)ListGetIndex(&parent->children, i);
        if (checkSibling == self)
        {
            return i;
        }
    }
    return 0;
}

void BacktrackTypeCheck(GeneralList *mathStack, uint8_t *nextReg, Error *error, GeneralList *typeMarkers, TreeNode *currentNode);
void LeftMath(GeneralList *mathStack, uint8_t *nextReg, Error *error, GeneralList *typeMarkers, TreeNode *currentNode);

bool IsFunctionDecleration(TreeNode *decleration)
{
    TreeNode *decrest = (TreeNode *)ListGetIndex(&decleration->children, 0);
    TreeNode *funcOrVar = (TreeNode *)PeekList(&decrest->children);
    if ((uint8_t)((Token *)funcOrVar->content)->value[0] == FUNC_DEC_REST + TERM_OFFSET)
    {
        return true;
    }
    else
    {
        return false;
    }
}
Identifier *DeclareVariable(Error *error, GeneralList *variableStack, TreeNode *currentNode, bool forceVariable)
{

    TreeNode *typeNode = (TreeNode *)ListGetIndex(&currentNode->children, 4);
    typeNode = (TreeNode *)ListGetIndex(&typeNode->children, 0);

    TreeNode *nameNode = (TreeNode *)ListGetIndex(&currentNode->children, 2);
    nameNode = (TreeNode *)ListGetIndex(&nameNode->children, 0);

    printf("a:\n");
    delay(100);
    if (FindSymbol(variableStack, ((Token *)nameNode->content)->value) != NULL)
    {
        SetError(error, DUPLICATE, SEMANTIC_ANALYZER, "Symbol was already defined earlier", 0);
        return NULL;
    }

    printf("a1:\n");
    delay(100);
    Identifier *newVar = (Identifier *)malloc(sizeof(Identifier));
    if (newVar != NULL)
    {
        printf("created newVar\n");
    }
    else
    {
        printf("failed newVar\n");
    }
    printf("a2");
    delay(100);

    newVar->name = malloc(strlen(((Token *)nameNode->content)->value) + 1);
    printf("a3");
    strcpy(newVar->name, ((Token *)nameNode->content)->value);

    printf("b:\n");
    delay(100);
    newVar->type = ParseType(((Token *)typeNode->content)->value);

    TreeNode *pointerNode = (TreeNode *)ListGetIndex(&currentNode->children, 3);

    newVar->pointerDepth = GetPointerDepth(pointerNode);

    printf("c:\n");
    delay(100);
    if (forceVariable)
    {
        newVar->funcOrVar = VARIABLE;
    }
    else
    {
        if (IsFunctionDecleration(currentNode))
        {
            newVar->funcOrVar = FUNCTION;
            InitializeList(&newVar->parameters);
        }
        else
        {
            newVar->funcOrVar = VARIABLE;
        }
    }
    printf("Pushing:\n");
    delay(100);
    PushList(variableStack, newVar);
    printf("pushed %s to symbol table\n", newVar->name);
    delay(100);
    return newVar;
}
void EvaluateExpression(Error *error, TreeNode *currentNode, GeneralList *variableStack)
{
    GeneralList postfix;
    InitializeList(&postfix);
    GeneralList inputTokens;
    InitializeList(&inputTokens);
    while (currentNode->children.count > 0)
    {
        Token *token = (Token *)((TreeNode *)PopListFirst(&currentNode->children))->content;
        printf("Shunt yard in: %s\n", token->value);
        PushList(&inputTokens, token);
    }
    error = ShuntYard(&inputTokens, variableStack, &postfix);
    if (error->errorCode != NO_ERROR)
    {
        return;
    }
    PostfixToASM(error, &postfix, variableStack);
    if (error->errorCode != NO_ERROR)
    {
        return;
    }
    AppendTree(currentNode, PopList(&postfix));
}
void InitializeStdFunctions(GeneralList *variableStack)
{
    Identifier *stdFunction = NULL;
    Identifier *parameter = NULL;

    // Malloc
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = MALLOC_LABEL;
    stdFunction->name = (char *)malloc(strlen("malloc") + 1);
    strcpy(stdFunction->name, "malloc");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("bytes") + 1);
    strcpy(parameter->name, "bytes");
    parameter->pointerDepth = 0;
    parameter->type = UINT32_T;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 1;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);

    // Free
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = FREE_LABEL;
    stdFunction->name = malloc(strlen("free") + 1);
    strcpy(stdFunction->name, "free");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("pointer") + 1);
    strcpy(parameter->name, "pointer");
    parameter->pointerDepth = 1;
    parameter->type = VOID;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 0;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);

    // pinMode
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = PIN_MODE;
    stdFunction->name = malloc(strlen("pinMode") + 1);
    strcpy(stdFunction->name, "pinMode");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("pin") + 1);
    strcpy(parameter->name, "pin");
    parameter->pointerDepth = 0;
    parameter->type = UINT8_T;
    PushList(&stdFunction->parameters, parameter);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("output") + 1);
    strcpy(parameter->name, "output");
    parameter->pointerDepth = 0;
    parameter->type = BOOL;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 0;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);

    // pinToggle
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = PIN_TOGGLE;
    stdFunction->name = malloc(strlen("pinToggle") + 1);
    strcpy(stdFunction->name, "pinToggle");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("pin") + 1);
    strcpy(parameter->name, "pin");
    parameter->pointerDepth = 0;
    parameter->type = UINT8_T;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 0;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);

    // pinWrite
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = PIN_WRITE;
    stdFunction->name = malloc(strlen("pinWrite") + 1);
    strcpy(stdFunction->name, "pinWrite");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("pin") + 1);
    strcpy(parameter->name, "pin");
    parameter->pointerDepth = 0;
    parameter->type = UINT8_T;
    PushList(&stdFunction->parameters, parameter);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("value") + 1);
    strcpy(parameter->name, "value");
    parameter->pointerDepth = 0;
    parameter->type = BOOL;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 0;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);

    // pinRead
    stdFunction = (Identifier *)malloc(sizeof(Identifier));
    stdFunction->funcOrVar = FUNCTION;
    stdFunction->labelNumber = PIN_READ;
    stdFunction->name = malloc(strlen("pinRead") + 1);
    strcpy(stdFunction->name, "pinRead");
    InitializeList(&stdFunction->parameters);

    parameter = (Identifier *)malloc(sizeof(Identifier));
    parameter->funcOrVar = VARIABLE;
    parameter->name = (char *)malloc(strlen("pin") + 1);
    strcpy(parameter->name, "pin");
    parameter->pointerDepth = 0;
    parameter->type = UINT8_T;
    PushList(&stdFunction->parameters, parameter);

    stdFunction->pointerDepth = 0;
    stdFunction->type = VOID;
    PushList(variableStack, stdFunction);
}
Error *SemanticAnylsis(TreeNode *AST)
{
    printf("semantic analysis\n");
    Error *error = (Error *)malloc(sizeof(Error));
    memset(error, 0, sizeof(Error));
    error->errorCode = NO_ERROR;

    if (AST == NULL)
    {
        SetError(error, UNEXPECTED_NULL, SEMANTIC_ANALYZER, "Missing AST reference", 0);
        goto exit;
    }

    GeneralList exploreMap;
    InitializeList(&exploreMap);

    GeneralList variableStack;
    InitializeList(&variableStack);

    InitializeStdFunctions(&variableStack);

    GeneralList tableIndexices;
    InitializeList(&tableIndexices);

    GeneralList typeMarkers;
    InitializeList(&typeMarkers);

    GeneralList mathStack;
    InitializeList(&mathStack);
    uint8_t nextReg = 0;

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

        delay(60);

        char *value = ((Token *)currentNode->content)->value;
        uint16_t lineNumber = ((Token *)currentNode->content)->lineNumber;
        if (value == NULL || value[0] == '\0')
        {
            // Reached epsilon, backtrack
            // Seems odd, probably due to a bad CFG
            printf("epsilon value, backtracking. May need to fix CFG\n");
            goto backtrack;
        }

        /*if (*((uint8_t *)value) < TERM_OFFSET)
        {
            printf("%s\n", value);
            if (value[0] == '{')
            {
                // Open a new context;
                printf("opened a context\n");
                PushByte(&tableIndexices, variableStack.count);
                ScopeOpenContext();
            }
            if (value[0] == '}')
            {
                // Close a context;
                printf("closed a context\n");
                if (tableIndexices.count == 0)
                {
                    SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "Missing matching '{'", lineNumber);
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
                CloseScopeContext(false);
            }
        }*/

        /*


        Decide if declerations or brackets should open scopes


        */
        if (*((uint8_t *)value) == TERM_OFFSET + DECLERATION && IsFunctionDecleration(currentNode))
        {
            printf("declaring function\n");
            Identifier *function = DeclareVariable(error, &variableStack, currentNode, false);
            if (function == NULL)
            {
                goto exit;
            }
            ScopeOpenContext();
            PushByte(&tableIndexices, variableStack.count);
            ASM_Label(currentLabelNum);
            UpdateStackList("PCount", PARAMETER);
            for (int i = 0; i < function->parameters.count; i++)
            {
                Identifier *parameter = (Identifier *)ListGetIndex(&function->parameters, i);
                UpdateStackList(parameter->name, PARAMETER);
            }
            UpdateStackList("T0", REG_STATE);
            UpdateStackList("T1", REG_STATE);
            UpdateStackList("T2", REG_STATE);
            UpdateStackList("T3", REG_STATE);
            UpdateStackList("T4", REG_STATE);
            UpdateStackList("T5", REG_STATE);
            UpdateStackList("T6", REG_STATE);

            UpdateStackList("RETURN", PARAMETER);
            function->labelNumber = currentLabelNum;
            currentLabelNum++;

            printf("Function declared\n");
            delay(100);
        }

        if (*((uint8_t *)value) == TERM_OFFSET + ASSIGN)
        {
            printf("Assign Or Call\n");
            TreeNode *identifierNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){2, 0}, 2);
            if (identifierNode == NULL)
            {
                SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol does not exist in the current scope", lineNumber);
                goto exit;
            }
            printf("Want to find symbol: %s\n", ((Token *)identifierNode->content)->value);
            Identifier *identifier = FindSymbol(&variableStack, ((Token *)identifierNode->content)->value);
            if (identifier == NULL)
            {
                SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol does not exist in the current scope", lineNumber);
                goto exit;
            }

            TreeNode *callOrAssignNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0}, 2);
            Token *callOrAssign = (Token *)callOrAssignNode->content;
            if (*((uint8_t *)callOrAssign->value) != CALL + TERM_OFFSET)
            {
                printf("Is assign, add assignment operator to expression\n");
                TreeNode *expressionNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0, 0}, 3);
                TreeNode *assignmentNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0, 1}, 3);
                char *assignmentOperator = ((Token *)assignmentNode->content)->value;
                if (strlen(assignmentOperator) > 1)
                {
                    printf("Assign has operator\n");
                    Token *newToken = malloc(sizeof(Token));
                    newToken->lineNumber = lineNumber;
                    newToken->type = OPERATOR;
                    newToken->value = malloc(2);
                    newToken->value[0] = assignmentOperator[0];
                    newToken->value[1] = '\0';
                    AppendTree(expressionNode, newToken);

                    newToken = malloc(sizeof(Token));
                    newToken->lineNumber = lineNumber;
                    newToken->type = IDENTIFIER;
                    newToken->value = malloc(strlen(identifier->name) + 1);
                    strcpy(newToken->value, identifier->name);
                    AppendTree(expressionNode, newToken);
                }
                else
                {
                    printf("Assign has no operator\n");
                }
            }
        }
        if (*((uint8_t *)value) == TERM_OFFSET + RETURN)
        {
            printf("closing function\n");
            // TODO: pass expression onto stack
            TreeNode *expressionNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0}, 2);
            if (expressionNode != NULL)
            {
                if (((Token *)expressionNode->content)->type == TERM_OFFSET + EXPRESSION)
                {
                    printf("return has expression\n");
                    delay(100);
                    EvaluateExpression(error, expressionNode, &variableStack);
                    if (error->errorCode != NO_ERROR)
                    {
                        goto exit;
                    }
                    TreeNode *stackNode = TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0, 0}, 3);
                    Token *tokenPointer = (Token *)stackNode->content;
                    uint16_t stackPointerIndex = 0;
                    sscanf(tokenPointer->value, "%hu", &stackPointerIndex);
                    uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
                    ASM_StackLoad(A0, offset * 4);
                }
            }

            ASM_JumpString("functionReturn");
            CloseScopeContext(true);
            goto backtrack;
        }
        if (*((uint8_t *)value) == TERM_OFFSET + IF)
        {
            printf("If statement\n");

            TreeNode *expressionNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){5}, 1);
            EvaluateExpression(error, expressionNode, &variableStack);
            if (error->errorCode != NO_ERROR)
            {
                goto exit;
            }

            printf("Reserve label: %d\n", currentLabelNum);
            uint32_t label = currentLabelNum++;

            Token *tokenPointer = (Token *)((TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){5, 0}, 2))->content;
            uint16_t stackPointerIndex = 0;
            sscanf(tokenPointer->value, "%hu", &stackPointerIndex);
            uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
            ASM_StackLoad(T1, offset * 4);
            ASM_Branch("beq", T1, X0, label);

            ScopeOpenContext();

            uint32_t *labelChild = (uint32_t *)malloc(sizeof(uint32_t));
            *labelChild = label;
            AppendTree(currentNode, labelChild);

            currentNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){4}, 1);
            PushByte(&exploreMap, 4);
            continue;
        }
        if (*((uint8_t *)value) == TERM_OFFSET + WHILE)
        {
            ScopeOpenContext();
            ScopeOpenContext();
            printf("while\n");

            uint32_t toplabel = currentLabelNum++;
            ASM_Label(toplabel);

            TreeNode *expressionNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){4}, 1);
            EvaluateExpression(error, expressionNode, &variableStack);
            if (error->errorCode != NO_ERROR)
            {
                goto exit;
            }

            uint32_t endLabel = currentLabelNum++;

            Token *tokenPointer = (Token *)((TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){4, 0}, 2))->content;
            uint16_t stackPointerIndex = 0;
            sscanf(tokenPointer->value, "%hu", &stackPointerIndex);
            uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
            ASM_StackLoad(T1, offset * 4);
            CloseScopeContext(false);
            ASM_Branch("beq", T1, X0, endLabel);

            uint32_t *labelChild = (uint32_t *)malloc(sizeof(uint32_t));
            *labelChild = endLabel;
            AppendTree(currentNode, labelChild);

            labelChild = (uint32_t *)malloc(sizeof(uint32_t));
            *labelChild = toplabel;
            AppendTree(currentNode, labelChild);

            currentNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){3}, 1);
            PushByte(&exploreMap, 4);
            continue;
        }
        if (*((uint8_t *)value) == TERM_OFFSET + FUNCTION_PARAMS)
        {
            if (currentNode->children.count == 5)
            {
                // find parent to add to their parameters
                TreeNode *parent = currentNode;
                while (currentNode->parent != NULL)
                {
                    parent = parent->parent;
                }
                if (parent == NULL)
                {
                    SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "How did you do this? This parameter has no parent function?!", lineNumber);
                    goto exit;
                }
                Identifier *parentFunciton = FindSymbol(&variableStack, ((Token *)parent->content)->value);
                if (parentFunciton == NULL)
                {
                    SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "How did you do this? This parameter has no parent function?!", lineNumber);
                    goto exit;
                }
                printf("Found parent function, pushing param\n");
                Identifier *variable = DeclareVariable(error, &variableStack, currentNode, true);
                if (variable == NULL)
                {
                    goto exit;
                }
                PushList(&parentFunciton->parameters, variable);
            }
        }
        if (*((uint8_t *)value) == TERM_OFFSET + EXPRESSION)
        {
            EvaluateExpression(error, currentNode, &variableStack);
            if (error->errorCode != NO_ERROR)
            {
                goto exit;
            }
        }

        // LeftMath(&mathStack, &nextReg, error, &typeMarkers, currentNode);

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
                printf("exploring UP\n");

                if (exploreMap.count == 0)
                {
                    // Reached root node
                    printf("tree root found, exitting\n");
                    goto success;
                }
                topValue = (uint8_t *)PeekList(&exploreMap);

                char *value = ((Token *)currentNode->content)->value;
                if (*((uint8_t *)value) == TERM_OFFSET + DECLERATION && !IsFunctionDecleration(currentNode))
                {
                    printf("Backtracked to variable\n");
                    Identifier *variable = DeclareVariable(error, &variableStack, currentNode, false);
                    if (variable == NULL)
                    {
                        goto exit;
                    }
                    TreeNode *expression = TreeGetNodeConstant(currentNode, (uint8_t[]){0, 0, 1}, 3);
                    if (expression == NULL)
                    {
                        printf("assignment is null\n");
                        ASM_StackPush(X0);
                    }
                    else
                    {
                        printf("assignment val is not null\n");
                        if (expression->children.count != 1)
                        {
                            SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Assignment expression has not been simplified", lineNumber);
                            goto exit;
                        }
                        Token *token = (Token *)(((TreeNode *)PeekList(&expression->children))->content);
                        printf("Type: %d\n", token->type);
                        printf("Line: %d\n", token->lineNumber);
                        if (token->type != STACK_POINTER_TOKEN)
                        {
                            SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Assignment expression has not been simplified", lineNumber);
                            goto exit;
                        }

                        uint16_t stackPointerIndex = 0;
                        sscanf(token->value, "%hu", &stackPointerIndex);
                        uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
                        ASM_StackLoad(T1, offset * 4);

                        ASM_StackPush(T1);
                    }
                    UpdateStackList(variable->name, STACK_VAR);
                }
                if (*((uint8_t *)value) == TERM_OFFSET + IF)
                {
                    TreeNode *labelNode = (TreeNode *)PopList(&currentNode->children);
                    uint32_t *label = (uint32_t *)labelNode->content;
                    printf("Retrieved reserved label: %d\n", *label);
                    CloseScopeContext(false);
                    ASM_Label(*label);
                    free(label);
                }
                if (*((uint8_t *)value) == TERM_OFFSET + WHILE)
                {
                    TreeNode *topLabelNode = (TreeNode *)PopList(&currentNode->children);
                    TreeNode *endLabelNode = (TreeNode *)PopList(&currentNode->children);
                    uint32_t *topLabel = (uint32_t *)topLabelNode->content;
                    uint32_t *endLabel = (uint32_t *)endLabelNode->content;
                    CloseScopeContext(false);
                    ASM_Jump(*topLabel);
                    ASM_Label(*endLabel);
                    free(topLabel);
                    free(endLabel);
                }
                if (*((uint8_t *)value) == TERM_OFFSET + ASSIGN)
                {
                    printf("Assign Or Call\n");
                    TreeNode *identifierNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){2, 0}, 2);
                    if (identifierNode == NULL)
                    {
                        SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol does not exist in the current scope", lineNumber);
                        goto exit;
                    }
                    printf("Want to find symbol: %s\n", ((Token *)identifierNode->content)->value);
                    Identifier *identifier = FindSymbol(&variableStack, ((Token *)identifierNode->content)->value);
                    if (identifier == NULL)
                    {
                        SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Symbol does not exist in the current scope", lineNumber);
                        goto exit;
                    }

                    TreeNode *callOrAssignNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0}, 2);
                    Token *callOrAssign = (Token *)callOrAssignNode->content;
                    if (*((uint8_t *)callOrAssign->value) == CALL + TERM_OFFSET)
                    {
                        printf("Is call\n");
                        if (identifier->funcOrVar != FUNCTION)
                        {
                            SetError(error, SYNTAX_ERROR, SEMANTIC_ANALYZER, "Function calls can only be used on functions", lineNumber);
                            goto exit;
                        }
                        printf("Current stack list\n");
                        for (int x = 0; x < scopes->count; x++)
                        {
                            GeneralList *scope = (GeneralList *)ListGetIndex(scopes, x);
                            for (int i = 0; i < scope->count; i++)
                            {
                                StackPointer *sp = (StackPointer *)ListGetIndex(scope, i);
                                printf("%s\n", sp->name);
                            }
                            printf("scope end");
                        }
                        TreeNode *parameterNode = (TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0, 1}, 3);
                        GeneralList parameters;
                        InitializeList(&parameters);
                        while (parameterNode->children.count > 1)
                        {
                            TreeNode *expression = (TreeNode *)TreeGetNodeConstant(parameterNode, (uint8_t[]){1, 0}, 2);
                            printf("Found a parameter\n");
                            delay(100);
                            PushList(&parameters, expression);
                            parameterNode = (TreeNode *)PeekListFirst(&parameterNode->children);
                        }
                        printf("All parameters found");
                        delay(100);
                        ASM_Imm(S0, parameters.count * 4 + 4);
                        ASM_Move(S0, A2);
                        ASM_JumpStringAndLink(S2, "push");
                        for (int i = 0; i < parameters.count; i++)
                        {
                            Token *token = (Token *)((TreeNode *)ListGetIndex(&parameters, i))->content;
                            LoadParameter(error, token, S0, &variableStack);
                            ASM_JumpStringAndLink(S2, "push");
                        }

                        CallFunction(identifier);
                    }
                    else
                    {
                        printf("Is assign\n");
                        Token *tokenPointer = (Token *)((TreeNode *)TreeGetNodeConstant(currentNode, (uint8_t[]){1, 0, 0, 0}, 4))->content;
                        uint16_t stackPointerIndex = 0;
                        sscanf(tokenPointer->value, "%hu", &stackPointerIndex);
                        uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
                        ASM_StackLoad(T1, offset * 4);
                        StackPointer *variableLocation = NULL;
                        bool isGlobalScope = false;
                        for (int i = 0; i < scopes->count; i++)
                        {
                            GeneralList *scope = (GeneralList *)ListGetIndex(scopes, i);
                            for (int x = 0; x < scope->count; x++)
                            {
                                StackPointer *stackPointer = (StackPointer *)ListGetIndex(scope, x);
                                if (strcmp(stackPointer->name, identifier->name) == 0)
                                {
                                    if (i == 0)
                                    {
                                        isGlobalScope = true;
                                    }
                                    variableLocation = stackPointer;
                                    break;
                                }
                            }
                        }
                        if (variableLocation == NULL)
                        {
                            SetError(error, UNDEFINED, SEMANTIC_ANALYZER, "Did not find variable on the stack", lineNumber);
                            goto exit;
                        }
                        if (isGlobalScope)
                            ASM_StackReplaceGlobal(T1, variableLocation->offset * 4);
                        else
                            ASM_StackReplace(T1, variableLocation->offset * 4);
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
success:
    for (int i = 0; i < variableStack.count; i++)
    {
        Identifier *variable = (Identifier *)ListGetIndex(&variableStack, i);
        if (strcmp(variable->name, "main") == 0)
        {
            ASM_Imm(S0, variable->parameters.count * 4 + 4);
            ASM_Move(S0, A2);
            ASM_JumpStringAndLink(S2, "push");

            ASM_LoadAddress(A1, variable->labelNumber);
            ASM_JumpStringAndLink(A7, "callFunction");
        }
    }
    return error;
exit:
    // Need to add list deinitialization
    return error;
}

#endif