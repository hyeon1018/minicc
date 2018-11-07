%{
        #include <stdio.h>
        #include "ast.h"
        #include "ucodegen.h"

        int yyerror(char *s);
        int yylex();

        extern FILE *yyin;
        astNode * root;
%}

%union{
  struct _ast * node;
  char * tval;
}


%token T_CONST T_ELSE T_IF T_INT T_RETURN T_VOID T_WHILE T_EQUAL T_NOTEQUAL T_LESSE T_GREATE T_AND T_OR T_INC T_DEC T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN;
%token <tval>  T_IDENT T_NUMBER;
%nonassoc T_ELSE;
%type <node> function_def translation_unit external_dcl function_name function_header compound_st
    dcl_spec formal_param opt_dcl_list declaration_list declaration dcl_specifiers dcl_specifier
    type_specifier type_qualifier opt_formal_param formal_param_list param_dcl init_dcl_list init_declarator
    declarator opt_number opt_stat_list statement_list statement expression_st opt_expression if_st
    while_st return_st expression assignment_exp actual_param actual_param_list unary_exp postfix_exp primary_exp
    logical_or_exp logical_and_exp equality_exp relational_exp additive_exp multiplicative_exp opt_actual_param
%%

mini_c :
	translation_unit{
        root = buildTree(PROGRAM, $1);
	return 0;
	};
translation_unit :
	external_dcl {
                $$ = $1;
        }|
	translation_unit external_dcl{
                $$ = expandNode($1, $2);
        };
external_dcl :
	function_def{
                $$ = $1;
        }  |
	declaration{
                $$ = $1;
        };
function_def :
	function_header compound_st{
                expandNode($1, $2);
                $$ = buildTree(FUNC_DEF, $1);
        };
function_header :
	dcl_spec function_name formal_param{
                expandNode($1, $2);
                expandNode($1, $3);
                $$ = buildTree(FUNC_HEAD, $1);
        };
dcl_spec :
	dcl_specifiers{
                $$ = buildTree(DCL_SPEC, $1);
        };
dcl_specifiers :
	dcl_specifier {
                $$ = $1;
        } |
	dcl_specifiers dcl_specifier {
                $$ = expandNode($1, $2);
        };
dcl_specifier :
	type_qualifier {
                $$ = $1;
        } |
	type_specifier 	{
                $$ = $1;
        };
type_qualifier :
	T_CONST {
                $$ = createNode(CONST_NODE, NULL);
        };
type_specifier :
	T_INT {
                $$ = createNode(INT_NODE, NULL);
        } |
	T_VOID {
                $$ = createNode(VOID_NODE, NULL);
        };
function_name :
	T_IDENT {
                $$ = createNode(IDENT, $1);
        };
formal_param :
	'(' opt_formal_param ')'{
                $$ = buildTree(FORMAL_PARAM, $2);
        };
opt_formal_param :
        {  $$ = NULL; }
	| formal_param_list{
                $$ = $1;
        };
formal_param_list :
	param_dcl  {
                $$ = $1;
        }|
	formal_param_list ',' param_dcl	{
                $$ = expandNode($1, $3);
        };
param_dcl :
	dcl_spec declarator{
                expandNode($1, $2);
                $$ = buildTree(PARAM_DCL, $1);
        };
compound_st :
	'{' opt_dcl_list opt_stat_list '}'{
                expandNode($2, $3);
                $$ = buildTree(COMPOUND_ST, $2);
        };
opt_dcl_list :
        { $$ = createNode(DCL_LIST, NULL); }
	 | declaration_list{

                 $$ = buildTree(DCL_LIST, $1);
         };
declaration_list :
	declaration {
                $$ = $1;
        } |
	declaration_list declaration{
                $$ = expandNode($1, $2);
        };
declaration :
	dcl_spec init_dcl_list ';'{
                expandNode($1, $2);
                $$ = buildTree(DCL, $1);
        };
init_dcl_list :
	init_declarator {
                $$ = $1;
        }|
	init_dcl_list ',' init_declarator{
                $$ = expandNode($1, $3);
        };
