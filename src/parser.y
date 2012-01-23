%{
#include <cstdio>
#include <iostream>
#include <string>

int yylex (void);
void yyerror (char const *);
extern FILE *yyin;

%}

%union {
  int ival;
  double fval;
  std::string *str;
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
%token TK_SCOLON      ";"

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

%% /* Gramática */

stmts:
   /* empty */
 | stmts statement

statement:
   if

if:
   "if" expr "{" stmts "}"
   { std::cout << "Encontre un if sin else" << std::endl }
 | "if" expr "{" stmts "}" "else" "{" stmts "}"
   { std::cout << "Encontre un if con else" << std::endl }

expr:
   TK_ID
 | TK_CONSTINT
 | TK_CONSTFLOAT
 | TK_TRUE
 | TK_FALSE

dec: 
   tipo list_items TK_SCOLON
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
  return yyparse();
}
