%{
#include <stdio.h>
#include <string.h>
#include "minic.tab.h"
%}

%%
"const"	return T_CONST;
"else"	return T_ELSE;
"if"	return T_IF;
"int"	return T_INT;
"return"	return T_RETURN;
"void"	return T_VOID;
"while"	return T_WHILE;
"=="	return T_EQUAL;
"!="	return T_NOTEQUAL;
"<="	return T_LESSE;
">="	return T_GREATE;
"&&"	return T_AND;
"||"	return T_OR;
"++"	return T_INC;
"--"	return T_DEC;
"+="	return T_ADD_ASSIGN;
"-="	return T_SUB_ASSIGN;
"*="	return T_MUL_ASSIGN;
"/="	return T_DIV_ASSIGN;
"%="	return T_MOD_ASSIGN;

[A-Za-z_][A-Za-z0-9_]* {
	yylval.tval = strdup(yytext);
	return T_IDENT;
}
[1-9][0-9]*|0([0-7]+|(x|X)[0-9A-Fa-f]*)? {
	yylval.tval = strdup(yytext);
	return T_NUMBER;
}
"/*"([^*]|\*+[^*/])*\**"*/"	;
"//".*	;
[ \t\n]	;
.	return(yytext[0]);

%%

int yywrap(){
	return 1;
}
