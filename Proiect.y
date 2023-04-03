%{
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include <stdbool.h>
    #include "functii2.h"

    extern FILE* yyin;
    extern char* yytext;
    extern int yylineno; 

%}

%union {
    int intval;
    char* strval;
    float floatval;
    bool boolval;
    char charval;
}

%start s


%token <strval>C_INT <strval>C_FLOAT <strval>C_BOOL <strval>ID <strval>C_STRING <strval>C_CHAR <strval>TYPE
%token <strval>OP1 <strval>OP2 <strval>LEQ <strval>GEQ <strval>EQ 
%token STRUCT AFISEAZA IF WHILE AND OR FOR VOID EVAL TYPEOF CONST RETURN
%type <strval>expr bool_expr fnct_call
%left AND OR
%left '!'
%left OP1
%left OP2
%nonassoc IFX 
%nonassoc ELSE

%%

s
      : '{' declarations '}' '{' functions '}' '{' structs '}' '{' instructions '}' 
      ;
functions
      : functions function 
      |
      ;

function
      : TYPE ID '(' parameters ')' '{' instructions RETURN ID ';' '}'           {if(strcmp(getTypeVar($9), $1)==0) addFunction($1, $2) ; else yyerror("Function type does not match return type.");}
      | VOID ID '(' parameters ')' '{' instructions '}'           {addFunction("void", $2) ;}
      ;
      
parameters
      : parameter ',' parameters
      | parameter
      |     
      ;

parameter
      :  TYPE ID              { addParameter($1, $2);} 
      ;

declarations
      : declarations declaration
	| declaration
      ;

declaration 
      : TYPE ID ';'           { adaugaVar($1, $2);}
      | ID ID ';'             { adaugaVarStruct($1, $2); }
      | CONST TYPE ID '=' expr ';'              {
                                                      if(strcmp($2, TypeOf($5))==0) 
                                                            adaugaVarConst($2, $3, $5);
                                                      else yyerror("incompatibilitate de tipuri la asignare");
                                                }
      ;

structs 
      : structs struct
      | 
      ;

struct 
      : STRUCT ID '{' members '}' ';' {addStruct($2);}
      ;

members
      : members member 
      | 
      ;

member
      : TYPE ID ';' {addMember($1, $2);}
      ;

instructions
      : instruction instructions;
      | instruction
      ;

blocInstr
      : '{' instructions '}'
      | instruction 
      | ';'
      ;

instruction 
      : assign_instr 
      | print_instr
      | if_instr
      | while_instr
      | fnct_call ';'
      | for_instr
      | declaration
      | eval_expr 
      | typeof 
      ;

typeof
      : TYPEOF '(' expr ')' ';'           {printf("%s", TypeOf($3));}

eval_expr
      : EVAL '(' expr ')' ';'             {printf("%s\n", $3);}
      ;
fnct_call
      : ID '(' fnct_call_param_list ')'            { printf("%s\n", $1);  checkFunctionCall($1); $$ = $1; }
      ;

fnct_call_param_list
      : fnct_call_param_list ',' fnct_call_param                   
      | fnct_call_param                                            
      | 
      ;

fnct_call_param : 
  //    : ID                                            { addToParamList(getTypeVar($1));}
       expr                                          {printf("%s %s\n", $1, TypeOf($1)); addToParamList(TypeOf($1)); }
      | bool_expr                                     {addToParamList("bool"); }
      | fnct_call                                     { addToParamList(GetReturnType($1)); }

assign_instr 
      : assignment ';'                          
      ;

assignment
      : ID '=' expr            { 
                                          printf("assign %s = %s\n", $1, $3); 
                                          if(strcmp(getTypeVar($1), TypeOf($3))==0) 
                                                setVal($1, $3);
                                          else yyerror("incompatibilitate de tipuri la asignare");
                                    }
      | ID '=' bool_expr       {
                                          printf("assign bool %s = %s\n", $1, $3); 
                                          if(strcmp(getTypeVar($1), "bool")==0)
                                                setVal($1, $3);
                                          else yyerror("incompatibilitate de tipuri la asignare");
                                    }
      ;

print_instr 
      : AFISEAZA ID ';' 
      ;

if_instr
      : IF '(' bool_expr ')'  blocInstr   %prec IFX  {printf("if");}
      | IF '(' bool_expr ')' blocInstr ELSE blocInstr 
      ;

while_instr
      : WHILE '(' bool_expr ')' blocInstr
      ;

for_instr
      : FOR '(' assign_instr bool_expr ';' assignment ')' blocInstr

expr
      : C_INT           {printf("%s\n", $1); $$ = $1;}
      | C_FLOAT         {printf("%s\n", $1); $$ = $1;}
      | C_STRING        {$$ = $1;}
      | C_CHAR          {$$ = $1;}
      | ID              {printf("%s\n", $1); /*char* val = getValoareVariabila($1); if(val!=NULL) */ $$ = getValoareVariabila($1); /* else yyerror("Stringul nu a fost initializat"); */}
      | expr OP1 expr    {
                              char* exp = (char*) malloc(strlen($1)+strlen($3)+2);
                              sprintf(exp, "%s%s%s", $1, $2, $3); 
                              $$ = Eval(ConstruiesteArbore(exp), TypeOf(exp)); 
                        }
      | expr OP2 expr   {
                              char* exp = (char*) malloc(strlen($1)+strlen($3)+2);
                              sprintf(exp, "%s%s%s", $1, $2, $3); 
                              $$ = Eval(ConstruiesteArbore(exp), TypeOf(exp)); 
                        }
      | '(' expr ')'    {printf("(%s)\n", $2);  $$ =$2;}
      ;
      
bool_expr
      : C_BOOL                      {printf("%s\n", $1); $$=$1;}
      | '!' bool_expr               {$$= BoolToString( ! StringToBool($2) );}
      | bool_expr AND bool_expr     {$$= BoolToString( StringToBool($1) && StringToBool($3) );}
      | bool_expr OR bool_expr      {$$= BoolToString( StringToBool($1) || StringToBool($3) );}
      | '(' bool_expr ')'           {$$=$2;}
      | expr '<' expr               {if(atoi($1) < atoi($3)) $$= "true";  else $$ = "false"; }
      | expr LEQ expr               {if(atoi($1) <= atoi($3)) $$= "true";  else $$ = "false";}
      | expr '>' expr               {if(atoi($1) > atoi($3)) $$= "true";  else $$ = "false";}
      | expr GEQ expr               {if(atoi($1) >= atoi($3)) $$= "true";  else $$ = "false";}
      | expr EQ expr                {if(atoi($1) == atoi($3)) $$= "true";  else $$ = "false"; }
      ;

%%

int main(int argc, char** argv){
      yyin=fopen(argv[1],"r");
      yyparse();

      PrintSymbolTable();
      return 0;
} 