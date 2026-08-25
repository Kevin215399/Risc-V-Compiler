#include "Common.h"
#include "Lexer.h"
#include "SyntaxAnalyzer.h"

char *sourceCode = "if(a<0){a=5;}";

void InitializeClasses()
{
    SetupCFG();
}

int main()
{
    stdio_init_all();
    sleep_ms(5000);
    printf("start");

    InitializeClasses();

    GeneralList *tokens = (GeneralList *)malloc(sizeof(GeneralList));
    InitializeList(tokens);

    Error *error = LexicalAnylsis(sourceCode, tokens);
    printf("Done\n");
    if (error->errorCode != NO_ERROR)
    {
        printf("Error: %d\n", error->errorCode);
        free(error);
        goto stall;
    }
    free(error);


    for (int i = 0; i < tokens->count; i++)
    {
        Token *token = (Token *)ListGetIndex(tokens, i);
        printf("Token: %d, %s\n", token->type, token->value);
    }
    sleep_ms(300);

    TreeNode *tree = (TreeNode*)malloc(sizeof(TreeNode));
    error = AnalyzeSyntax(tokens, tree);
    printf("Done\n");
    if (error->errorCode != NO_ERROR)
    {
        printf("Error: %d\n", error->errorCode);
        free(error);
        goto stall;
    }
    free(error);


stall:
    while (1)
    {
        sleep_ms(100);
    }
}
