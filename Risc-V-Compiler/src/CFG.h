#ifndef CFG_h
#define CFG_h

const char *NonTerms[] = {"uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int", "char", "bool", "void", "", "while", "for", "if", "IDENTIFIER", "return", "break", "}", "*", ")", "[", ";", "(", "=", ",", "else", "{", "-", "~", "&", "!", "LITERAL", "+=", "-=", "*=", "/=", "^=", "%%="};
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
#define EXPRESSION 25
#define TYPE_TERM 26
#define IDENTIFIER_TERM 27
#define LITERAL_TERM 28

#define MALLOC_LABEL (uint32_t)(-1)
#define FREE_LABEL (uint32_t)(-2)
#define MEM_READ (uint32_t)(-3)
#define MEM_WRITE (uint32_t)(-4)
#define PIN_OUT (uint32_t)(-5)
#define PIN_IN (uint32_t)(-6)
#define PIN_READ (uint32_t)(-7)
#define PIN_WRITE (uint32_t)(-8)
#define PIN_TOGGLE (uint32_t)(-9)

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
        "e" -> ""
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
    AddTerm(2, 16, "");
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
    AddTerm(3, 16, "");
    // DECLERATION
    AddNonTerm(4, 0, 26);
    AddNonTerm(4, 0, 5);
    AddNonTerm(4, 0, 27);
    AddNonTerm(4, 0, 6);
    AddNonTerm(4, 0, 7);
    AddNonTerm(4, 1, 26);
    AddNonTerm(4, 1, 5);
    AddNonTerm(4, 1, 27);
    AddNonTerm(4, 1, 6);
    AddNonTerm(4, 1, 7);
    AddNonTerm(4, 2, 26);
    AddNonTerm(4, 2, 5);
    AddNonTerm(4, 2, 27);
    AddNonTerm(4, 2, 6);
    AddNonTerm(4, 2, 7);
    AddNonTerm(4, 3, 26);
    AddNonTerm(4, 3, 5);
    AddNonTerm(4, 3, 27);
    AddNonTerm(4, 3, 6);
    AddNonTerm(4, 3, 7);
    AddNonTerm(4, 4, 26);
    AddNonTerm(4, 4, 5);
    AddNonTerm(4, 4, 27);
    AddNonTerm(4, 4, 6);
    AddNonTerm(4, 4, 7);
    AddNonTerm(4, 5, 26);
    AddNonTerm(4, 5, 5);
    AddNonTerm(4, 5, 27);
    AddNonTerm(4, 5, 6);
    AddNonTerm(4, 5, 7);
    AddNonTerm(4, 6, 26);
    AddNonTerm(4, 6, 5);
    AddNonTerm(4, 6, 27);
    AddNonTerm(4, 6, 6);
    AddNonTerm(4, 6, 7);
    AddNonTerm(4, 7, 26);
    AddNonTerm(4, 7, 5);
    AddNonTerm(4, 7, 27);
    AddNonTerm(4, 7, 6);
    AddNonTerm(4, 7, 7);
    AddNonTerm(4, 8, 26);
    AddNonTerm(4, 8, 5);
    AddNonTerm(4, 8, 27);
    AddNonTerm(4, 8, 6);
    AddNonTerm(4, 8, 7);
    // POINTER
    AddTerm(5, 9, "");
    AddTerm(5, 13, "");
    AddTerm(5, 17, "*");
    AddNonTerm(5, 17, 5);
    AddTerm(5, 18, "");
    // ARRAY
    AddTerm(6, 9, "");
    AddTerm(6, 18, "");
    AddTerm(6, 19, "[");
    AddNonTerm(6, 19, 25);
    AddTerm(6, 19, "]");
    AddTerm(6, 20, "");
    AddTerm(6, 21, "");
    AddTerm(6, 22, "");
    AddTerm(6, 23, "");
    // DECREST
    AddNonTerm(7, 20, 8);
    AddNonTerm(7, 21, 9);
    AddNonTerm(7, 22, 8);
    // VAR_DEC_REST
    AddTerm(8, 20, ";");
    AddTerm(8, 22, "=");
    AddNonTerm(8, 22, 25);
    AddTerm(8, 22, ";");
    // FUNC_DEC_REST
    AddTerm(9, 21, "(");
    AddNonTerm(9, 21, 10);
    AddTerm(9, 21, ")");
    AddTerm(9, 21, "{");
    AddNonTerm(9, 21, 2);
    AddTerm(9, 21, "}");
    // FUNCTION_PARAMS
    AddNonTerm(10, 0, 26);
    AddNonTerm(10, 0, 5);
    AddNonTerm(10, 0, 27);
    AddNonTerm(10, 0, 6);
    AddNonTerm(10, 0, 11);
    AddNonTerm(10, 1, 26);
    AddNonTerm(10, 1, 5);
    AddNonTerm(10, 1, 27);
    AddNonTerm(10, 1, 6);
    AddNonTerm(10, 1, 11);
    AddNonTerm(10, 2, 26);
    AddNonTerm(10, 2, 5);
    AddNonTerm(10, 2, 27);
    AddNonTerm(10, 2, 6);
    AddNonTerm(10, 2, 11);
    AddNonTerm(10, 3, 26);
    AddNonTerm(10, 3, 5);
    AddNonTerm(10, 3, 27);
    AddNonTerm(10, 3, 6);
    AddNonTerm(10, 3, 11);
    AddNonTerm(10, 4, 26);
    AddNonTerm(10, 4, 5);
    AddNonTerm(10, 4, 27);
    AddNonTerm(10, 4, 6);
    AddNonTerm(10, 4, 11);
    AddNonTerm(10, 5, 26);
    AddNonTerm(10, 5, 5);
    AddNonTerm(10, 5, 27);
    AddNonTerm(10, 5, 6);
    AddNonTerm(10, 5, 11);
    AddNonTerm(10, 6, 26);
    AddNonTerm(10, 6, 5);
    AddNonTerm(10, 6, 27);
    AddNonTerm(10, 6, 6);
    AddNonTerm(10, 6, 11);
    AddNonTerm(10, 7, 26);
    AddNonTerm(10, 7, 5);
    AddNonTerm(10, 7, 27);
    AddNonTerm(10, 7, 6);
    AddNonTerm(10, 7, 11);
    AddNonTerm(10, 8, 26);
    AddNonTerm(10, 8, 5);
    AddNonTerm(10, 8, 27);
    AddNonTerm(10, 8, 6);
    AddNonTerm(10, 8, 11);
    AddTerm(10, 9, "");
    AddTerm(10, 18, "");
    // PARAM_REST
    AddTerm(11, 9, "");
    AddTerm(11, 18, "");
    AddTerm(11, 23, ",");
    AddNonTerm(11, 23, 10);
    // WHILE
    AddTerm(12, 10, "while");
    AddTerm(12, 10, "(");
    AddNonTerm(12, 10, 25);
    AddTerm(12, 10, ")");
    AddTerm(12, 10, "{");
    AddNonTerm(12, 10, 2);
    AddTerm(12, 10, "}");
    // FOR
    AddTerm(13, 11, "for");
    AddTerm(13, 11, "(");
    AddNonTerm(13, 11, 26);
    AddNonTerm(13, 11, 27);
    AddTerm(13, 11, "=");
    AddNonTerm(13, 11, 25);
    AddTerm(13, 11, ";");
    AddNonTerm(13, 11, 25);
    AddTerm(13, 11, ";");
    AddNonTerm(13, 11, 20);
    AddTerm(13, 11, ")");
    AddTerm(13, 11, "{");
    AddNonTerm(13, 11, 2);
    AddTerm(13, 11, "}");
    // IF
    AddTerm(14, 12, "if");
    AddTerm(14, 12, "(");
    AddNonTerm(14, 12, 25);
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
    AddTerm(15, 24, "else");
    AddNonTerm(15, 24, 16);
    // ELSE_REST
    AddNonTerm(16, 12, 14);
    AddTerm(16, 25, "{");
    AddNonTerm(16, 25, 2);
    AddTerm(16, 25, "}");
    // BREAK
    AddTerm(17, 15, "break");
    AddTerm(17, 15, ";");
    // RETURN
    AddTerm(18, 14, "return");
    AddNonTerm(18, 14, 19);
    AddTerm(18, 14, ";");
    // RETURN_VAL
    AddTerm(19, 9, "");
    AddNonTerm(19, 13, 25);
    AddNonTerm(19, 17, 25);
    AddTerm(19, 20, "");
    AddNonTerm(19, 21, 25);
    AddNonTerm(19, 26, 25);
    AddNonTerm(19, 27, 25);
    AddNonTerm(19, 28, 25);
    AddNonTerm(19, 29, 25);
    AddNonTerm(19, 30, 25);
    // ASSIGN
    AddNonTerm(20, 13, 27);
    AddNonTerm(20, 13, 21);
    AddTerm(20, 13, ";");
    // ASSIGN_OR_CALL
    AddNonTerm(21, 21, 23);
    AddNonTerm(21, 22, 22);
    AddNonTerm(21, 31, 22);
    AddNonTerm(21, 32, 22);
    AddNonTerm(21, 33, 22);
    AddNonTerm(21, 34, 22);
    AddNonTerm(21, 35, 22);
    AddNonTerm(21, 36, 22);
    // ASSIGN_REST
    AddTerm(22, 22, "=");
    AddNonTerm(22, 22, 25);
    AddTerm(22, 31, "+=");
    AddNonTerm(22, 31, 25);
    AddTerm(22, 32, "-=");
    AddNonTerm(22, 32, 25);
    AddTerm(22, 33, "*=");
    AddNonTerm(22, 33, 25);
    AddTerm(22, 34, "/=");
    AddNonTerm(22, 34, 25);
    AddTerm(22, 35, "^=");
    AddNonTerm(22, 35, 25);
    AddTerm(22, 36, "%%=");
    AddNonTerm(22, 36, 25);
    // CALL
    AddTerm(23, 21, "");
    AddTerm(23, 21, "(");
    AddNonTerm(23, 21, 24);
    AddTerm(23, 21, ")");
    // CALL_PARAM
    AddNonTerm(24, 13, 25);
    AddTerm(24, 18, "");
    AddNonTerm(24, 21, 25);
    AddTerm(24, 23, ",");
    AddNonTerm(24, 23, 25);
    AddNonTerm(24, 26, 25);
    AddNonTerm(24, 27, 25);
    AddNonTerm(24, 28, 25);
    AddNonTerm(24, 29, 25);
    AddNonTerm(24, 30, 25);
    // EXPRESSION
    // TYPE
    AddTerm(26, 0, "uint8_t");
    AddTerm(26, 1, "uint16_t");
    AddTerm(26, 2, "uint32_t");
    AddTerm(26, 3, "int8_t");
    AddTerm(26, 4, "int16_t");
    AddTerm(26, 5, "int");
    AddTerm(26, 6, "char");
    AddTerm(26, 7, "bool");
    AddTerm(26, 8, "void");
    // IDENTIFIER
    // LITERAL
}
#endif