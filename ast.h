#include <stdio.h>
#include <stdlib.h>

typedef struct _ast{
        int tokenNumber;
        char * tokenValue;
        struct _ast * nextNode;
        struct _ast * subNode;
}astNode;

enum nodeNumber {
        PROGRAM, FUNC_DEF, FUNC_HEAD, DCL_SPEC, CONST_NODE,
        INT_NODE, VOID_NODE, FORMAL_PARAM, PARAM_DCL, COMPOUND_ST,
        DCL_LIST, DCL, DCL_ITEM, SIMPLE_VAR, ARRAY_VAR,
        STAT_LIST, EXP_ST, IF_ST, IF_ELSE_ST, WHILE_ST,
        RETURN_ST, ASSIGN_OP, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN,
        DIV_ASSIGN, MOD_ASSIGN, LOGICAL_OR, LOGICAL_AND, EQ,
        NE, GT, LT, GE, LE,
        ADD, SUB, MUL, DIV, MOD,
        UNARY_MINUS, LOGICAL_NOT, PRE_INC, PRE_DEC, INDEX,
        CALL, POST_INC, POST_DEC, ACTUAL_PARAM, NUMBER,
        IDENT, IDENT_ADDR
};

astNode * buildTree(int tokenNumber, astNode * firstNode);
astNode * expandNode(astNode * firstNode, astNode * lastNode);
astNode * createNode(int tokenNumber, char * tokenValue);
void printAST(astNode * node, int depth);