init_declarator :
	declarator {
                $$ = buildTree(DCL_ITEM, $1);
        } |
	declarator '=' T_NUMBER {
                astNode * tmp = createNode(NUMBER, $3);
                expandNode($1, tmp);
                $$ = buildTree(DCL_ITEM, $1);
        };
declarator :
	T_IDENT {
                $$ = buildTree(SIMPLE_VAR, createNode(IDENT, $1));
        }|
	T_IDENT '[' opt_number ']'{
                astNode * tmp = createNode(IDENT, $1);
                expandNode(tmp, $3);
                $$ = buildTree(ARRAY_VAR, tmp);
        };
opt_number :
        { $$ = NULL; }
	 | T_NUMBER{
                 $$ = createNode(NUMBER, $1);
         };
opt_stat_list :
        { $$ = NULL; }
	 | statement_list{
                 $$ = buildTree(STAT_LIST, $1);
         };
statement_list :
	statement {
                $$ = $1;
        }|
	statement_list statement{
                $$ = expandNode($1, $2);
        };
statement :
	compound_st  {
                $$= $1;
        }|
	expression_st {
                $$= $1;
        } |
	if_st  {
                $$= $1;
        }|
	while_st  {
                $$= $1;
        }|
	return_st{
                $$= $1;
        };
expression_st :
	opt_expression ';'{
                $$= buildTree(EXP_ST, $1);
        };
opt_expression :
        { $$ = NULL; }
	 | expression {
                 $$ = $1;
         };
if_st :
	T_IF '(' expression ')' statement T_ELSE statement {
                expandNode($3, $5);
                expandNode($3, $7);
                $$ = buildTree(IF_ELSE_ST, $3);
        }|
	T_IF '(' expression ')' statement{
                expandNode($3, $5);
                $$ = buildTree(IF_ST, $3);
        };
while_st :
	T_WHILE '(' expression ')' statement{
                expandNode($3, $5);
                $$ = buildTree(WHILE_ST, $3);
        };
return_st :
	T_RETURN opt_expression ';'{
                $$ = buildTree(RETURN_ST, $2);
        };
expression :
	assignment_exp{
                $$ = $1;
        };
assignment_exp :
	logical_or_exp  {
                $$ = $1;
        }|
	unary_exp '=' assignment_exp  {
                expandNode($1, $3);
                $$ = buildTree(ASSIGN_OP, $1);
        }|
	unary_exp T_ADD_ASSIGN assignment_exp {
                expandNode($1, $3);
                $$ = buildTree(ADD_ASSIGN, $1);
        } |
	unary_exp T_SUB_ASSIGN assignment_exp {
                expandNode($1, $3);
                $$ = buildTree(SUB_ASSIGN, $1);
        } |
	unary_exp T_MUL_ASSIGN assignment_exp  {
                expandNode($1, $3);
                $$ = buildTree(MUL_ASSIGN, $1);
        }|
	unary_exp T_DIV_ASSIGN assignment_exp  {
                expandNode($1, $3);
                $$ = buildTree(DIV_ASSIGN, $1);
        }|
	unary_exp T_MOD_ASSIGN assignment_exp{
                expandNode($1, $3);
                $$ = buildTree(MOD_ASSIGN, $1);
        };
logical_or_exp :
	logical_and_exp {
                $$ = $1;
        } |
	logical_or_exp T_OR logical_and_exp{
                expandNode($1, $3);
                $$ = buildTree(LOGICAL_OR, $1);
        };
logical_and_exp :
	equality_exp   {
                $$ = $1;
        }|
	logical_and_exp T_AND equality_exp{
                expandNode($1, $3);
                $$ = buildTree(LOGICAL_AND, $1);
        };
equality_exp :
	relational_exp  {
                $$ = $1;
        }|
	equality_exp T_EQUAL relational_exp {
                expandNode($1, $3);
                $$ = buildTree(EQ, $1);
        } |
	equality_exp T_NOTEQUAL relational_exp{
                expandNode($1, $3);
                $$ = buildTree(NE, $1);
        };
