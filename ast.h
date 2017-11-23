#include <stdio.h>
#include <stdlib.h>

typedef struct _ast{
        int tokenNumber;
        char * tokenValue;
        struct _ast * nextNode;
        struct _ast * subNode;
}astNode;

enum nodeNumber {
        PROGRAM, FUNCTION_DEF, DECLARATION, DCL_SPEC, FORMAL_PARAM_LIST, FUNCTION_HEADER, FUN_CALL,
        DECLARATION_LIST, INIT_DCL_LIST, IF_ST, WHILE_ST, RETURN_ST, SPEC_CONST, SPEC_INT, SPEC_VOID,
        ASSIGN_OP, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
        OR_OP, AND_OP, EQUAL_OP, NOTEQUAL_OP, GREAT_OP, LESS_OP, LESSE_OP, GREATE_OP,
        ADD_OP, SUB_OP, MUL_OP, DIV_OP, MOD_OP, MINUS_OP, NOT_OP, PRE_DEC_OP, PRE_INC_OP,
        POST_INC_OP, POST_DEC_OP, ACTUAL_PARAM_LIST, IDENT, NUMBER, STATEMENT_LIST, FUNCTION_NAME,
        DEC_INDEX, INDEX, COMPOUND_ST, PARAM_DCL, INIT_DCL
};

astNode * buildTree(int tokenNumber, astNode * firstNode);
astNode * expandNode(astNode * firstNode, astNode * lastNode);
astNode * createNode(int tokenNumber, char * tokenValue);
void printAST(astNode * node, int depth);
