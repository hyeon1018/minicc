#include "ast.h"

astNode * buildTree(int tokenNumber, astNode * firstNode){
        astNode * newNode;

        newNode = (astNode *)malloc(sizeof(astNode));
        newNode->tokenValue = NULL;
        newNode->tokenNumber = tokenNumber;
        newNode->subNode = firstNode;
        newNode->nextNode = NULL;

        return newNode;
}


astNode * expandNode(astNode * firstNode, astNode * lastNode){
        astNode * node = firstNode;

        while(NULL != node->nextNode){
                node = node->nextNode;
        }
        node->nextNode=lastNode;

        return firstNode;
}

astNode * createNode(int tokenNumber, char * tokenValue){
        astNode * newNode;

        newNode = (astNode *)malloc(sizeof(astNode));
        newNode->tokenNumber = tokenNumber;
        newNode->tokenValue = tokenValue;
        newNode->subNode = NULL;
        newNode->nextNode = NULL;
}


void printAST(astNode * node, int depth){
        //print self
        printf("-");
        for(int i = 0 ; i < depth ; i++){
                //indepth
                printf("\t");
        }
        switch (node->tokenNumber){
                case PROGRAM : printf("PROGRAM"); break;
                case FUNCTION_DEF : printf("FUNCTION_DEF"); break;
                case DECLARATION : printf("DECLARATION"); break;
                case DCL_SPEC : printf("DCL_SPEC"); break;
                case FORMAL_PARAM_LIST : printf("FORMAL_PARAM_LIST"); break;
                case DECLARATION_LIST : printf("DECLARATION_LIST"); break;
                case INIT_DCL_LIST : printf("INIT_DCL_LIST"); break;
                case IF_ST  : printf("IF_ST"); break;
                case WHILE_ST : printf("WHILE_ST"); break;
                case RETURN_ST : printf("RETURN_ST"); break;
                case SPEC_CONST : printf("SPEC_CONST"); break;
                case SPEC_INT : printf("SPEC_INT"); break;
                case SPEC_VOID : printf("SPEC_VOID"); break;
                case ASSIGN_OP : printf("ASSIGN_OP"); break;
                case ADD_ASSIGN : printf("ADD_ASSIGN"); break;
                case SUB_ASSIGN : printf("SUB_ASSIGN"); break;
                case MUL_ASSIGN : printf("MUL_ASSIGN"); break;
                case DIV_ASSIGN : printf("DIV_ASSIGN"); break;
                case MOD_ASSIGN : printf("MOD_ASSIGN"); break;
                case OR_OP : printf("OR_OP"); break;
                case AND_OP : printf("AND_OP"); break;
                case EQUAL_OP : printf("EQUAL_OP"); break;
                case NOTEQUAL_OP : printf("NOTEQUAL_OP"); break;
                case GREAT_OP : printf("GREAT_OP"); break;
                case LESS_OP : printf("LESS_OP"); break;
                case LESSE_OP : printf("LESSE_OP"); break;
                case GREATE_OP : printf("GREATE_OP"); break;
                case ADD_OP  : printf("ADD_OP"); break;
                case SUB_OP : printf("SUB_OP"); break;
                case MUL_OP : printf("MUL_OP"); break;
                case DIV_OP : printf("DIV_OP"); break;
                case MOD_OP : printf("MOD_OP"); break;
                case MINUS_OP : printf("MINUS_OP"); break;
                case NOT_OP : printf("NOT_OP"); break;
                case PRE_DEC_OP : printf("PRE_DEC_OP"); break;
                case PRE_INC_OP : printf("PRE_INC_OP"); break;
                case POST_INC_OP : printf("POST_INC_OP"); break;
                case POST_DEC_OP : printf("POST_DEC_OP"); break;
                case ACTUAL_PARAM_LIST : printf("ACTUAL_PARAM_LIST"); break;
                case IDENT : printf("IDENT"); break;
                case NUMBER : printf("NUMBER"); break;
                case STATEMENT_LIST : printf("STATEMENT_LIST"); break;
                case FUNCTION_NAME : printf("FUNCTION_NAME"); break;
                case DEC_INDEX : printf("DEC_INDEX"); break;
                case FUNCTION_HEADER : printf("FUNCTION_HEADER"); break;
                case FUN_CALL : printf("FUN_CALL"); break;
                case INDEX : printf("INDEX"); break;
                case COMPOUND_ST : printf("COMPOUND_ST"); break;
                case PARAM_DCL : printf("PARAM_DCL"); break;
                case INIT_DCL : printf("INIT_DCL"); break;
                default : printf("%d",node->tokenNumber); break;
        }

        if(node->tokenValue != NULL){
                printf("\t -> %s",node->tokenValue);
        }
        printf("\n");
        //subNode
        if(node->subNode != NULL){
                printAST(node->subNode, depth+1);
        }
        //nextNode
        if(node->nextNode != NULL){
                printAST(node->nextNode, depth);
        }
}
