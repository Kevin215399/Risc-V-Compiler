#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "Common.h"
#include "SemanticAnalyzer.h"
#include "Common/Keywords.h"
#include "ASMTemplate.h"

GeneralList *asmGlobalList;
GeneralList *asmFunctionList;

GeneralList *scopes;

uint32_t currentLabelNum;

void PushTargetASMList(char *value)
{
    if (scopes->count > 1)
    {
        PushList(asmFunctionList, value);
    }
    else
    {
        PushList(asmGlobalList, value);
    }
}
void PopTargetASMList()
{
    if (scopes->count > 1)
    {
        free(PopList(asmFunctionList));
    }
    else
    {
        free(PopList(asmGlobalList));
    }
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

void ASM_Comment(char *message)
{
    char *cstr = malloc(1 + strlen(message) + 1);
    sprintf(cstr, "#%s", message);
    PushTargetASMList(cstr);
    printf("asm comment done");
}
void ASM_Math(char *operation, Registers inA, Registers inB, Registers out)
{
    char *cstr = malloc(strlen(operation) + 1 + 2 + 1 + 2 + 1 + 2 + 1);
    sprintf(cstr, "%s %s,%s,%s", operation, registerNames[(uint8_t)out], registerNames[(uint8_t)inA], registerNames[(uint8_t)inB]);
    PushTargetASMList(cstr);
    printf("asm math done\n");
}
void ASM_MathI(char *operation, Registers destination, Registers source, int value)
{
    char *cstr = malloc(strlen(operation) + 1 + 2 + 1 + 2 + 1 + IntLength(value) + 1);
    sprintf(cstr, "%s %s,%s,%d", operation, registerNames[(uint8_t)destination], registerNames[(uint8_t)source], value);
    PushTargetASMList(cstr);
    printf("ASM_MathI done\n");
}
void ASM_Move(Registers source, Registers destination)
{
    char *cstr = malloc(strlen("mv") + 1 + 2 + 1 + 2 + 1);
    sprintf(cstr, "mv %s,%s", registerNames[(uint8_t)destination], registerNames[(uint8_t)source]);
    PushTargetASMList(cstr);
    printf("asm move done\n");
}
void ASM_Imm(Registers destination, uint32_t value)
{
    char *cstr = malloc(strlen("li") + 1 + 2 + 1 + UintLength(value) + 1);
    sprintf(cstr, "li %s,%u", registerNames[(uint8_t)destination], value);
    PushTargetASMList(cstr);
    printf("ASM_Imm done\n");
}
// Takes the location in bytes (not 4 byte blocks)
void ASM_StackLoad(Registers destination, uint16_t frameOffset)
{
    if (frameOffset < 2000)
    {
        char *cstr = malloc(strlen("lw") + 1 + 2 + 1 + IntLength(frameOffset) + 4 + 1);
        sprintf(cstr, "lw %s,%d(t0)", registerNames[(uint8_t)destination], frameOffset);
        PushTargetASMList(cstr);
        printf("asm load small done\n");
        return;
    }
    ASM_Imm(T4, frameOffset);
    ASM_Math("add", T4, T0, T3);
    char *cstr = malloc(strlen("lw") + 1 + 2 + 1 + 2 + 4 + 1);
    sprintf(cstr, "lw %s,0(t3)", registerNames[(uint8_t)destination]);
    PushTargetASMList(cstr);
    printf("asm load big done\n");
}
// Takes the location in bytes (not 4 byte blocks)
void ASM_StackLoadGlobal(Registers destination, uint16_t frameOffset)
{
    ASM_Imm(T3, STACK_ADDR);
    ASM_Imm(T4, frameOffset);
    ASM_Math("add", T3, T4, T3);
    ASM_Math("add", T3, GP, T3);
    char *cstr = malloc(strlen("lw") + 1 + 2 + 1 + 2 + 4 + 1);
    sprintf(cstr, "lw %s,0(t3)", registerNames[(uint8_t)destination]);
    PushTargetASMList(cstr);
    printf("asm load big done\n");
}
// Takes the location in bytes (not 4 byte blocks)
void ASM_StackReplace(Registers source, uint16_t frameOffset)
{
    if (frameOffset < 2000)
    {
        char *cstr = malloc(strlen("sw") + 1 + 2 + 1 + IntLength(frameOffset) + 4 + 1);
        sprintf(cstr, "sw %s,%d(t0)", registerNames[(uint8_t)source], frameOffset);
        ASM_Comment("StackReplace");
        PushTargetASMList(cstr);
        printf("asm replace small done\n");
        return;
    }
    ASM_Imm(T4, frameOffset);
    ASM_Math("add", T4, T0, T3);
    char *cstr = malloc(strlen("sw") + 1 + 2 + 1 + 2 + 4 + 1);
    sprintf(cstr, "sw %s,0(t3)", registerNames[(uint8_t)source]);
    PushTargetASMList(cstr);
    printf("asm replace big done\n");
}
// Takes the location in bytes (not 4 byte blocks)
void ASM_StackReplaceGlobal(Registers source, uint16_t frameOffset)
{

    ASM_Imm(T3, STACK_ADDR);
    ASM_Imm(T4, frameOffset);
    ASM_Math("add", T3, T4, T3);
    ASM_Math("add", T3, GP, T3);

    char *cstr = malloc(strlen("sw") + 1 + 2 + 1 + 2 + 4 + 1);
    sprintf(cstr, "sw %s,0(t3)", registerNames[(uint8_t)source]);
    PushTargetASMList(cstr);
    printf("asm replace big done\n");
}
// Takes the location in bytes (not 4 byte blocks)
void ASM_LoadFromRegister(Registers destination, Registers location)
{
    char *cstr = malloc(strlen("lw") + 1 + 2 + 1 + 2 + 4 + 1);
    sprintf(cstr, "lw %s,0(%s)", registerNames[(uint8_t)destination], registerNames[(uint8_t)location]);
    PushTargetASMList(cstr);
    printf("asm load from reg loc done\n");
}
void ASM_StackPush(Registers valueReg)
{
    char *cstr = malloc(strlen("sw") + 1 + 2 + 1 + 5 + 1);
    sprintf(cstr, "sw %s,0(sp)", registerNames[(uint8_t)valueReg]);
    PushTargetASMList(cstr);
    ASM_MathI("addi", SP, SP, 4);

    printf("asm push done\n");
}
void ASM_Branch(char *branchType, Registers compareA, Registers compareB, uint32_t label)
{
    char *cstr = malloc(strlen(branchType) + 1 + 2 + 1 + 2 + 1 + 1 + UintLength(label) + 1);
    sprintf(cstr, "%s %s,%s,L%d", branchType, registerNames[(uint8_t)compareA], registerNames[(uint8_t)compareB], label);
    PushTargetASMList(cstr);
    printf("asm branch done\n");
}
void ASM_Label(uint32_t label)
{
    char *cstr = malloc(1 + UintLength(label) + 2);
    sprintf(cstr, "L%d:", label);
    PushTargetASMList(cstr);
    printf("asm label done\n");
}
void ASM_Jump(uint32_t label)
{
    char *cstr = malloc(2 + 1 + UintLength(label) + 1);
    sprintf(cstr, "j L%d", label);
    PushTargetASMList(cstr);
    printf("asm jump done\n");
}
void ASM_JumpString(char *label)
{
    char *cstr = malloc(2 + strlen(label) + 1);
    sprintf(cstr, "j %s", label);
    PushTargetASMList(cstr);
    printf("asm jump done\n");
}
void ASM_LoadAddress(Registers targetRegister, uint32_t label)
{
    char *cstr = malloc(2 + 1 + 2 + 1 + 1 + UintLength(label) + 1);
    sprintf(cstr, "la %s,L%d", registerNames[(uint8_t)targetRegister], label);
    PushTargetASMList(cstr);
    printf("asm LA done\n");
}
void ASM_LoadAddressString(Registers targetRegister, char *label)
{
    char *cstr = malloc(2 + 1 + 2 + 1 + strlen(label) + 1);
    sprintf(cstr, "la %s,%s", registerNames[(uint8_t)targetRegister], label);
    PushTargetASMList(cstr);
    printf("asm LA str done\n");
}
void ASM_JumpAndLink(Registers targetRegister, uint32_t label)
{
    char *cstr = malloc(3 + 1 + 2 + 1 + 1 + UintLength(label) + 1);
    sprintf(cstr, "jal %s,L%d", registerNames[(uint8_t)targetRegister], label);
    PushTargetASMList(cstr);
    printf("asm JAL done\n");
}
void ASM_JumpStringAndLink(Registers targetRegister, char *label)
{
    char *cstr = malloc(3 + 1 + 2 + 1 + strlen(label) + 1);
    sprintf(cstr, "jal %s,%s", registerNames[(uint8_t)targetRegister], label);
    PushTargetASMList(cstr);
    printf("asm JSAL done\n");
}
void CallFunction(Identifier *function)
{
    switch (function->labelNumber)
    {
    case PIN_MODE:
        ASM_LoadAddressString(A1, "pinMode");
        ASM_JumpStringAndLink(A7, "callFunction");
        break;
    case PIN_TOGGLE:
        ASM_LoadAddressString(A1, "pinToggle");
        ASM_JumpStringAndLink(A7, "callFunction");
        break;
    case PIN_WRITE:
        ASM_LoadAddressString(A1, "pinWrite");
        ASM_JumpStringAndLink(A7, "callFunction");
        break;
    case PIN_READ:
        ASM_LoadAddressString(A1, "pinRead");
        ASM_JumpStringAndLink(A7, "callFunction");
        break;
    default:
        ASM_LoadAddress(A1, function->labelNumber);
        ASM_JumpStringAndLink(A7, "callFunction");
        break;
    }
    return;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

void PrintASMProgram()
{
    printf("%s\n", picoAsmHeader);
    for (int i = 0; i < asmGlobalList->count; i++)
    {
        char *value = (char *)ListGetIndex(asmGlobalList, i);
        printf("%s\n", value);
    }
    printf("j EXIT\n");
    printf("\n");

    for (int i = 0; i < asmFunctionList->count; i++)
    {
        char *value = (char *)ListGetIndex(asmFunctionList, i);
        printf("%s\n", value);
    }
    printf("%s\n", stdFunctions);
}

void ScopeOpenContext()
{
    printf("Open context %d\n", scopes->count);
    delay(100);
    GeneralList *newScope = (GeneralList *)malloc(sizeof(GeneralList));
    InitializeList(newScope);
    PushList(scopes, newScope);
}
void CloseScopeContext(bool closingFunction)
{
    printf("Close context %d\n", scopes->count);
    delay(100);
    printf("Closing scope\n");
    if (scopes->count <= 1)
    {
        printf("BAD NO CLOSE GLOBAL SCOPE!!");
    }
    GeneralList *lastScope = (GeneralList *)PeekList(scopes);

    if (!closingFunction && lastScope->count != 0)
    {
        ASM_Imm(T6, lastScope->count * 4);
        ASM_Math("sub", SP, T6, SP);
    }
    PopList(scopes);

    while (lastScope->count > 0)
    {
        free(PopList(lastScope));
    }
    free(lastScope);
}
void UpdateStackList(char *name, StackType type)
{
    // Multiply by 4
    uint32_t newLocation = 0;
    if (scopes->count == 1)
    {
        GeneralList *scope = (GeneralList *)PeekListFirst(scopes);
        newLocation += scope->count;
    }
    else
    {
        for (int x = 1; x < scopes->count; x++)
        {
            GeneralList *scope = (GeneralList *)ListGetIndex(scopes, x);
            newLocation += scope->count;
        }
    }
    StackPointer *newPointer = malloc(sizeof(StackPointer));
    newPointer->name = malloc(strlen(name) + 1);
    strcpy(newPointer->name, name);
    newPointer->offset = newLocation;
    newPointer->type = type;
    PushList((GeneralList *)PeekList(scopes), newPointer);
    printf("New stack var: %s\n", newPointer->name);
}

void SetASMDestination()
{
    asmFunctionList = malloc(sizeof(GeneralList));
    InitializeList(asmFunctionList);
    asmGlobalList = malloc(sizeof(GeneralList));
    InitializeList(asmGlobalList);
    scopes = malloc(sizeof(GeneralList));
    InitializeList(scopes);

    currentLabelNum = 0;
    ScopeOpenContext();
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

StackPointer *LoadParameter(Error *error, Token *parameter, Registers destinationReg, GeneralList *variableStack)
{
    if (parameter->type == IDENTIFIER)
    {
        printf("param is identifier\n");
        /*Identifier *identifier = NULL;
        for (int i = 0; i < variableStack->count; i++)
        {
            Identifier *checkIdentifier = (Identifier *)ListGetIndex(variableStack, i);
            if (strcmp(checkIdentifier->name, parameter->value) == 0)
            {
                identifier = checkIdentifier;
                break;
            }
        }
        if (identifier->funcOrVar == FUNCTION)
        {
            printf(" Is function\n");
            for (int i = 0; i < identifier->parameters.count; i++)
            {
            }
            ASM_LoadAddress(A1, identifier->labelNumber);
            ASM_JumpStringAndLink(A7, "callFunction");
            ASM_Move(A0, destinationReg);
        }
        else
        {*/
        printf(" Is variable: %s\n", parameter->value);
        delay(100);
        StackPointer *stackPointer = NULL;
        bool notFound = true;
        bool isGlobalScope = false;
        for (int x = 0; x < scopes->count; x++)
        {
            GeneralList *scope = ((GeneralList *)ListGetIndex(scopes, x));
            for (int i = 0; i < scope->count; i++)
            {
                StackPointer *checkPointer = ListGetIndex(scope, i);
                printf("checking scope: %s\n", checkPointer->name);
                if (strcmp(checkPointer->name, parameter->value) == 0)
                {
                    stackPointer = checkPointer;
                    notFound = false;
                    if (x == 0)
                    {
                        isGlobalScope = true;
                    }
                    printf("found at stack loc: %d\n", stackPointer->offset);
                    break;
                }
            }
        }
        if (notFound)
        {
            printf("not found\n");
            delay(100);

            SetError(error, SYNTAX_ERROR, ASM_GENERATION, "Symbol does not exist in current scope", parameter->lineNumber);
            return NULL;
        }
        if (isGlobalScope)
            ASM_StackLoadGlobal(destinationReg, stackPointer->offset * 4);
        else
            ASM_StackLoad(destinationReg, stackPointer->offset * 4);
        return stackPointer;
        //}
    }
    else if (parameter->type == STACK_POINTER_TOKEN)
    {
        printf("param is STACK_POINTER_TOKEN\n");
        delay(100);
        uint16_t stackPointerIndex = 0;
        sscanf(parameter->value, "%hu", &stackPointerIndex);
        uint32_t offset = ((StackPointer *)ListGetIndex((GeneralList *)PeekList(scopes), stackPointerIndex))->offset;
        ASM_StackLoad(destinationReg, offset * 4);
        return NULL;
    }
    else
    {
        printf("param is literal\n");
        delay(100);
        IdentifierType type = TypeOfString(parameter->value);
        if (type == BOOL)
        {
            printf("bool lit\n");
            if (strcmp(parameter->value, "false") == 0)
                ASM_MathI("addi", destinationReg, X0, 0);
            else
                ASM_MathI("addi", destinationReg, X0, 1);
        }
        else if (type <= UINT32_T)
        {
            printf("uint lit\n");
            uint32_t value = 0;
            uint8_t matches = sscanf(parameter->value, "%u", &value);
            if (matches == 0)
            {
                SetError(error, SYNTAX_ERROR, ASM_GENERATION, "uint32_t does not match format", parameter->lineNumber);
            }
            ASM_Imm(destinationReg, value);
        }
        else if (type <= INT)
        {
            printf("int lit\n");
            int value = 0;
            uint8_t matches = sscanf(parameter->value, "%d", &value);
            if (matches == 0)
            {
                SetError(error, SYNTAX_ERROR, ASM_GENERATION, "uint32_t does not match format", parameter->lineNumber);
            }
            ASM_Imm(destinationReg, value);
        }
        else if (type == CHAR)
        {
            printf("str lit\n");

            if (strlen(parameter->value) < 2)
            {
                char value = parameter->value[0];
                ASM_Imm(destinationReg, (uint32_t)value);
                return NULL;
            }
            if (parameter->value[0] != '\"' || parameter->value[strlen(parameter->value - 2)] != '\"')
            {
                printf("str format error\n");

                SetError(error, SYNTAX_ERROR, ASM_GENERATION, "string does not match format", parameter->lineNumber);
                return NULL;
            }
            printf("Scopes: %d\n", scopes->count);
            uint16_t litLoc = ((GeneralList *)PeekList(scopes))->count;
            for (int i = 1; i < strlen(parameter->value) - 1; i++)
            {
                printf("%d\n", i);

                ASM_Imm(destinationReg, parameter->value[i]);
                ASM_StackPush(destinationReg);
                UpdateStackList("STRLIT", STR_LITERAL);
            }
            ASM_StackPush(X0);
            UpdateStackList("STRLIT", STR_LITERAL);
            ASM_Imm(destinationReg, (uint32_t)litLoc);
        }
        return NULL;
    }
    printf("Parameter is none!\n");
    SetError(error, UNEXPECTED_NULL, ASM_GENERATION, "Parameter is invalid", parameter->lineNumber);
    return NULL;
}
void PostfixToASM(Error *error, GeneralList *postfix, GeneralList *variableStack)
{
    uint16_t lineNumber = ((Token *)PeekList(postfix))->lineNumber;
    uint16_t index = 0;
    printf("Special parser postfix out:\n");
    for (int i = 0; i < postfix->count; i++)
    {
        Token *printToken = (Token *)ListGetIndex(postfix, i);
        printf("%s", printToken->value);
    }
    printf("\n");
    if (postfix->count == 1)
    {
        LoadParameter(error, (Token *)PopList(postfix), T1, variableStack);
        ASM_StackPush(T1);
        UpdateStackList("TEMP", STACK_POINTER);

        Token *newToken = malloc(sizeof(Token));
        newToken->type = STACK_POINTER_TOKEN;
        newToken->lineNumber = lineNumber;

        uint16_t stackIndex = ((GeneralList *)PeekList(scopes))->count - 1;
        newToken->value = malloc(IntLength(stackIndex) + 1);
        sprintf(newToken->value, "%hu", stackIndex);
        PushList(postfix, newToken);
    }
    else
        while (true)
        {
            if (index >= postfix->count)
            {
                printf("break, %d, %d\n", index, postfix->count);
                break;
            }
            Token *currentToken = (Token *)ListGetIndex(postfix, index);
            printf("current: %s\n", currentToken->value);
            delay(60);
            if (currentToken->type == IDENTIFIER)
            {
                printf("Function found\n");
                Identifier *function = NULL;
                for (int i = 0; i < variableStack->count; i++)
                {
                    Identifier *checkIdentifier = (Identifier *)ListGetIndex(variableStack, i);
                    if (strcmp(checkIdentifier->name, currentToken->value) == 0)
                    {
                        function = checkIdentifier;
                        break;
                    }
                }
                if (function->funcOrVar != FUNCTION)
                {
                    index++;
                    printf("Not a function, skipping\n");
                    continue;
                }
                ASM_Imm(S0, function->parameters.count * 4 + 4);
                ASM_Move(S0, A2);
                ASM_JumpStringAndLink(S2, "push");
                for (int i = 0; i < function->parameters.count; i++)
                {
                    Token *parameter = (Token *)ListGetIndex(postfix, index - function->parameters.count + i);
                    if (parameter == NULL)
                    {
                        printf("missing param\n");
                        SetError(error, UNEXPECTED_NULL, ASM_GENERATION, "Function missing parameter", lineNumber);
                        goto exit;
                    }
                    printf("loading param %d...\n", i);
                    StackPointer *stackPointer = LoadParameter(error, parameter, S0, variableStack);
                    printf("loaded\n");
                    delay(100);
                    if (error->errorCode != NO_ERROR)
                    {
                        printf("load err\n");
                        delay(100);
                        error->line = currentToken->lineNumber;
                        goto exit;
                    }
                    ASM_JumpStringAndLink(S2, "push");
                }
                printf("calling\n");
                delay(100);
                CallFunction(function);
                printf("function called\n");
                delay(100);

                for (int i = 0; i < function->parameters.count + 1; i++)
                {
                    printf("deleting parameter %d/%d, at index %s\n", i, function->parameters.count + 1, index - function->parameters.count);
                    delay(100);
                    free(DeleteListElement(postfix, index - function->parameters.count));
                }
                ASM_StackPush(A0);
                UpdateStackList("RET_VAL", STACK_POINTER);

                Token *newToken = malloc(sizeof(Token));
                newToken->type = STACK_POINTER_TOKEN;
                newToken->lineNumber = currentToken->lineNumber;

                uint16_t stackIndex = ((GeneralList *)PeekList(scopes))->count - 1;
                newToken->value = malloc(IntLength(stackIndex) + 1);
                sprintf(newToken->value, "%hu", stackIndex);
                printf("insert to : %d\n", index - function->parameters.count);
                ListInsert(postfix, index - function->parameters.count, newToken);
                printf("set index to 0\n");
                index = 0;
                continue;
            }
            else if (currentToken->type == OPERATOR)
            {
                printf("Operator found\n");
                OperatorPrecedence *operator= NULL;
                for (int i = 0; i < OPERATOR_PRECEDENT_COUNT; i++)
                {
                    if (strcmp(OPERATOR_PRECEDENT_LIST[i].operator, currentToken->value) == 0)
                    {
                        operator= & OPERATOR_PRECEDENT_LIST[i];
                        break;
                    }
                }
                if (operator== NULL)
                {
                    SetError(error, SYNTAX_ERROR, ASM_GENERATION, "Operator not recognized", lineNumber);
                    goto exit;
                }

                Token *parameters[5];
                StackPointer *param1StackLoc = NULL;
                printf("getting params\n");
                for (int i = 0; i < operator->parameters; i++)
                {
                    parameters[i] = (Token *)ListGetIndex(postfix, index - operator->parameters + i);
                    if (parameters[i] == NULL)
                    {
                        printf("missing param\n");
                        SetError(error, UNEXPECTED_NULL, ASM_GENERATION, "Operator missing parameter", lineNumber);
                        goto exit;
                    }
                    StackPointer *stackPointer = LoadParameter(error, parameters[i], T1 + i, variableStack);
                    if (error->errorCode != NO_ERROR)
                    {
                        error->line = currentToken->lineNumber;
                        goto exit;
                    }
                    if (i == 0)
                    {
                        param1StackLoc = stackPointer;
                    }
                }

                if (operator->parameters == 2)
                {
                    if (strcmp(currentToken->value, "+") == 0)
                    {
                        printf("Add operator");
                        ASM_Math("add", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "-") == 0)
                    {
                        printf("Sub operator");
                        ASM_Math("sub", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "*") == 0)
                    {
                        printf("Mult operator");
                        ASM_Math("mul", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "/") == 0)
                    {
                        printf("Div operator");
                        ASM_Math("div", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "%%") == 0)
                    {
                        printf("%% operator");
                        ASM_Math("rem", T1, T2, T1);
                    }

                    if (strcmp(currentToken->value, "==") == 0)
                    {
                        printf("== operator");
                        ASM_Math("xor", T1, T2, T1);
                        ASM_MathI("sltiu", T1, T1, 1);
                    }
                    if (strcmp(currentToken->value, "!=") == 0)
                    {
                        printf("!= operator");
                        ASM_Math("xor", T1, T2, T1);
                        ASM_Math("sltu", X0, T1, T1);
                    }
                    if (strcmp(currentToken->value, "<") == 0)
                    {
                        printf("< operator");
                        ASM_Math("slt", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "<=") == 0)
                    {
                        printf("<= operator");
                        ASM_Math("slt", T1, T2, T3);
                        ASM_Math("xor", T1, T2, T4);
                        ASM_MathI("sltiu", T4, T4, 1);
                        ASM_Math("or", T3, T4, T1);
                    }
                    if (strcmp(currentToken->value, ">") == 0)
                    {
                        printf("> operator");

                        ASM_Math("slt", T1, T2, T3);
                        ASM_Math("xor", T1, T2, T1);
                        ASM_Math("sltu", X0, T1, T1);
                        ASM_Math("xor", T1, T3, T1);
                    }
                    if (strcmp(currentToken->value, ">=") == 0)
                    {
                        printf(">= operator");
                        ASM_Math("slt", T1, T2, T1);
                        ASM_MathI("xori", T1, T1, 1);
                    }
                    if (strcmp(currentToken->value, "|") == 0)
                    {
                        printf("| operator");
                        ASM_Math("or", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "||") == 0)
                    {
                        printf("|| operator");
                        ASM_Math("sltu", X0, T1, T1);
                        ASM_Math("sltu", X0, T2, T2);
                        ASM_Math("or", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "&") == 0)
                    {
                        printf("& operator");
                        ASM_Math("and", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "&&") == 0)
                    {
                        printf("&& operator");
                        ASM_Math("sltu", X0, T1, T1);
                        ASM_Math("sltu", X0, T2, T2);
                        ASM_Math("and", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "^") == 0)
                    {
                        printf("^ operator");
                        ASM_Math("xor", T1, T2, T1);
                    }
                }
                if (operator->parameters == 1)
                {
                    if (strcmp(currentToken->value, "u*") == 0)
                    {
                        printf("Dereference operator");
                        ASM_Math("add", T1, T0, T1);
                        ASM_LoadFromRegister(T1, T1);
                    }
                    if (strcmp(currentToken->value, "u-") == 0)
                    {
                        printf("unary sub operator");
                        ASM_Math("sub", X0, T1, T1);
                    }
                    if (strcmp(currentToken->value, "u!") == 0)
                    {
                        printf("Unary not");
                        ASM_Branch("beq", T1, X0, currentLabelNum);
                        ASM_MathI("addi", T1, X0, 1);
                        ASM_Label(currentLabelNum);
                        ASM_MathI("xori", T1, T1, 1);
                        currentLabelNum++;
                    }
                    if (strcmp(currentToken->value, "u~") == 0)
                    {
                        printf("unary sub operator");
                        ASM_Imm(T2, (uint32_t)(-1));
                        ASM_Math("xor", T1, T2, T1);
                    }
                    if (strcmp(currentToken->value, "u&") == 0)
                    {
                        if (param1StackLoc == NULL)
                        {
                            SetError(error, POINTERS, ASM_GENERATION, "'&' can only be used on variables", lineNumber);
                            goto exit;
                        }
                        printf("Unary reference\n");
                        // Pop the parameter load, because it is not needed for this operation
                        PopTargetASMList();
                        ASM_Imm(T1, param1StackLoc->offset);
                        ASM_Math("add", T0, T1, T1);
                    }
                }

                for (int i = 0; i < operator->parameters + 1; i++)
                {
                    free(DeleteListElement(postfix, index - operator->parameters));
                }
                ASM_StackPush(T1);
                UpdateStackList("TEMP", STACK_POINTER);

                Token *newToken = malloc(sizeof(Token));
                newToken->type = STACK_POINTER_TOKEN;
                newToken->lineNumber = currentToken->lineNumber;

                uint16_t stackIndex = ((GeneralList *)PeekList(scopes))->count - 1;
                newToken->value = malloc(IntLength(stackIndex) + 1);
                sprintf(newToken->value, "%hu", stackIndex);
                printf("insert to : %d\n", index - operator->parameters);
                ListInsert(postfix, index - operator->parameters, newToken);
                printf("set index to 0\n");
                index = 0;
                continue;
            }
            printf("loopDone\n");
            index++;
        }
    printf("Postfix converted, tokens left: %d\n", postfix->count);
    delay(100);
    if (postfix->count > 1)
    {
        SetError(error, SYNTAX_ERROR, ASM_GENERATION, "Equation did not simplfiy", lineNumber);
        goto exit;
    }
    Token *token = (Token *)PeekList(postfix);
    printf("Type: %d\n", token->type);
    printf("Line: %d\n", token->lineNumber);

    return;
exit:
    return;
}
#endif