#include "Common.h"
#include "Lexer.h"

char *sourceCode = "int test = 5; test += 4; if";

void InitializeClasses()
{
    // nothing yet...
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

stall:
    while (1)
    {
        sleep_ms(100);
    }
}
