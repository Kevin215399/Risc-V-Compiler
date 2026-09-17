#ifndef CFG_h
#define CFG_h

const char *NonTerms[] = {"uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int", "char", "bool", "void", "", "while", "for", "if", "IDENTIFIER", "return", "break", "pinSet", "pinFlip", "pinMode", "print", "malloc", "memset", "memread", "free", "}", "*", ")", "[", ";", "(", "=", ",", "else", "{", "-", "~", "&", "!", "LITERAL", "+=", "-=", "*=", "/=", "^=", "%%=", "]", "<", ">", "<=", ">=", "!=", "==", "+", "/", "%%"};
#define GLOBAL_BODY 0
#define GLOBAL_LINE 1
#define BODY 2
#define LINE 3
#define DECLERATION 4
#define POINTER 5
#define ARRAY 6
#define DECREST 7
#define VAR_DEC_REST 8
#define FUNC_DEC_REST 9
#define FUNCTION_PARAMS 10
#define PARAM_REST 11
#define WHILE 12
#define FOR 13
#define IF 14
#define ELSE 15
#define ELSE_REST 16
#define BREAK 17
#define RETURN 18
#define RETURN_VAL 19
#define ASSIGN 20
#define ASSIGN_OR_CALL 21
#define ASSIGN_REST 22
#define CALL 23
#define CALL_PARAM 24
#define SPECIAL 25
#define PIN_SET 26
#define PIN_FLIP 27
#define PIN_MODE 28
#define PRINT 29
#define EXPRESSION 30
#define EQUALITY 31
#define EQUALITYREST 32
#define ADDITION 33
#define ADDITIONREST 34
#define MULTIPLY 35
#define MULTIPLYREST 36
#define UNARY_ATOM 37
#define ATOM 38
#define TYPE_TERM 39
#define IDENTIFIER_TERM 40
#define LITERAL_TERM 41
#define MALLOC 42
#define MEMSET 43
#define MEMREAD 44
#define FREE 45
#define CAST_OR_EXPR 46
#define TRY_CALL 47

typedef struct GrammarRule
{
    uint8_t x;
    uint8_t y;
    GeneralList stack;
} GrammarRule;

GeneralList grammarTable;
#define TERM_OFFSET 128

