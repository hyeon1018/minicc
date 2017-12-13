#include "ucodegen.h"

void parsestatments(astNode * block);

symtab * symboltable;
FILE * destFile;

int level, offset;
int printsyt;

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

        level=1;
        offset=1;

        symboltable->leveltop = 0;
        symboltable->avail = &(symboltable->table[0]);
        symboltable->leveltable[symboltable->leveltop] = symboltable->avail;

        for(int i = 0 ; i < HASH_SIZE ; i++){
                symboltable->hashtable[i]=NULL;
        }
}

//print Symboltable (For test)
void printST(){
        if(printsyt){
                printf("=====Symbol Table=====\n");
                printf("Addr\t%-15s\thash\tqual\tspec\tlevel\toffset\twidth\tinit\tnewtSym\n", "name");
                sym * cur = &(symboltable->table[0]);

                while(cur < symboltable->avail){
                        //name, hashvalue, qual, spec, level, offset, width

                        printf("%x\t%-15s\t%d\t",cur ,cur->name, hash(cur->name));
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
                        printf("%d\t%d\t%d\t%d\t%x\n",cur->level, cur->offset, cur->width,cur->init, cur->nextSym);
                        cur++;
                }
        }
}

//symboltable;
void insertSymbol(char * name, Qualifier qual, Specifier spec, int level, int offset, int width, int init){
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
        newsym->width = width;
        newsym->init = init;

        //reconstract hashtable;
        newsym->nextSym = symboltable->hashtable[h];
        symboltable->hashtable[h]=newsym;

        //move avail;
        symboltable->avail += 1;
}
void resetSymbol(){
        sym * cursym = symboltable->avail;
        symboltable->leveltop--;
        cursym--;
        while(cursym >= symboltable->leveltable[symboltable->leveltop]){
                //reset
                int h = hash(cursym->name);
                symboltable->hashtable[h] = cursym->nextSym;
                cursym--;
        }
        symboltable->avail = cursym+1;

}
sym * findSymbol(char * name){
        int h = hash(name);
        sym * cur = symboltable->hashtable[h];
        while(cur){
                if(!strcmp(name, cur->name)){
                        return cur;
                }
                cur = cur->nextSym;
        }
        return -1;
}

