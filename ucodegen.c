#include "ucodegen.h"

symtab * symboltable;
FILE * destFile;

//get hash value of identifer
int hash(char * string){
        int value = 0;
        while(*string != 0){
                value = value*11 + *string;
                string++;
        }
        return value%31;
}

//create symboltable
void initSymtab(){
        symboltable = (symtab *)malloc(sizeof(symtab));

        symboltable->level = 0;
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



void ucodegen(astNode * root, FILE * dest){

        astNode * cur;

        initSymtab();

        dest = destFile;

        //add global vars and functions on symboltable;
        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == DCL){
                        //add var on symboltable;(set)
                        //level = 0, offset += index size or 1;

                }else if(cur->tokenNumber == FUNC_DEF){
                        //add fun spec on symboltable;

                }else{
                        //error;
                }
                cur = cur->nextNode;
        }

        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == FUNC_DEF){
                        //parse function detail;
                        //block -> symbol table;
                }
                cur = cur->nextNode;
        }

}
