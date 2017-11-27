#define HASH_SIZE 31
#define STACK_SIZE 16
#define TABLE_SIZE 100

#include <string.h>
#include "ast.h"


typedef enum {
    SPEC_NONE, SPEC_VOID, SPEC_INT
} Specifier;

typedef enum {
    QUAL_NONE, QUAL_FUNC, QUAL_PARA, QUAL_CONST, QUAL_VAR
} Qualifier;

typedef struct _symbol{
        char * name;
        Qualifier qual;
        Specifier spec;
        int level;
        int offset;
        int dim;
        struct _symbol * nextSym;
}sym;

typedef struct _symboltable{
        struct _symbol * hashtable[HASH_SIZE];
        struct _symbol table[TABLE_SIZE];
        struct _symbol * leveltable[STACK_SIZE];
        int level;
        struct _symbol * avail;
}symtab;

void ucodegen(astNode * root, FILE * dest);
