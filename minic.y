%{
#include <stdio.h>

extern FILE * yyin;
%}

%token T_CONST T_ELSE T_IF T_INT T_RETURN T_VOID T_WHILE T_EQUAL T_NOTEQUAL T_LESSE T_GREATE T_AND T_OR T_INC T_DEC T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_IDENT T_NUMBER;
%nonassoc T_ELSE

%%

mini_c :
	translation_unit{
	printf("Syntax Correct");
	return 0;
	};
translation_unit :
	external_dcl |
	translation_unit external_dcl;
external_dcl :
	function_def  |``
	declaration;
function_def :
	function_header compound_st;
function_header :
	dcl_spec funtion_name formal_param;
dcl_spec :
	dcl_specifiers;
dcl_specifiers :
	dcl_specifier  |
	dcl_specifiers dcl_specifier ;
dcl_specifier :
	type_qualifier  |
	type_specifier 	;
type_qualifier :
	T_CONST;
type_specifier :
	T_INT  |
	T_VOID ;
funtion_name :
	T_IDENT ;
formal_param :
	'(' opt_formal_param ')';
opt_formal_param :
	| formal_param_list;
formal_param_list :
	param_dcl  |
	formal_param_list ',' param_dcl	;
param_dcl :
	dcl_spec declarator;
compound_st :
	'{' opt_dcl_list opt_stat_list '}';
opt_dcl_list :
	 | declaration_list;
declaration_list :
	declaration  |
	declaration_list declaration;
declaration :
	dcl_spec init_dcl_list ';';
init_dcl_list :
	init_declarator |
	init_dcl_list ',' init_declarator;
init_declarator :
	declarator  |
	declarator '=' T_NUMBER ;
declarator :
	T_IDENT |
	T_IDENT '[' opt_number ']';
opt_number :
	 | T_NUMBER;
opt_stat_list :
	 | statement_list;
statement_list :
	statement |
	statement_list statement;
statement :
	compound_st  |
	expression_st  |
	if_st  |
	while_st  |
	return_st;
expression_st :
	opt_expression ';';
opt_expression :
	 | expression;
if_st :
	T_IF '(' expression ')' statement T_ELSE statement |
	T_IF '(' expression ')' statement ;
while_st :
	T_WHILE '(' expression ')' statement;
return_st :
	T_RETURN opt_expression ';';
expression :
	assignment_exp;
assignment_exp :
	logical_or_exp  |
	unary_exp '=' assignment_exp  |
	unary_exp T_ADD_ASSIGN assignment_exp  |
	unary_exp T_SUB_ASSIGN assignment_exp  |
	unary_exp T_MUL_ASSIGN assignment_exp  |
	unary_exp T_DIV_ASSIGN assignment_exp  |
	unary_exp T_MOD_ASSIGN assignment_exp;
logical_or_exp :
	logical_and_exp  |
	logical_or_exp T_OR logical_and_exp;
logical_and_exp :
	equality_exp   |
	logical_and_exp T_AND equality_exp;
equality_exp :
	relational_exp  |
	equality_exp T_EQUAL relational_exp  |
	equality_exp T_NOTEQUAL relational_exp;
relational_exp :
	additive_exp  |
	relational_exp '>' additive_exp  |
	relational_exp '<' additive_exp  |
	relational_exp T_LESSE additive_exp  |
	relational_exp T_GREATE additive_exp;
additive_exp :
	multiplicative_exp  |
	additive_exp '+' multiplicative_exp  |
	additive_exp '-' multiplicative_exp;
multiplicative_exp :
	unary_exp  |
	multiplicative_exp '*' unary_exp  |
	multiplicative_exp '/' unary_exp  |
	multiplicative_exp '%' unary_exp;
unary_exp :
	postfix_exp  |
	'-' unary_exp  |
	'!' unary_exp  |
	T_INC unary_exp  |
	T_DEC unary_exp;
postfix_exp :
	primary_exp  |
	postfix_exp '[' expression ']'  |
	postfix_exp '(' opt_actual_param ')'  |
	postfix_exp T_INC  |
	postfix_exp T_DEC;
opt_actual_param :
	actual_param
	| ;
actual_param :
	actual_param_list;
actual_param_list :
	assignment_exp |
	actual_param_list ',' assignment_exp;
primary_exp :
	T_IDENT  |
	T_NUMBER  |
	'(' expression ')';
%%



int main(int argc, char *argv[])
{
  yyparse();
  return 0;
}

void yyerror(char *s) {
	printf("%s\n",s );
}
