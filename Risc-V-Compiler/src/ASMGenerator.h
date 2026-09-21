#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "Common.h"
#include "SemanticAnalyzer.h"

GeneralList *asmProgram;

#define RESULT_REG T2

void SetASMDestination(GeneralList *asmList)
{
    asmProgram = asmList;
}
void ASM_Math(char *operation, Registers inA, Registers inB, Registers out)
{
    char *cstr = malloc(strlen(operation) + 1 + 2 + 1 + 2 + 1 + 2 + 1);
    sprintf(cstr, "%s t%d,t%d,t%d", operation, (uint8_t)out, (uint8_t)inA, (uint8_t)inB);
    PushList(asmProgram, cstr);
    printf("asm math done\n");
}
void ASM_Move(Registers source, Registers destination)
{
    char *cstr = malloc(strlen("mv") + 1 + 2 + 1 + 2 + 1);
    sprintf(cstr, "mv t%d,t%d", (uint8_t)destination, (uint8_t)source);
    PushList(asmProgram, cstr);
    printf("asm move done\n");
}
void ASM_Imm(Registers destination, uint32_t value)
{
    char *cstr = malloc(strlen("addi") + 1 + 2 + 1 + 2 + 1 + IntLength(value) + 1);
    sprintf(cstr, "addi t%d,x0,%d", (uint8_t)destination, value);
    PushList(asmProgram, cstr);
    printf("asm Imm done\n");
}
void PrintASMProgram()
{
    for (int i = 0; i < asmProgram->count; i++)
    {
        char *value = (char *)ListGetIndex(asmProgram, i);
        printf("%s\n", value);
    }
}
#endif