//printucode
void print0op(char *opcode){
    fprintf(destFile, "%-10s %s\n","", opcode);
}
void print1op(char *opcode, int operand){
    fprintf(destFile, "%-10s %-7s %d\n","", opcode, operand);
}
void print2op(char *opcode, int operand1, int operand2){
    fprintf(destFile, "%-10s %-7s %-7d %d\n","",  opcode, operand1, operand2);
}
void printjumpop(char *opcode, char *operand){
    fprintf(destFile, "%-10s %-7s %s\n", "", opcode, operand);
}
void printsym(int operand1, int operand2, int operand3){
    fprintf(destFile, "%-10s %-7s %-7d %-7d %d\n","", "sym", operand1, operand2, operand3);
}
void printproc(char *label, int proc1, int proc2, int proc3){
    fprintf(destFile, "%-10s %-7s %-7d %-7d %d\n", label, "proc", proc1, proc2, proc3);
}
void printlabel(char *label){
        fprintf(destFile, "%-10s nop\n", label);
}
void getlabel(char *label){
        static int labelnum = 0;
        sprintf(label, "$$%d", labelnum++);
}
void printrightvalue(astNode * rightvalue){
        sym * symbol;

        if(rightvalue->tokenNumber == NUMBER){
                print1op("ldc", atoi(rightvalue->tokenValue));
        }else{
                symbol = findSymbol(rightvalue->tokenValue);
                if(symbol != -1){
                        if(symbol->qual == QUAL_CONST){ //상수
                                print1op("ldc", symbol->init);
                        }else if(symbol->width > 1){
                                print2op("lda", symbol->level, symbol->offset);
                        }else{
                                print2op("lod", symbol->level, symbol->offset);
                        }
                }
        }
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
        //width = argu numbers
        q = cur->nextNode->nextNode;
        q = q->subNode;
        while(q){
                argunum++;
                q=q->nextNode;
        }
        //insert function name on symbol Table
        insertSymbol(cur->nextNode->tokenValue, QUAL_FUNC, spec, level, 0, argunum, 0);

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
                insertSymbol(p->tokenValue, qual, spec, level, offset, 1, init);
        } else {
                insertSymbol(p->tokenValue, qual, spec, level, offset, 1, 0);
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

        insertSymbol(p->tokenValue, qual, spec, level, offset, size, 0);
        offset += size;
}
void dclparam(astNode * formalparam){
        //declaration formal params
        astNode * p=formalparam->subNode; // p = param dcl;

        while(p){
                astNode * s = p->subNode->subNode;
                Specifier typespec = SPEC_NONE;
                Qualifier typequal = QUAL_PARA;

                while(s){
                        switch (s->tokenNumber) {
                                case INT_NODE: typespec = SPEC_INT; break;
                                case CONST_NODE : typequal = QUAL_CONST; break;
                                default : break;
                        }
                        s = s->nextNode;
                }

                s = p->subNode->nextNode;
                switch (s->tokenNumber) {
                        case SIMPLE_VAR: dclsimpleVar(s, typespec, typequal); break;
                        case ARRAY_VAR : dclarrayVar(s, typespec, typequal); break;
                }



                p = p->nextNode;
        }



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

void parsefunction(astNode * func_def){
        assert(func_def->tokenNumber==FUNC_DEF);
        astNode * func_header = func_def->subNode;
        astNode * compoundst = func_header->nextNode;
        astNode * p;

        //symboltable
        level++;
        offset=1;
        symboltable->leveltable[symboltable->leveltop] = symboltable->avail;
        symboltable->leveltop++;
        //insert param dcl on symboltable;
        dclparam(func_header->subNode->nextNode->nextNode);

        //parse dcl;
        p = compoundst->subNode->subNode;
        while(p){
                tokenDCL(p->subNode);
                p=p->nextNode;
        }
        printST();

        printproc(func_header->subNode->nextNode->tokenValue ,offset-1, level, 2);
        for(sym * s = symboltable->leveltable[symboltable->leveltop-1]; s < symboltable->avail; s++){
                printsym(level, s->offset, s->width);
        }

        //parse statement
        p = compoundst->subNode->nextNode->subNode;
        while(p){
                parsestatments(p);
                p = p->nextNode;
        }


        print0op("end");

        //reset symboltable
        printST();
        resetSymbol();
}
void parseexpression(astNode * expression){
        static lvalue = 0;
        static expdepth = 0;
        expdepth++;
        switch (expression->tokenNumber) {
                case ASSIGN_OP: {
                        astNode * left = expression->subNode;
                        astNode * right = expression->subNode->nextNode;

                        if(left->subNode){
                                lvalue = 1;
                                parseexpression(left);
                                lvalue = 0;
                        }

                        if(right->subNode){
                                parseexpression(right);
                        }else{
                                printrightvalue(right);
                        }

                        if(left->subNode == NULL){
                                sym * symbol = findSymbol(left->tokenValue);
                                if(symbol != -1){
                                        print2op("str", symbol->level, symbol->offset);
                                }
                        }else{
                                print0op("sti");
                        }

                        break;
                }
                case ADD_ASSIGN:
                case SUB_ASSIGN:
                case MUL_ASSIGN:
                case DIV_ASSIGN:
                case MOD_ASSIGN: {
                        astNode * left = expression->subNode;
                        astNode * right = expression->subNode->nextNode;

                        if(left->subNode){
                                lvalue = 1;
                                parseexpression(left);
                                lvalue = 0;
                        }

                        if(left->subNode){
                                parseexpression(left);
                        }else{
                                printrightvalue(left);
                        }

                        if(right->subNode){
                                parseexpression(right);
                        }else{
                                printrightvalue(right);
                        }

                        switch(expression->tokenNumber){
                                case ADD_ASSIGN : print0op("add"); break;
                                case SUB_ASSIGN : print0op("sub"); break;
                                case MUL_ASSIGN : print0op("mult"); break;
                                case DIV_ASSIGN : print0op("div"); break;
                                case MOD_ASSIGN : print0op("mod"); break;
                        }

                        if(!left->subNode){
                                sym * symbol = findSymbol(left->tokenValue);
                                if(symbol != -1){
                                        print2op("str", symbol->level, symbol->offset);
                                }
                        }else{
                                print0op("sti");
                        }
                        break;
                }
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                case EQ:
                case NE:
                case GT:
                case LT:
                case GE:
                case LE:
                case LOGICAL_OR:
                case LOGICAL_AND:{
                        astNode * left = expression->subNode;
                        astNode * right = expression->subNode->nextNode;

                        if(left->subNode){
                                parseexpression(left);
                        }else{
                                printrightvalue(left);
                        }

                        if(right->subNode){
                                parseexpression(right);
                        }else{
                                printrightvalue(right);
                        }

                        switch(expression->tokenNumber){
                                case ADD : print0op("add"); break;
                                case SUB : print0op("sub"); break;
                                case MUL : print0op("mult"); break;
                                case DIV : print0op("div"); break;
                                case MOD : print0op("mod"); break;
                                case EQ : print0op("eq"); break;
                                case NE : print0op("ne"); break;
                                case GT : print0op("gt"); break;
                                case LT : print0op("lt"); break;
                                case GE : print0op("ge"); break;
                                case LE : print0op("le"); break;
                                case LOGICAL_AND : print0op("and"); break;
                                case LOGICAL_OR : print0op("or"); break;
                        }
                        break;
                }
                case UNARY_MINUS:
                case LOGICAL_NOT:{
                        astNode *p = expression->subNode;

                        if(p->subNode){
                                parseexpression(p);
                        }else{
                                printrightvalue(p);
                        }
                        switch(expression->tokenNumber){
                                case UNARY_MINUS : print0op("neg"); break;
                                case LOGICAL_NOT : print0op("not"); break;
                                default : break;
                        }
                        break;
                }
                case INDEX:{
                        astNode * indexp = expression->subNode->nextNode;
                        sym * symbol;

                        if(indexp->subNode){
                                parseexpression(indexp);
                        }else{
                                printrightvalue(indexp);
                        }
                        symbol = findSymbol(expression->subNode->tokenValue);
                        if(symbol != -1){
                                print2op("lda", symbol->level, symbol->offset);
                                print0op("add");
                        }
                        if(!lvalue){
                                print0op("ldi");
                        }

                        break;
                }
                case PRE_INC:
                case POST_INC:
                case PRE_DEC:
                case POST_DEC:{
                        astNode * p = expression->subNode;
                        //assert subNode == IDENT or INDEX;
                        if(p->subNode){
                                lvalue = 1;
                                parseexpression(p);
                                lvalue = 0;
                        }
                        if(p->subNode){
                                parseexpression(p);
                        }else{
                                printrightvalue(p);
                        }
                        switch(expression->tokenNumber){

                                case PRE_INC : {
                                        print0op("inc");
                                        if(expdepth > 1){
                                                print0op("dup");
                                        }
                                        break;
                                }
                                case POST_INC : {
                                        if(expdepth > 1){
                                                print0op("dup");
                                        }
                                        print0op("inc");
                                        break;
                                }
                                case PRE_DEC : {
                                        print0op("dec");
                                        if(expdepth > 1){
                                                print0op("dup");
                                        }
                                        break;
                                }
                                case POST_DEC : {
                                        if(expdepth > 1){
                                                print0op("dup");
                                        }
                                        print0op("dec");
                                        break;
                                }
                        }

                        if(!p->subNode){
                                sym * symbol = findSymbol(p->tokenValue);
                                if(symbol != -1){
                                        print2op("str", symbol->level, symbol->offset);
                                }
                        }else{
                                print0op("sti");
                        }
                        break;
                }
                case CALL:{
                        astNode * p = expression->subNode;
                        sym * symbol = findSymbol(p->tokenValue);

                        if(symbol != -1){
                                int argnum = 0;
                                print0op("ldp");
                                p = p->nextNode->subNode;
                                while(p){
                                        if(p->subNode){
                                                parseexpression(p);
                                        }else{
                                                printrightvalue(p);
                                        }
                                        argnum++;
                                        p = p->nextNode;
                                }

                                if(argnum = symbol->width){
                                        printjumpop("call", symbol->name);
                                }
                        }
                        break;
                }
                case IDENT_ADDR:{
                        //for call by addr;
                        //for subNode is only ident
                        if(expression->subNode){
                                sym * symbol;

                                switch(expression->subNode->tokenNumber){
                                        case IDENT : {
                                                symbol = findSymbol(expression->subNode->tokenValue);
                                                if(symbol != -1){
                                                        print2op("lda", symbol->level, symbol->offset);
                                                }
                                                break;
                                        }
                                        case INDEX : {
                                                astNode * indexp = expression->subNode->subNode->nextNode;

                                                if(indexp->subNode){
                                                        parseexpression(indexp);
                                                }else{
                                                        printrightvalue(indexp);
                                                }

                                                symbol = findSymbol(expression->subNode->subNode->tokenValue);
                                                if(symbol != -1){
                                                        print2op("lda", symbol->level, symbol->offset);
                                                        print0op("add");
                                                }
                                                break;
                                        }
                                        default : printf("can't get address of expression"); break;

                                }
                                break;
                        }
                        default : break;
                }
        }
        expdepth--;
}
void parsestatments(astNode * statement){
        astNode * cur;
        char label1[10]={0};
        char label2[10]={0};
        switch (statement->tokenNumber) {
                case EXP_ST : {
                        if(statement->subNode){
                                parseexpression(statement->subNode);
                        }
                        break;
                }
                case COMPOUND_ST : {
                        char label[10] = {0};
                        level++;
                        offset=1;
                        symboltable->leveltable[symboltable->leveltop] = symboltable->avail;
                        symboltable->leveltop++;
                        //set symboltable
                        cur = statement->subNode->subNode;
                        while(cur){
                                tokenDCL(cur->subNode);
                                cur=cur->nextNode;
                        }
                        //parse statement
                        cur = statement->subNode->nextNode->subNode;
                        while(cur){
                                parsestatments(cur);
                                cur = cur->nextNode;
                        }

                        //reset symboltable
                        printST();
                        resetSymbol();

                        break;
                }
                case IF_ST :{
                        getlabel(label1);
                        parseexpression(statement->subNode);
                        printjumpop("fjp", label1);
                        parsestatments(statement->subNode->nextNode);
                        printlabel(label1);
                        break;
                }
                case IF_ELSE_ST : {
                        getlabel(label1);
                        getlabel(label2);
                        parseexpression(statement->subNode);
                        printjumpop("fjp", label1);
                        parsestatments(statement->subNode->nextNode);
                        printjumpop("ujp", label2);
                        printlabel(label1);
                        parsestatments(statement->subNode->nextNode->nextNode);
                        printlabel(label2);
                        break;
                }
                case WHILE_ST : {
                        getlabel(label1);
                        getlabel(label2);
                        printlabel(label1);
                        parseexpression(statement->subNode);
                        printjumpop("fjp", label2);
                        parsestatments(statement->subNode->nextNode);
                        printjumpop("ujp", label1);
                        printlabel(label2);
                        break;
                }
                case RETURN_ST : {
                        if(statement->subNode){
                                astNode * p = statement->subNode;
                                if(p->subNode){
                                        parseexpression(p);
                                }else{
                                        printrightvalue(p);
                                }
                                print0op("retv");
                        }else{
                                print0op("ret");
                        }
                        break;
                }
                default : break; //?????
        }



}

void ucodegen(astNode * root, FILE * dest, int printsy){

        astNode * cur;

        initSymtab();
        destFile = dest;
        printsyt = printsy;

        //add global vars and functions on symboltable;
        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == DCL){
                        tokenDCL(cur->subNode);
                }else if(cur->tokenNumber == FUNC_DEF){
                        dclfunchead(cur->subNode);
                        printST();
                }else{
                        //error;
                }
                cur = cur->nextNode;
        }

        //add predefined function in symboltable;
        insertSymbol("write", QUAL_FUNC, SPEC_VOID, level, 0, 1, 0);
        insertSymbol("read", QUAL_FUNC, SPEC_VOID, level, 0, 1, 0);

        cur = root->subNode;
        while(cur){
                if(cur->tokenNumber == FUNC_DEF){
                        parsefunction(cur);
                }
                cur = cur->nextNode;
        }

        printST();

        print1op("bgn", offset);
        print0op("ldp");
        printjumpop("call", "main");
        print0op("end");
}
