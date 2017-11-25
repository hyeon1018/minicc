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
        printf("%d",node->tokenNumber);

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
