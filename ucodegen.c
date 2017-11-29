#include "ucodegen.h"

symtab * symboltable;
FILE * destFile;

int level, offset;

//get hash value of identifer
int hash(char * string){
        unsigned int value = 0;
        while(*string != 0){
                value = value*11 + *string;
                string++;
        }
        return value%31;
}

//create symboltable
void initSymtab(){
        symboltable = (symtab *)malloc(sizeof(symtab));

        level=0;
        offset=0;

        symboltable->leveltop = 0;
        symboltable->leveltable[symboltable->leveltop] = &(symboltable->table[0]);
        symboltable->avail = &(symboltable->table[0]);
}

//print Symboltable (For test)
void printST(){
        printf("=====Symbol Table=====\n");
        printf("Addr\tname\thash\tqual\tspec\tlevel\toffset\tdim\tnewtSym\n");
        sym * cur = &(symboltable->table[0]);

        while(cur < symboltable->avail){
                //name, hashvalue, qual, spec, level, offset, dim

                printf("%x\t%s\t%d\t",cur ,cur->name, hash(cur->name));
                switch (cur->qual) {
                        case QUAL_NONE: printf("Q_NONE\t"); break;
                        case QUAL_FUNC: printf("Q_FUNC\t"); break;
                        case QUAL_PARA: printf("Q_PARA\t"); break;
                        case QUAL_CONST: printf("Q_CONST\t"); break;
                        case QUAL_VAR: printf("Q_VAR\t"); break;
                        default : printf("\t"); break;
                }
                switch (cur->spec) {
                        case SPEC_NONE: printf("NONE\t"); break;
                        case SPEC_VOID: printf("VOID\t"); break;
                        case SPEC_INT: printf("INT\t"); break;
                        default : printf("\t"); break;
                }
                printf("%d\t%d\t%d\t%x\n",cur->level, cur->offset, cur->dim, cur->nextSym);
                cur++;
        }
}

//insert symbol on symboltable;
void insertSymbol(char * name, Qualifier qual, Specifier spec, int level, int offset, int dim){
        int h;
        sym * nextsym;
        sym * newsym;

        //get hash of ident.
        h = hash(name);

        //create & insert symbol table on top(avail)
        newsym = symboltable->avail;
        newsym->name = name;
        newsym->qual = qual;
        newsym->spec = spec;
        newsym->level = level;
        newsym->offset = offset;

        //reconstract hashtable;
        newsym->nextSym = symboltable->hashtable[h];
        symboltable->hashtable[h]=newsym;

        //move avail;
        symboltable->avail += 1;
}

sym * findtable(char * name){
        int h = hash(name);
        sym * cur = symboltable->hashtable[h];
        while(cur){
                if(!strcmp(name, cur->name)){
                        return cur;
                }
                cur = cur->nextSym;
        }
        return 0;
}

void dclfunchead(astNode * cur){
        int argunum = 0;
        astNode *p,  *q;
        Specifier spec = SPEC_NONE;

        assert(cur->tokenNumber == FUNC_HEAD);
        cur = cur->subNode;
        //get funtion return type from DCL_SPEC
        p = cur->subNode;
        while(p){
                switch (p->tokenNumber) {
                        case INT_NODE : spec = SPEC_INT; break;
                        case VOID_NODE : spec = SPEC_VOID; break;
                        default : break;
                }
                p = p->nextNode;
        }

        //dim = argu numbers
        q = cur->nextNode->nextNode;
        q = q->subNode;
        while(q){
                argunum++;
                q=q->nextNode;
        }
        //insert function name on symbol Table
        insertSymbol(cur->nextNode->tokenValue, QUAL_FUNC, spec, level, offset, argunum);

        offset++;
}
void dclsimpleVar(astNode * simplevar, Specifier spec, Qualifier qual){
        int sign = 1;

        astNode *p = simplevar->subNode;        //var name;
        astNode *q = simplevar->nextNode;       //init var;

        assert(simplevar->tokenNumber == SIMPLE_VAR);

        if(qual == QUAL_CONST){
                int init;
                if(q == NULL){
                        printf("%s is constant, but not have value", p->tokenValue);
                        exit(0);
                }
                if(q->tokenNumber == UNARY_MINUS){
                        sign = -1;
                        q = q->subNode;
                }
                init = sign * atoi(q->tokenValue);
                insertSymbol(p->tokenValue, qual, spec, level, offset, 0);
        } else {
                insertSymbol(p->tokenValue, qual, spec, level, offset, 0);
        }

        offset++;
}
void dclarrayVar(astNode * arrayvar, Specifier spec, Qualifier qual){
        astNode *p = arrayvar->subNode;
        int size;

        assert(arrayvar->tokenNumber == ARRAY_VAR);

        if(p->nextNode == NULL){
                printf("array size is not specified");
        } else {
                size = atoi(p->nextNode->tokenValue);
        }

        insertSymbol(p->tokenValue, qual, spec, level, offset, 1);
        offset += size;
}
void tokenDCL(astNode * cur){
        int typespec, typequal;
        astNode *p;
        astNode *q;

        typespec = SPEC_INT;
        typequal = QUAL_VAR;


        assert(cur->tokenNumber == DCL_SPEC);

        p = cur->subNode;
        while(p){
                switch (p->tokenNumber) {
                        case INT_NODE: typespec = SPEC_INT; break;
                        case CONST_NODE : typequal = QUAL_CONST; break;
                        default : break;
                }
                p = p->nextNode;
        }

        p = cur->nextNode;
        assert(p->tokenNumber == DCL_ITEM);
        while(p){
                q = p->subNode;
                switch (q->tokenNumber) {
                        case SIMPLE_VAR: dclsimpleVar(q, typespec, typequal); break;
                        case ARRAY_VAR : dclarrayVar(q, typespec, typequal); break;
                }
                p = p->nextNode;
        }
}

void resetSymbol(){
        sym * cursym = symboltable->avail;
        cursym-- ;
        symboltable->leveltop--;
        while(cursym > symboltable->leveltable[symboltable->leveltop]){
                //reset
                int h = hash(cursym->name);
                symboltable->hashtable[h] = cursym->nextSym;
                cursym--;
        }
        symboltable->avail = cursym;

}
void parseblock(astNode * compoundst){
        assert(compoundst->tokenNumber == COMPOUND_ST);
        astNode * p;

        //set var for symboltable
        level++;
        offset=0;

        symboltable->leveltable[symboltable->leveltop] = symboltable->avail;
        symboltable->leveltop++;

        //set symboltable
        p = compoundst->subNode->subNode;
        while(p){
                tokenDCL(p->subNode);
                p=p->nextNode;
        }

        //parse statement


        //reset symboltable

        printST();
        resetSymbol();
}

void ucodegen(astNode * root, FILE * dest){

        astNode * cur;

        initSymtab();

        dest = destFile;

        //add global vars and functions on symboltable;
        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == DCL){
                        tokenDCL(cur->subNode);
                }else if(cur->tokenNumber == FUNC_DEF){
                        dclfunchead(cur->subNode);
                }else{
                        //error;
                }
                cur = cur->nextNode;
        }

        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == FUNC_DEF){
                        parseblock(cur->subNode->nextNode);
                }
                cur = cur->nextNode;
        }

        printST();
}