uint8_t GetTermIndex(char *term)
{
    uint8_t x = 255;
    for (int i = 0; i < sizeof(NonTerms) / sizeof(NonTerms[0]); i++)
    {
        if (strcmp(NonTerms[i], term) == 0)
        {
            x = i;
            break;
        }
    }
    return x;
}
uint16_t FindRuleInCFG(uint8_t x, uint8_t y)
{
    for (int i = 0; i < grammarTable.count; i++)
    {
        GrammarRule *checkRule = (GrammarRule *)ListGetIndex(&grammarTable, i);
        if (checkRule->x == x && checkRule->y == y)
        {
            return i;
        }
    }
    // rule does not exist
    return (uint16_t)(-1);
}
uint16_t FindOrCreateRuleInCFG(uint8_t x, uint8_t y)
{
    uint16_t ruleIndex = FindRuleInCFG(x, y);
    if (ruleIndex != (uint16_t)(-1))
    {
        // rule already exists
        return ruleIndex;
    }
    // rule does not exist, create it
    GrammarRule *newRule = (GrammarRule *)malloc(sizeof(GrammarRule));
    newRule->x = x;
    newRule->y = y;
    InitializeList(&newRule->stack);
    PushList(&grammarTable, newRule);
    return grammarTable.count - 1;
}
void AddTerm(uint8_t y, uint8_t x, char *term)
{
    uint16_t ruleIndex = FindOrCreateRuleInCFG(x, y);
    GrammarRule *rule = (GrammarRule *)ListGetIndex(&grammarTable, ruleIndex);
    if (rule == NULL)
        return;
    PushCharArr(&rule->stack, term);
}
void AddNonTerm(uint8_t y, uint8_t x, uint8_t nonterm)
{
    uint16_t ruleIndex = FindOrCreateRuleInCFG(x, y);
    GrammarRule *rule = (GrammarRule *)ListGetIndex(&grammarTable, ruleIndex);
    if (rule == NULL)
        return;
    PushByte(&rule->stack, nonterm + TERM_OFFSET);
}
void SetupCFG()
{
    InitializeList(&grammarTable);

    /*
        Transformations required from excel sheet
        , 0) -> , "*") and nonterm
        "" -> ""
        "%" -> "%%"
        "%=" -> "%%="
    */

    // GLOBAL_BODY
    AddNonTerm(0, 0, 1);
    AddNonTerm(0, 0, 0);
    AddNonTerm(0, 1, 1);
    AddNonTerm(0, 1, 0);
    AddNonTerm(0, 2, 1);
    AddNonTerm(0, 2, 0);
    AddNonTerm(0, 3, 1);
    AddNonTerm(0, 3, 0);
    AddNonTerm(0, 4, 1);
    AddNonTerm(0, 4, 0);
    AddNonTerm(0, 5, 1);
    AddNonTerm(0, 5, 0);
    AddNonTerm(0, 6, 1);
    AddNonTerm(0, 6, 0);
    AddNonTerm(0, 7, 1);
    AddNonTerm(0, 7, 0);
    AddNonTerm(0, 8, 1);
    AddNonTerm(0, 8, 0);
    AddTerm(0, 9, "");
    // GLOBAL_LINE
    AddNonTerm(1, 0, 4);
    AddNonTerm(1, 1, 4);
    AddNonTerm(1, 2, 4);
    AddNonTerm(1, 3, 4);
    AddNonTerm(1, 4, 4);
    AddNonTerm(1, 5, 4);
    AddNonTerm(1, 6, 4);
    AddNonTerm(1, 7, 4);
    AddNonTerm(1, 8, 4);
    // BODY
    AddNonTerm(2, 0, 3);
    AddNonTerm(2, 0, 2);
    AddNonTerm(2, 1, 3);
    AddNonTerm(2, 1, 2);
    AddNonTerm(2, 2, 3);
    AddNonTerm(2, 2, 2);
    AddNonTerm(2, 3, 3);
    AddNonTerm(2, 3, 2);
    AddNonTerm(2, 4, 3);
    AddNonTerm(2, 4, 2);
    AddNonTerm(2, 5, 3);
    AddNonTerm(2, 5, 2);
    AddNonTerm(2, 6, 3);
    AddNonTerm(2, 6, 2);
    AddNonTerm(2, 7, 3);
    AddNonTerm(2, 7, 2);
    AddNonTerm(2, 8, 3);
    AddNonTerm(2, 8, 2);
    AddTerm(2, 9, "");
    AddNonTerm(2, 10, 3);
    AddNonTerm(2, 10, 2);
    AddNonTerm(2, 11, 3);
    AddNonTerm(2, 11, 2);
    AddNonTerm(2, 12, 3);
    AddNonTerm(2, 12, 2);
    AddNonTerm(2, 13, 3);
    AddNonTerm(2, 13, 2);
    AddNonTerm(2, 14, 3);
    AddNonTerm(2, 14, 2);
    AddNonTerm(2, 15, 3);
    AddNonTerm(2, 15, 2);
    AddNonTerm(2, 16, 3);
    AddNonTerm(2, 16, 2);
    AddNonTerm(2, 17, 3);
    AddNonTerm(2, 17, 2);
    AddNonTerm(2, 18, 3);
    AddNonTerm(2, 18, 2);
    AddNonTerm(2, 19, 3);
    AddNonTerm(2, 19, 2);
    AddNonTerm(2, 20, 3);
    AddNonTerm(2, 20, 2);
    AddNonTerm(2, 21, 3);
    AddNonTerm(2, 21, 2);
    AddNonTerm(2, 22, 3);
    AddNonTerm(2, 22, 2);
    AddNonTerm(2, 23, 3);
    AddNonTerm(2, 23, 2);
    AddTerm(2, 24, "");
    // LINE
    AddNonTerm(3, 0, 4);
    AddNonTerm(3, 1, 4);
    AddNonTerm(3, 2, 4);
    AddNonTerm(3, 3, 4);
    AddNonTerm(3, 4, 4);
    AddNonTerm(3, 5, 4);
    AddNonTerm(3, 6, 4);
    AddNonTerm(3, 7, 4);
    AddNonTerm(3, 8, 4);
    AddNonTerm(3, 10, 12);
    AddNonTerm(3, 11, 13);
    AddNonTerm(3, 12, 14);
    AddNonTerm(3, 13, 20);
    AddNonTerm(3, 14, 18);
    AddNonTerm(3, 15, 17);
    AddNonTerm(3, 16, 25);
    AddNonTerm(3, 17, 25);
    AddNonTerm(3, 18, 25);
    AddNonTerm(3, 19, 25);
    AddNonTerm(3, 20, 25);
    AddNonTerm(3, 21, 25);
    AddNonTerm(3, 22, 25);
    AddNonTerm(3, 23, 25);
    AddTerm(3, 24, "");
    // DECLERATION
    AddNonTerm(4, 0, 39);
    AddNonTerm(4, 0, 5);
    AddNonTerm(4, 0, 40);
    AddNonTerm(4, 0, 6);
    AddNonTerm(4, 0, 7);
    AddNonTerm(4, 1, 39);
    AddNonTerm(4, 1, 5);
    AddNonTerm(4, 1, 40);
    AddNonTerm(4, 1, 6);
    AddNonTerm(4, 1, 7);
    AddNonTerm(4, 2, 39);
    AddNonTerm(4, 2, 5);
    AddNonTerm(4, 2, 40);
    AddNonTerm(4, 2, 6);
    AddNonTerm(4, 2, 7);
    AddNonTerm(4, 3, 39);
    AddNonTerm(4, 3, 5);
    AddNonTerm(4, 3, 40);
    AddNonTerm(4, 3, 6);
    AddNonTerm(4, 3, 7);
    AddNonTerm(4, 4, 39);
    AddNonTerm(4, 4, 5);
    AddNonTerm(4, 4, 40);
    AddNonTerm(4, 4, 6);
    AddNonTerm(4, 4, 7);
    AddNonTerm(4, 5, 39);
    AddNonTerm(4, 5, 5);
    AddNonTerm(4, 5, 40);
    AddNonTerm(4, 5, 6);
    AddNonTerm(4, 5, 7);
    AddNonTerm(4, 6, 39);
    AddNonTerm(4, 6, 5);
    AddNonTerm(4, 6, 40);
    AddNonTerm(4, 6, 6);
    AddNonTerm(4, 6, 7);
    AddNonTerm(4, 7, 39);
    AddNonTerm(4, 7, 5);
    AddNonTerm(4, 7, 40);
    AddNonTerm(4, 7, 6);
    AddNonTerm(4, 7, 7);
    AddNonTerm(4, 8, 39);
    AddNonTerm(4, 8, 5);
    AddNonTerm(4, 8, 40);
    AddNonTerm(4, 8, 6);
    AddNonTerm(4, 8, 7);
    // POINTER
    AddTerm(5, 9, "");
    AddTerm(5, 13, "");
    AddTerm(5, 25, "*");
    AddNonTerm(5, 25, 5);
    AddTerm(5, 26, "");
    // ARRAY
    AddTerm(6, 9, "");
    AddTerm(6, 26, "");
    AddTerm(6, 27, "[");
    AddNonTerm(6, 27, 30);
    AddTerm(6, 27, "]");
    AddTerm(6, 28, "");
    AddTerm(6, 29, "");
    AddTerm(6, 30, "");
    AddTerm(6, 31, "");

    // DECREST
    AddNonTerm(7, 28, 8);
    AddNonTerm(7, 29, 9);
    AddNonTerm(7, 30, 8);
    // VAR_DEC_REST
    AddTerm(8, 28, ";");
    AddTerm(8, 30, "=");
    AddNonTerm(8, 30, 30);
    AddTerm(8, 30, ";");
    // FUNC_DEC_REST
    AddTerm(9, 29, "(");
    AddNonTerm(9, 29, 10);
    AddTerm(9, 29, ")");
    AddTerm(9, 29, "{");
    AddNonTerm(9, 29, 2);
    AddTerm(9, 29, "}");
    // FUNCTION_PARAMS
    AddNonTerm(10, 0, 39);
    AddNonTerm(10, 0, 5);
    AddNonTerm(10, 0, 40);
    AddNonTerm(10, 0, 6);
    AddNonTerm(10, 0, 11);
    AddNonTerm(10, 1, 39);
    AddNonTerm(10, 1, 5);
    AddNonTerm(10, 1, 40);
    AddNonTerm(10, 1, 6);
    AddNonTerm(10, 1, 11);
    AddNonTerm(10, 2, 39);
    AddNonTerm(10, 2, 5);
    AddNonTerm(10, 2, 40);
    AddNonTerm(10, 2, 6);
    AddNonTerm(10, 2, 11);
    AddNonTerm(10, 3, 39);
    AddNonTerm(10, 3, 5);
    AddNonTerm(10, 3, 40);
    AddNonTerm(10, 3, 6);
    AddNonTerm(10, 3, 11);
    AddNonTerm(10, 4, 39);
    AddNonTerm(10, 4, 5);
    AddNonTerm(10, 4, 40);
    AddNonTerm(10, 4, 6);
    AddNonTerm(10, 4, 11);
    AddNonTerm(10, 5, 39);
    AddNonTerm(10, 5, 5);
    AddNonTerm(10, 5, 40);
    AddNonTerm(10, 5, 6);
    AddNonTerm(10, 5, 11);
    AddNonTerm(10, 6, 39);
    AddNonTerm(10, 6, 5);
    AddNonTerm(10, 6, 40);
    AddNonTerm(10, 6, 6);
    AddNonTerm(10, 6, 11);
    AddNonTerm(10, 7, 39);
    AddNonTerm(10, 7, 5);
    AddNonTerm(10, 7, 40);
    AddNonTerm(10, 7, 6);
    AddNonTerm(10, 7, 11);
    AddNonTerm(10, 8, 39);
    AddNonTerm(10, 8, 5);
    AddNonTerm(10, 8, 40);
    AddNonTerm(10, 8, 6);
    AddNonTerm(10, 8, 11);
    AddTerm(10, 9, "");
    AddTerm(10, 26, "");
    // PARAM_REST
    AddTerm(11, 9, "");
    AddTerm(11, 26, "");
    AddTerm(11, 31, ",");
    AddNonTerm(11, 31, 10);
    // WHILE
    AddTerm(12, 10, "while");
    AddTerm(12, 10, "(");
    AddNonTerm(12, 10, 30);
    AddTerm(12, 10, ")");
    AddTerm(12, 10, "{");
    AddNonTerm(12, 10, 2);
    AddTerm(12, 10, "}");
    // FOR
    AddTerm(13, 11, "for");
    AddTerm(13, 11, "(");
    AddNonTerm(13, 11, 39);
    AddNonTerm(13, 11, 40);
    AddTerm(13, 11, "=");
    AddNonTerm(13, 11, 30);
    AddTerm(13, 11, ";");
    AddNonTerm(13, 11, 30);
    AddTerm(13, 11, ";");
    AddNonTerm(13, 11, 20);
    AddTerm(13, 11, ")");
    AddTerm(13, 11, "{");
    AddNonTerm(13, 11, 2);
    AddTerm(13, 11, "}");
    // IF
    AddTerm(14, 12, "if");
    AddTerm(14, 12, "(");
    AddNonTerm(14, 12, 30);
    AddTerm(14, 12, ")");
    AddTerm(14, 12, "{");
    AddNonTerm(14, 12, 2);
    AddTerm(14, 12, "}");
    AddNonTerm(14, 12, 15);
    // ELSE
    AddTerm(15, 0, "");
    AddTerm(15, 1, "");
    AddTerm(15, 2, "");
    AddTerm(15, 3, "");
    AddTerm(15, 4, "");
    AddTerm(15, 5, "");
    AddTerm(15, 6, "");
    AddTerm(15, 7, "");
    AddTerm(15, 8, "");
    AddTerm(15, 9, "");
    AddTerm(15, 10, "");
    AddTerm(15, 11, "");
    AddTerm(15, 12, "");
    AddTerm(15, 13, "");
    AddTerm(15, 14, "");
    AddTerm(15, 15, "");
    AddTerm(15, 16, "");
    AddTerm(15, 17, "");
    AddTerm(15, 18, "");
    AddTerm(15, 19, "");
    AddTerm(15, 20, "");
    AddTerm(15, 21, "");
    AddTerm(15, 22, "");
    AddTerm(15, 23, "");
    AddTerm(15, 24, "");
    AddTerm(15, 32, "else");
    AddNonTerm(15, 32, 16);
    // ELSE_REST
    AddNonTerm(16, 12, 14);
    AddTerm(16, 33, "{");
    AddNonTerm(16, 33, 2);
    AddTerm(16, 33, "}");
    // BREAK
    AddTerm(17, 15, "break");
    AddTerm(17, 15, ";");
    // RETURN
    AddTerm(18, 14, "return");
    AddNonTerm(18, 14, 19);
    AddTerm(18, 14, ";");
    // RETURN_VAL
    AddTerm(19, 9, "");
    AddNonTerm(19, 13, 30);
    AddNonTerm(19, 20, 30);
    AddNonTerm(19, 22, 30);
    AddNonTerm(19, 25, 30);
    AddTerm(19, 28, "");
    AddNonTerm(19, 29, 30);
    AddNonTerm(19, 34, 30);
    AddNonTerm(19, 35, 30);
    AddNonTerm(19, 36, 30);
    AddNonTerm(19, 37, 30);
    AddNonTerm(19, 38, 30);
    AddTerm(19, 50, "ssss");
    // ASSIGN
    AddNonTerm(20, 13, 40);
    AddNonTerm(20, 13, 21);
    AddTerm(20, 13, ";");
    // ASSIGN_OR_CALL
    AddNonTerm(21, 29, 23);
    AddTerm(21, 30, "");
    AddNonTerm(21, 30, 22);
    AddTerm(21, 39, "");
    AddNonTerm(21, 39, 22);
    AddTerm(21, 40, "");
    AddNonTerm(21, 40, 22);
    AddTerm(21, 41, "");
    AddNonTerm(21, 41, 22);
    AddTerm(21, 42, "");
    AddNonTerm(21, 42, 22);
    AddTerm(21, 43, "");
    AddNonTerm(21, 43, 22);
    AddTerm(21, 44, "");
    AddNonTerm(21, 44, 22);
    // ASSIGN_REST
    AddTerm(22, 30, "=");
    AddNonTerm(22, 30, 30);
    AddTerm(22, 39, "+=");
    AddNonTerm(22, 39, 30);
    AddTerm(22, 40, "-=");
    AddNonTerm(22, 40, 30);
    AddTerm(22, 41, "*=");
    AddNonTerm(22, 41, 30);
    AddTerm(22, 42, "/=");
    AddNonTerm(22, 42, 30);
    AddTerm(22, 43, "^=");
    AddNonTerm(22, 43, 30);
    AddTerm(22, 44, "%%=");
    AddNonTerm(22, 44, 30);
    // CALL
    AddTerm(23, 29, "");
    AddTerm(23, 29, "(");
    AddNonTerm(23, 29, 24);
    AddTerm(23, 29, ")");
    // CALL_PARAM
    AddNonTerm(24, 13, 30);
    AddNonTerm(24, 20, 30);
    AddNonTerm(24, 22, 30);
    AddTerm(24, 26, "");
    AddNonTerm(24, 29, 30);
    AddTerm(24, 31, ",");
    AddNonTerm(24, 31, 30);
    AddNonTerm(24, 34, 30);
    AddNonTerm(24, 35, 30);
    AddNonTerm(24, 36, 30);
    AddNonTerm(24, 37, 30);
    AddNonTerm(24, 38, 30);
    // SPECIAL
    AddNonTerm(25, 16, 26);
    AddNonTerm(25, 17, 27);
    AddNonTerm(25, 18, 28);
    AddNonTerm(25, 19, 29);
    AddNonTerm(25, 20, 42);
    AddNonTerm(25, 21, 43);
    AddNonTerm(25, 22, 44);
    AddNonTerm(25, 23, 45);
    // PIN_SET
    AddTerm(26, 16, "pinSet");
    AddTerm(26, 16, "(");
    AddNonTerm(26, 16, 30);
    AddTerm(26, 16, ",");
    AddNonTerm(26, 16, 30);
    AddTerm(26, 16, ")");
    AddTerm(26, 16, ";");
    // PIN_FLIP
    AddTerm(27, 17, "pinFlip");
    AddTerm(27, 17, "(");
    AddNonTerm(27, 17, 30);
    AddTerm(27, 17, ")");
    AddTerm(27, 17, ";");
    // PIN_MODE
    AddTerm(28, 18, "pinMode");
    AddTerm(28, 18, "(");
    AddNonTerm(28, 18, 30);
    AddTerm(28, 18, ",");
    AddNonTerm(28, 18, 30);
    AddTerm(28, 18, ")");
    AddTerm(28, 18, ";");
    // PRINT
    AddTerm(29, 19, "print");
    AddTerm(29, 19, "(");
    AddNonTerm(29, 19, 30);
    AddTerm(29, 19, ")");
    AddTerm(29, 19, ";");
    // EXPRESSION
    AddNonTerm(30, 13, 31);
    AddNonTerm(30, 20, 31);
    AddNonTerm(30, 22, 31);
    AddNonTerm(30, 29, 31);
    AddNonTerm(30, 34, 31);
    AddNonTerm(30, 35, 31);
    AddNonTerm(30, 36, 31);
    AddNonTerm(30, 37, 31);
    AddNonTerm(30, 38, 31);
    // EQUALITY
    AddNonTerm(31, 13, 33);
    AddNonTerm(31, 13, 32);
    AddNonTerm(31, 20, 33);
    AddNonTerm(31, 20, 32);
    AddNonTerm(31, 22, 33);
    AddNonTerm(31, 22, 32);
    AddNonTerm(31, 29, 33);
    AddNonTerm(31, 29, 32);
    AddNonTerm(31, 34, 33);
    AddNonTerm(31, 34, 32);
    AddNonTerm(31, 35, 33);
    AddNonTerm(31, 35, 32);
    AddNonTerm(31, 36, 33);
    AddNonTerm(31, 36, 32);
    AddNonTerm(31, 37, 33);
    AddNonTerm(31, 37, 32);
    AddNonTerm(31, 38, 33);
    AddNonTerm(31, 38, 32);
    // EQUALITYREST
    AddTerm(32, 26, "");
    AddTerm(32, 28, "");
    AddTerm(32, 31, "");
    AddTerm(32, 45, "");
    AddTerm(32, 46, "<");
    AddNonTerm(32, 46, 33);
    AddNonTerm(32, 46, 32);
    AddTerm(32, 47, ">");
    AddNonTerm(32, 47, 33);
    AddNonTerm(32, 47, 32);
    AddTerm(32, 48, "<=");
    AddNonTerm(32, 48, 33);
    AddNonTerm(32, 48, 32);
    AddTerm(32, 49, ">=");
    AddNonTerm(32, 49, 33);
    AddNonTerm(32, 49, 32);
    AddTerm(32, 50, "!=");
    AddNonTerm(32, 50, 33);
    AddNonTerm(32, 50, 32);
    AddTerm(32, 51, "==");
    AddNonTerm(32, 51, 33);
    AddNonTerm(32, 51, 32);
    // ADDITION
    AddNonTerm(33, 13, 35);
    AddNonTerm(33, 13, 34);
    AddNonTerm(33, 20, 35);
    AddNonTerm(33, 20, 34);
    AddNonTerm(33, 22, 35);
    AddNonTerm(33, 22, 34);
    AddNonTerm(33, 29, 35);
    AddNonTerm(33, 29, 34);
    AddNonTerm(33, 34, 35);
    AddNonTerm(33, 34, 34);
    AddNonTerm(33, 35, 35);
    AddNonTerm(33, 35, 34);
    AddNonTerm(33, 36, 35);
    AddNonTerm(33, 36, 34);
    AddNonTerm(33, 37, 35);
    AddNonTerm(33, 37, 34);
    AddNonTerm(33, 38, 35);
    AddNonTerm(33, 38, 34);
    // ADDITIONREST
    AddTerm(34, 26, "");
    AddTerm(34, 28, "");
    AddTerm(34, 31, "");
    AddTerm(34, 34, "-");
    AddNonTerm(34, 34, 35);
    AddNonTerm(34, 34, 34);
    AddTerm(34, 45, "");
    AddTerm(34, 46, "");
    AddTerm(34, 47, "");
    AddTerm(34, 48, "");
    AddTerm(34, 49, "");
    AddTerm(34, 50, "");
    AddTerm(34, 51, "");
    AddTerm(34, 52, "+");
    AddNonTerm(34, 52, 35);
    AddNonTerm(34, 52, 34);
    // MULTIPLY
    AddNonTerm(35, 13, 37);
    AddNonTerm(35, 13, 36);
    AddNonTerm(35, 20, 37);
    AddNonTerm(35, 20, 36);
    AddNonTerm(35, 22, 37);
    AddNonTerm(35, 22, 36);
    AddNonTerm(35, 29, 37);
    AddNonTerm(35, 29, 36);
    AddNonTerm(35, 34, 37);
    AddNonTerm(35, 34, 36);
    AddNonTerm(35, 35, 37);
    AddNonTerm(35, 35, 36);
    AddNonTerm(35, 36, 37);
    AddNonTerm(35, 36, 36);
    AddNonTerm(35, 37, 37);
    AddNonTerm(35, 37, 36);
    AddNonTerm(35, 38, 37);
    AddNonTerm(35, 38, 36);
    // MULTIPLYREST
    AddTerm(36, 25, "*");
    AddNonTerm(36, 25, 37);
    AddNonTerm(36, 25, 36);
    AddTerm(36, 26, "");
    AddTerm(36, 28, "");
    AddTerm(36, 31, "");
    AddTerm(36, 34, "");
    AddTerm(36, 45, "");
    AddTerm(36, 46, "");
    AddTerm(36, 47, "");
    AddTerm(36, 48, "");
    AddTerm(36, 49, "");
    AddTerm(36, 50, "");
    AddTerm(36, 51, "");
    AddTerm(36, 52, "");
    AddTerm(36, 53, "/");
    AddNonTerm(36, 53, 37);
    AddNonTerm(36, 53, 36);
    AddTerm(36, 54, "%%");
    AddNonTerm(36, 54, 37);
    AddNonTerm(36, 54, 36);
    // UNARY_ATOM
    AddNonTerm(37, 13, 38);
    AddNonTerm(37, 20, 38);
    AddNonTerm(37, 22, 38);
    AddTerm(37, 25, "*");
    AddNonTerm(37, 25, 37);
    AddTerm(37, 29, "(");
    AddNonTerm(37, 29, 46);
    AddTerm(37, 34, "-");
    AddNonTerm(37, 34, 37);
    AddTerm(37, 35, "~");
    AddNonTerm(37, 35, 37);
    AddTerm(37, 36, "&");
    AddNonTerm(37, 36, 37);
    AddTerm(37, 37, "!");
    AddNonTerm(37, 37, 37);
    AddNonTerm(37, 38, 38);
    // ATOM
    AddNonTerm(38, 13, 40);
    AddNonTerm(38, 13, 47);
    AddNonTerm(38, 20, 42);
    AddNonTerm(38, 22, 44);
    AddTerm(38, 29, "(");
    AddNonTerm(38, 29, 30);
    AddTerm(38, 29, ")");
    AddNonTerm(38, 38, 41);
    // TYPE
    AddTerm(39, 0, "uint8_t");
    AddTerm(39, 1, "uint16_t");
    AddTerm(39, 2, "uint32_t");
    AddTerm(39, 3, "int8_t");
    AddTerm(39, 4, "int16_t");
    AddTerm(39, 5, "int");
    AddTerm(39, 6, "char");
    AddTerm(39, 7, "bool");
    AddTerm(39, 8, "void");
    // IDENTIFIER
    // LITERAL
    // MALLOC
    AddTerm(42, 20, "malloc");
    AddTerm(42, 20, "(");
    AddNonTerm(42, 20, 30);
    AddTerm(42, 20, ")");
    AddTerm(42, 20, ";");
    // MEMSET
    AddTerm(43, 21, "memset");
    AddTerm(43, 21, "(");
    AddNonTerm(43, 21, 30);
    AddTerm(43, 21, ",");
    AddNonTerm(43, 21, 30);
    AddTerm(43, 21, ")");
    AddTerm(43, 21, ";");
    // MEMREAD
    AddTerm(44, 22, "memread");
    AddTerm(44, 22, "(");
    AddNonTerm(44, 22, 30);
    AddTerm(44, 22, ")");
    AddTerm(44, 22, ";");
    // FREE
    AddTerm(45, 23, "free");
    AddTerm(45, 23, "(");
    AddNonTerm(45, 23, 30);
    AddTerm(45, 23, ")");
    AddTerm(45, 23, ";");
    // CAST_OR_EXPR
    AddNonTerm(46, 0, 39);
    AddNonTerm(46, 0, 5);
    AddTerm(46, 0, ")");
    AddNonTerm(46, 0, 37);
    AddNonTerm(46, 1, 39);
    AddNonTerm(46, 1, 5);
    AddTerm(46, 1, ")");
    AddNonTerm(46, 1, 37);
    AddNonTerm(46, 2, 39);
    AddNonTerm(46, 2, 5);
    AddTerm(46, 2, ")");
    AddNonTerm(46, 2, 37);
    AddNonTerm(46, 3, 39);
    AddNonTerm(46, 3, 5);
    AddTerm(46, 3, ")");
    AddNonTerm(46, 3, 37);
    AddNonTerm(46, 4, 39);
    AddNonTerm(46, 4, 5);
    AddTerm(46, 4, ")");
    AddNonTerm(46, 4, 37);
    AddNonTerm(46, 5, 39);
    AddNonTerm(46, 5, 5);
    AddTerm(46, 5, ")");
    AddNonTerm(46, 5, 37);
    AddNonTerm(46, 6, 39);
    AddNonTerm(46, 6, 5);
    AddTerm(46, 6, ")");
    AddNonTerm(46, 6, 37);
    AddNonTerm(46, 7, 39);
    AddNonTerm(46, 7, 5);
    AddTerm(46, 7, ")");
    AddNonTerm(46, 7, 37);
    AddNonTerm(46, 8, 39);
    AddNonTerm(46, 8, 5);
    AddTerm(46, 8, ")");
    AddNonTerm(46, 8, 37);
    AddNonTerm(46, 13, 30);
    AddTerm(46, 13, ")");
    AddNonTerm(46, 20, 30);
    AddTerm(46, 20, ")");
    AddNonTerm(46, 22, 30);
    AddTerm(46, 22, ")");
    AddNonTerm(46, 29, 30);
    AddTerm(46, 29, ")");
    AddNonTerm(46, 34, 30);
    AddTerm(46, 34, ")");
    AddNonTerm(46, 35, 30);
    AddTerm(46, 35, ")");
    AddNonTerm(46, 36, 30);
    AddTerm(46, 36, ")");
    AddNonTerm(46, 37, 30);
    AddTerm(46, 37, ")");
    AddNonTerm(46, 38, 30);
    AddTerm(46, 38, ")");
    // TRY_CALL
    AddTerm(47, 9, "");
    AddTerm(47, 25, "");
    AddTerm(47, 26, "");
    AddTerm(47, 28, "");
    AddNonTerm(47, 29, 23);
    AddTerm(47, 31, "");
    AddTerm(47, 34, "");
    AddTerm(47, 45, "");
    AddTerm(47, 46, "");
    AddTerm(47, 47, "");
    AddTerm(47, 48, "");
    AddTerm(47, 49, "");
    AddTerm(47, 50, "");
    AddTerm(47, 51, "");
    AddTerm(47, 52, "");
    AddTerm(47, 53, "");
    AddTerm(47, 54, "");
}
#endif