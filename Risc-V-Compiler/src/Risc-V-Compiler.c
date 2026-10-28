#include "Common.h"
#include "Lexer.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "ASMGenerator.h"
#include "SpecialParser.h"
char *sourceCode = R"(

void main()
{
    int RLED = 16;
    int GLED = 17;
    int BLED = 18;

    pinMode(16,OUTPUT);
    pinMode(17,OUTPUT);
    pinMode(18,OUTPUT);
    pinMode(2,INPUT_PULLUP);
    pinMode(3,INPUT_PULLUP);
    pinMode(4,INPUT_PULLUP);
    pinMode(5,INPUT_PULLUP);

    int blink = 0;
    bool mode = 0;

    while(true){
        if(!pinRead(2)){
            if(mode == 0){
                pinWrite(RLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(RLED);
            }
        }
        if(pinRead(2)){
            pinWrite(RLED,LOW);
        }
            
        if(!pinRead(3)){
            if(mode == 0){
                pinWrite(GLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(GLED);
            }
        }
        if(pinRead(3)){
            pinWrite(GLED,LOW);
        }
            
        if(!pinRead(4)){
            if(mode == 0){
                pinWrite(BLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(BLED);
            }
        }
        if(pinRead(4)){
            pinWrite(BLED,LOW);
        }

        if(!pinRead(5)){
            mode ^= 1;
            int wait = 0;
            while(wait < 1000000){
                wait+=1;
            }
        }
        blink += 1;
        if(blink > 10000){
            blink = 0;
        }
        
    }
    return 0;
}

)";

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
