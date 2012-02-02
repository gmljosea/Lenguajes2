%defines
%output "parser.cc"
%define api.pure
%locations

%code requires {
#include <cstdio>
#include <iostream>
#include <string>

#include "expression.hh"
#include "statement.hh"

}

%code {

int yylex(YYSTYPE*,YYLTYPE*);
void yyerror (char const *);
extern FILE *yyin;
Block *ast; //evidentemente esto no es, pero es para ir viendo que resulta el parser
int current_scope() {
  return 0;
}

}

%union {
  int ival;
  double fval;
  std::string *str;
  Statement *stmt;
  Block *blk;
  Expression *exp;
};

// Tokens de palabras reservadas
%token TK_IF          "if"
%token TK_ELSE        "else"
%token TK_FOR         "for"
%token TK_IN          "in"
%token TK_STEP        "step"
%token TK_WHILE       "while"
%token TK_INT         "int"
%token TK_CHAR        "char"
%token TK_BOOL        "bool"
%token TK_FLOAT       "float"
%token TK_ARRAY       "array"
%token TK_STRING      "string"
%token TK_BOX         "box"
%token TK_VOID        "void"
%token TK_VARIANT     "variant:"
%token TK_TRUE        "true"
%token TK_FALSE       "false"
%token TK_RETURN      "return"
%token TK_BREAK       "break"
%token TK_NEXT        "next"

// Tokens de símbolos especiales

%token TK_PLUS        "+"
%token TK_MINUS       "-"
%token TK_TIMES       "*"
%token TK_DIV         "/"
%token TK_MOD         "%"
%token TK_EQU         "="
%token TK_LT          "<"
%token TK_GT          ">"
%token TK_GTE         ">="
%token TK_LTE         "<="
%token TK_AND         "and"
%token TK_OR          "or"
%token TK_NOT         "not"

%token TK_LBRACE      "{"
%token TK_RBRACE      "}"
%token TK_LBRACKET    "["
%token TK_RBRACKET    "]"
%token TK_LPARENT     "("
%token TK_RPARENT     ")"
%token TK_COMMA       ","
%token TK_SCOLON      ";"

%token TK_DPERIOD     ".."

%token TK_DOLLAR      "$"
%token TK_DDOLLAR     "$$"

// Token identificador (de variable, función o box)
%token <str> TK_ID

// Token de un string cualquiera encerrado entre comillas
 // No confundirse con TK_STRING que se refiere a la palabra reservada 'string'
%token <str> TK_CONSTSTRING

// Tokens de constantes numéricas
%token <ival> TK_CONSTINT
%token <fval> TK_CONSTFLOAT

%type <stmt> statement if while for dec
%type <blk> block stmts
%type <exp> expr

%% /* Gramática */

/*
Nota: para engranar el sistema de leblanc-cook, se modifica block para que maneje
apropiadamente los alcances.
*/

/*
Nota: hacer que la grmática esté en inglés 100% y sea legible
*/

start:
   block { ast = $1; } /* solo para propósitos de testing rapido */
 | globals

globals:
   global
 | globals global

global:
   /*globalvar*/
  funtype TK_ID "(" params ")" block

funtype: "void" | tipo

params:
   /* empty */
 | paramlist

paramlist:
   passby tipo TK_ID
 | paramlist "," passby tipo TK_ID

passby:
   /* empty */
 | "$"
 | "$$"

block: "{" stmts "}"  { $$ = $2; }

stmts:
   statement        { $$ = new Block(current_scope(), $1);
     std::cout << "Ubicacion " << @1.first_line << ":" << @1.first_column
	       << " a " << @1.last_line << ":" << @1.last_column << std::endl;}
 | stmts statement  { $1->push_back($2); $$ = $1; }

statement:
  ";"       { $$ = new Null(); }
 | if
 | while
 | for
 | dec
 | asignment ";"
 | expr ";" /* Temporal, hay que discutir esto */

if:
   "if" expr block
   { std::cout << "Encontré un if sin else" << std::endl;
     $$ = new If($2, $3);}
 | "if" expr block "else" block
   { std::cout << "Encontré un if con else" << std::endl;
     $$ = new If($2, $3, $5); }

while:
   "while" expr block
   { std::cout << "Encontré un while" << std::endl;
     $$ = new While($2, $3);}

for:
   "for" TK_ID "in" expr ".." expr block
   { std::cout << "Encontré un for sin paso" << std::endl;
     $$ = new BoundedFor($2, $4, $6, NULL, $7); }
 | "for" TK_ID "in" expr ".." expr "step" expr block
   { std::cout << "Encontré un for con paso" << std::endl;
     $$ = new BoundedFor($2, $4, $6, $8, $9); }
 /*| "for" TK_ID "in" TK_ID block  //foreach de arreglos*/

expr:
   TK_ID          { $$ = new Expression(); }
 | TK_CONSTINT   { $$ = new Expression(); }
 | TK_CONSTFLOAT { $$ = new Expression(); }
 | TK_TRUE { $$ = new Expression(); }
 | TK_FALSE { $$ = new Expression(); }

asignment:
   lvalue "=" expr
 | lvalue "," asignment "," expr

lvalue: TK_ID

dec: 
   tipo list_items ";"
   {std::cout << "declaracion"}

list_items:
    item 
   |list_items "," item

item:
   TK_ID 
   | TK_ID "=" expr

tipo:
   "int"
   | "char"
   | "bool"
   | "float"

%%

void yyerror (char const *s) {
  std::cerr << "Error: " << s << std::endl;
}

// Por ahora el main está aquí, pero luego hay que moverlo
int main (int argc, char **argv) {
  if (argc == 2) {
    yyin = fopen(argv[1], "r");
  }
  yyparse();
  ast->print(0);
  return 0;
}
