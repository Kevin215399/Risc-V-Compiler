#include "Common.h"
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "ASMGenerator.h"
#include "SpecialParser.h"
char *sourceCode = "void main(){pinOut(25); pinToggle(25); return 1;}";

//I think i added passing parameters, but only when it goes through the special parse
//Either get rid of independent calls or add it to call nodes

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
    GeneralList tokens;
    InitializeList(&tokens);

    Error *error = LexicalAnylsis(sourceCode, &tokens);
    printf("Done\n");
    if (error->errorCode != NO_ERROR)
    {
        PrintError(error);
        free(error);
        goto inspect;
    }
    free(error);
    for (int i = 0; i < tokens.count; i++)
    {
        Token *token = (Token *)ListGetIndex(&tokens, i);
        printf("Token: %d, %s\n", token->type, token->value);
    }
    sleep_ms(300);

    TreeNode *tree = (TreeNode *)malloc(sizeof(TreeNode));
    InitializeTree(tree);
    error = AnalyzeSyntax(&tokens, tree);
    printf("Done\n");
    if (error->errorCode != NO_ERROR)
    {
        PrintError(error);
        free(error);
        goto inspect;
    }
    free(error);

    SetASMDestination();

    error = SemanticAnylsis(tree);
    printf("Done\n");
    if (error->errorCode != NO_ERROR)
    {
        PrintError(error);
        free(error);
        goto inspect;
    }
    free(error);

    PrintASMProgram();

inspect:
    char message[32] = {'\0'};
    uint8_t messageIndex = 0;
    GeneralList navigation;
    InitializeList(&navigation);
    while (true)
    {
        char c = (char)getchar();

        if (c == '\n')
        {
            TreeNode *node = TreeGetNode(tree, &navigation);
            if (node == NULL)
                goto getNodeFail;
            uint8_t *value = ((Token *)node->content)->value;

            printf("Node value: ");
            if (*value >= TERM_OFFSET)
            {
                printf("%d\n", (int)(*value) - TERM_OFFSET);
            }
            else
            {
                printf("%s\n", (char *)value);
            }
            int children = (int)node->children.count;
            for (int i = 0; i < children; i++)
            {
                PushByte(&navigation, i);
                TreeNode *node = TreeGetNode(tree, &navigation);
                if (node == NULL)
                    goto getNodeFail;
                uint8_t *value = ((Token *)node->content)->value;
                if (*value >= TERM_OFFSET)
                {
                    printf("%d, ", (int)(*value) - TERM_OFFSET);
                }
                else
                {
                    printf("%s, ", (char *)value);
                }
                free(PopList(&navigation));
            }
            printf("\n");

        getNodeFail:
            EmptyList(&navigation);
            messageIndex = 0;
            memset(message, 0, sizeof(message));
        }
        else if (IsNumber(c))
        {
            PushByte(&navigation, (uint8_t)(c - 48));
            message[messageIndex++] = c;
        }
    }
exit:
    while (1)
    {
        sleep_ms(100);
    }
}