relational_exp :
	additive_exp  {
                $$ = $1;
        }|
	relational_exp '>' additive_exp  {
                expandNode($1, $3);
                $$ = buildTree(GT, $1);
        }|
	relational_exp '<' additive_exp  {
                expandNode($1, $3);
                $$ = buildTree(LT, $1);
        }|
	relational_exp T_LESSE additive_exp  {
                expandNode($1, $3);
                $$ = buildTree(LE, $1);
        }|
	relational_exp T_GREATE additive_exp{
                expandNode($1, $3);
                $$ = buildTree(GE, $1);
        };
additive_exp :
	multiplicative_exp  {
                $$ = $1;
        }|
	additive_exp '+' multiplicative_exp  {
                expandNode($1, $3);
                $$ = buildTree(ADD, $1);
        }|
	additive_exp '-' multiplicative_exp{
                expandNode($1, $3);
                $$ = buildTree(SUB, $1);
        };
multiplicative_exp :
	unary_exp  {
                $$ = $1;
        }|
	multiplicative_exp '*' unary_exp  {
                expandNode($1, $3);
                $$ = buildTree(MUL, $1);
        }|
	multiplicative_exp '/' unary_exp  {
                expandNode($1, $3);
                $$ = buildTree(DIV, $1);
        }|
	multiplicative_exp '%' unary_exp{
                expandNode($1, $3);
                $$ = buildTree(MOD, $1);
        };
unary_exp :
	postfix_exp  {
                $$ = $1;
        }|
	'-' unary_exp {
                $$ = buildTree(UNARY_MINUS, $2);
        } |
	'!' unary_exp {
                $$ = buildTree(LOGICAL_NOT, $2);
        } |
	T_INC unary_exp {
                $$ = buildTree(PRE_INC, $2);
        } |
	T_DEC unary_exp{
                $$ = buildTree(PRE_DEC, $2);
        } |
        '&' unary_exp{
                $$ = buildTree(IDENT_ADDR, $2);
        };
postfix_exp :
	primary_exp  {
                $$ = $1;
        }|
	postfix_exp '[' expression ']'  {
                expandNode($1, $3);
                $$ = buildTree(INDEX, $1);
        }|
	postfix_exp '(' opt_actual_param ')' {
                expandNode($1, $3);
                $$ = buildTree(CALL, $1);
        } |
	postfix_exp T_INC {
                $$ = buildTree(POST_INC, $1);
        } |
	postfix_exp T_DEC{
                $$ = buildTree(POST_DEC, $1);
        };
opt_actual_param :
	actual_param{
                $$ = $1;
        }|
        { $$ = NULL; } ;
actual_param :
	actual_param_list{
                $$ = buildTree(ACTUAL_PARAM, $1);
        };
actual_param_list :
	assignment_exp {
                $$ = $1;
        }|
	actual_param_list ',' assignment_exp{
                $$ = expandNode($1, $3);
        };
primary_exp :
	T_IDENT  {
                $$ = createNode(IDENT, $1);
        }|
	T_NUMBER  {
                $$ = createNode(NUMBER, $1);
        }|
	'(' expression ')'{
                $$ = $2;
        };
%%



int main(int argc, char *argv[])
{
        FILE * sourceFile;
        FILE * destFile;
        char * destName;
        int printast = 0;
        int printsyt = 0;

        if(argc < 3){
                printf("Argument : minic [sourcefile] [destfile] [flags]\nflags : -a print abstract symtax tree\n-s print symbol table dump before reset");
                return -1;
        }

        //FILE READ
        sourceFile = fopen(argv[1], "r");
        if(!sourceFile){
                printf("sourceFile is not exist");
                return -1;
        }

        for(int i = 3 ; i < argc ; i++){
                if( strcmp(argv[i], "-a") == 0 ) printast = 1;
                if( strcmp(argv[i], "-s") == 0 ) printsyt = 1;
        }

        //source -> AST
        yyin = sourceFile;
        while(!feof(yyin)){
                yyparse();
        }

        if(printast){
                printAST(root, 0);
        }

        //AST -> ucode
        destFile = fopen(argv[2], "w");
        if(!destFile){
                printf("can't create ucode file\n");
        }
        ucodegen(root, destFile, printsyt);

        return 0;
}

int yyerror(char *s) {
	printf("%s\n",s );
}
