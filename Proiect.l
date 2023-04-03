%{
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include "Proiect.tab.h"

%}

%option noyywrap

%%
"int"|"string"|"float"|"bool"|"char" {yylval.strval=strdup(yytext); return TYPE;}
"cout << " {yylval.strval=strdup(yytext); return AFISEAZA;}
"void" {return VOID;} 
"struct" {return STRUCT;}
"return" {return RETURN;}


"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"for" {return FOR;}
"Eval" {return EVAL;}
"TypeOf" {return TYPEOF;}
"const" {return CONST;}

"&&" {return AND;}
"||" {return OR;}

"+"|"-" {yylval.strval = strdup(yytext); return OP1;}
"*"|"/" {yylval.strval = strdup(yytext); return OP2;}
">=" {yylval.strval = strdup(yytext); return GEQ;}
"<=" {yylval.strval = strdup(yytext); return LEQ;}
"==" {yylval.strval = strdup(yytext); return EQ;}
['][a-zA-z0-9]['] {
                        yylval.strval = strdup(yytext);
                        return C_CHAR;
                  }
["][a-zA-z0-9]*["] {    yylval.strval = strdup(yytext);
                        return C_STRING;
                  }
"true"|"false" {yylval.strval = strdup(yytext) ; return C_BOOL;}
[a-zA-Z][a-zA-Z0-9]*[@]?[a-zA-Z0-9]*  {yylval.strval=strdup(yytext); return ID;}
[-](0|[1-9][0-9]*)\.[0-9]+|(0|[1-9][0-9]*)\.[0-9]+ {yylval.strval = strdup(yytext); return C_FLOAT;}
0|[-][1-9][0-9]*|[1-9][0-9]* { yylval.strval= strdup(yytext); return C_INT;}



[ \t] ;
\n {yylineno++;}
\r ;
. {return yytext[0];}
