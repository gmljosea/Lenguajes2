%defines
%output "parser.cc"

%locations

%code requires {
#include <cstdio>
#include <iostream>
#include <list>
#include <set>
#include <string>

#include "expression.hh"
#include "program.hh"
#include "statement.hh"
#include "type.hh"
}

%code {

int yylex();
void yyerror (char const *);
extern FILE *yyin;

// Resultado del parseo
Program program;

// Variables globales útiles para chequeos durante el parseo
SymFunction* currentfun; // Función parseada actual
std::list<std::string> looplabels;

/**
 * Extrae los campos de yylloc y los utiliza para inicializar los campos
 * de ubicación de un Statement cualquiera.
 */
void setLocation(Statement* stmt, YYLTYPE* yylloc) {
  stmt->setLocation(yylloc->first_line, yylloc->first_column, yylloc->last_line,
		    yylloc->last_column);
}

/**
 * Empila una etiqueta de iteración nueva en la pila, chequeando que no exista
 * otra etiqueta con el mismo nombre ya empilada.
 * Si ya existe una etiqueta, se cuenta como error semántico. Sin importar si
 * hay o no error, igual se empila la etiqueta, de manera que el parseo pueda
 * continuar normalmente.
 */
void pushLoopLabel(std::string label, YYLTYPE* yylloc) {
  for (std::list<std::string>::iterator it = looplabels.begin();
       it != looplabels.end(); it++) {
    if (*it == label) {
      program.error("etiqueta '"+label+"' repetida.", yylloc->first_line,
		    yylloc->first_column);
      program.errorCount++;
      break;
    }
  }
  looplabels.push_front(label);
}

/**
 * Desempila una etiqueta de la pila de etiquetas, más nada.
 */
void popLoopLabel() {
  looplabels.pop_front();
}

bool functionRedeclared(std::string id) {
  SymFunction* symf = program.symtable.lookup_function(id);
}

bool variableRedeclared(std::string id) {

}

}

%union {
  int ival;
  double fval;
  std::string *str;
  Statement *stmt;
  Block *blk;
  Expression *exp;
  Type *type;
  std::list<Expression*> *exps;
  std::list<Lvalue*> *lvalues;
  std::list<std::pair<SymVar*,Expression*>> *decls;
  std::pair<SymVar*,Expression*> *decl;
  Lvalue *lvalue;
  PassType passtype;
  listSymPairs *argsdec;
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
%token TK_VARIANT     "variant"
%token TK_TRUE        "true"
%token TK_FALSE       "false"
%token TK_RETURN      "return"
%token TK_BREAK       "break"
%token TK_NEXT        "next"
%token TK_WRITE       "write"
%token TK_READ        "read"
%token TK_RETRY       "retry"
%token TK_WRITELN     "writeln"

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
%token TK_COLON       ":"

// Token identificador (de variable, función o box)
%token <str> TK_ID

// Token de un string cualquiera encerrado entre comillas
 // No confundirse con TK_STRING que se refiere a la palabra reservada 'string'
%token <str> TK_CONSTSTRING

// Tokens de constantes numéricas
%token <ival> TK_CONSTINT
%token <fval> TK_CONSTFLOAT

%type <stmt> statement if while for variabledec asignment
%type <blk> stmts keepscope_block newscope_block else
%type <exp> expr funcallexp step
%type <str> label
%type <type> type
%type <lvalues> lvalues
%type <lvalue> lvalue
%type <exps> explist nonempty_explist
%type <decls> vardec_items
%type <decl> vardec_item
%type <passtype> passby
%type <argsdec> args nonempty_args

%% /* Gramática */

 /* Produce un programa escrito en Devanix. Un programa es básicamente una
    secuencia de declaraciones de funciones, variables globales y boxes. */
globals:
  global
| globals global

 /* Produce una declaración de función, variable global o box */
global:
  variabledec
    { program.globalinits.push_back(dynamic_cast<VariableDec*> $1); }

| type TK_ID enterscope "(" args ")"
    { SymFunction* sym = new SymFunction(*$2, @2.first_line,
					 @2.first_column, $5);
      currentfun = sym;
      if (!functionRedeclared(*$2)) {
	/* Si una función se redeclara, no se inserta en la tabla de símbolos
	   Pero si instanciamos el SymFunction y actualizamos la variable
	   currentfun para por lo menos poder chequear los return.
         */
	program.symtable.insert(sym);
      }
    }
  keepscope_block leavescope
    {
      currentfun->setType(*$1);
      currentfun->setBlock($8);
      program.functions.push_back(currentfun);
    }

 /* Produce una lista de declaraciones de argumentos de una función,
    posiblemente vacía. */
args:
  /* empty */
    { $$ = new listSymPairs(); }
| nonempty_args

 /* Produce una lista de declaraciones de argumentos de una función con al menos
    un argumento. */
nonempty_args:
  passby type TK_ID
    {
     $$ = new listSymPairs();
      if (!variableRedeclared(*$3)) {
      SymVar* arg = new SymVar(*$3, @3.first_line, @3.first_column, true);
     arg->setType(*$2);
     //if ($1 == PassBy::readonly) arg->setReadonly(true);
     $$->push_back(std::pair<PassType,SymVar*>($1,arg));
} }
| nonempty_args "," passby type TK_ID
   { /* !!! Meter símbolo en la tabla y agregarlo a la lista de argumentos */
     SymVar* arg = new SymVar(*$5, @5.first_line, @5.first_column, true);
     arg->setType(*$4);
     //if ($3 == PassBy::readonly) arg->setReadonly(true);
     $1->push_back(std::pair<PassType,SymVar*>($3,arg));
     $$ = $1;}


passby:
   /* empty */  { $$ = PassType::normal; }
 | "$"          { $$ = PassType::reference; }
 | "$$"         { $$ = PassType::readonly; }

keepscope_block:
 /* Igual que la regla 'block', pero no abre un alcance.
    Esto porque como es un bloque de función, el alcance que le corresponde
    fue abierto antes de llegar a esta regla (para meter los símbolos de los
    parámetros) */
   "{" stmts "}" { setLocation($2,&@$); $$ = $2; }

 // ** Fin gramática de la declaración de funciones

/**
 * Un bloque es un entorno de referencia único junto a una secuencia de
 * instrucciones.
 */
newscope_block:
   "{" enterscope stmts "}" leavescope
   { setLocation($3,&@$); $$ = $3;}

enterscope:
   /* empty */ { program.symtable.enter_scope(); }

leavescope:
   /* empty */ { program.symtable.leave_scope(); }

 // ** Produce una secuencia de instrucciones
stmts:
   statement
   { $$ = new Block(0, $1);
     $1->setEnclosing($$);
     setLocation($1, &@$);}
 | stmts statement
   { $1->push_back($2);
     $2->setEnclosing($1);
     setLocation($2, &@$);
     $$ = $1; }

statement:
  ";"       { $$ = new Null(); }
 | if
 | while
 | for
 | variabledec
 | asignment
 | funcallexp ";" { $$ = new FunctionCall($1); }
 | "break" TK_ID ";" { $$ = new Break($2); }
 | "break" ";" { $$ = new Break(NULL); }
 | "next" TK_ID ";" { $$ = new Next($2); }
 | "next" ";" { $$ = new Next(NULL); }
 | "return" expr ";" { $$ = new Return($2); }
 | "return" ";"     { $$ = new Return(); }
 | "retry" ";" { $$ = new Retry(); }
 | "write" nonempty_explist ";" { $$ = new Write(*$2,false); }
 | "writeln" nonempty_explist ";" { $$ = new Write(*$2,true); }
 | "read" lvalue ";"   { $$ = new Read($2,NULL); }

if:
   "if" expr newscope_block else
   { $$ = new If($2, $3, $4);
     $3->setEnclosing($$);
     setLocation($$,&@$);}

else:
  /* empty */ { $$ = NULL; }
 |  "else" newscope_block
     { $$ = $2; }

while:
   label
{ if ($1) pushLoopLabel(*$1, &yylloc); }
   "while" expr newscope_block
     { if ($1) popLoopLabel();
       $$ = new While($1, $4, $5); }

for:
   label
 { if ($1) pushLoopLabel(*$1, &yylloc); }
   "for" TK_ID "in" expr ".." expr step enterscope
     { /* Insertar TK_ID en la tabla con tipo Int */ }
   keepscope_block leavescope
     { if ($1) popLoopLabel();
       SymVar* loopvar = new SymVar(*$4, @4.first_line,
				    @4.first_column, false);
       IntType it;
       loopvar->setType(it);
       $$ = new BoundedFor($1, loopvar, $6, $8, $9, $12); }
 /* Hubiera preferido usar variables nombradas como $label, pero
    bison no las reconoce, contrario a lo que indica el
    manual. Yay open source. */

step:
   /* empty */ { $$ = NULL; }
 | "step" expr { $$ = $2; }


label:
   /* empty */  { $$ = NULL; }
 | TK_ID ":"    { $$ = $1; }

 // ** Inicio gramática de la asignación
asignment: // Modificar clase Asignment para que reciba listas de rvalues y lvalues
   lvalues "=" explist ";" { $$ = new Asignment(*$1, *$3);  }

lvalues: // Devuelve list<Lvalue*>
   lvalue { $$ = new std::list<Lvalue*>(); $$->push_back($1); }
 | lvalues "," lvalue { $1->push_back($3); $$ = $1; }

lvalue: // Instanciar lvalue (falta hacer la clase)
   TK_ID  { $$ = new Lvalue(); }

 // ** Fin gramática de la asignación

 // ** Inicio gramática de la declaración de variables
variabledec:
   type vardec_items ";"
   { for (std::list<std::pair<SymVar*,Expression*>>::iterator it = $2->begin();
	  it != $2->end(); it++) {
       // !!! Setear el tipo del símbolo
       //first->setType($1);;
     }
     $$ = new VariableDec(*$1,*$2);
   }

vardec_items: // Devuelve una lista de pair<string,expr>
   vardec_item
   { $$ = new std::list<std::pair<SymVar*,Expression*>>();
     $$->push_back(*$1); }
 | vardec_items "," vardec_item
   { $1->push_back(*$3); $$ = $1; }

vardec_item: // Devuelve un pair<string,expr>
   TK_ID
   { SymVar* sym = new SymVar(*$1, @1.first_line, @1.first_column,false);
     // !!! Falta Agregar a tabla de símbolos
     $$ = new std::pair<SymVar*,Expression*>(sym,NULL); }
 | TK_ID "=" expr
   { SymVar* sym = new SymVar(*$1, @1.first_line, @1.first_column,false);
     // !!! Falta agregar a tabla de símbolos
     $$ = new std::pair<SymVar*,Expression*>(sym,$3); }

type:
   "int"   { $$ = new IntType(); }
 | "char"  { $$ = new CharType(); }
 | "bool"  { $$ = new BoolType(); }
 | "float" { $$ = new FloatType(); }
 | "void"  { $$ = new VoidType(); }
 // ** Fin gramática de la declaración de variables

 // ** Inicio gramática de las expresiones
expr:
   TK_ID          { $$ = new VarExp(); }
 | TK_CONSTINT    { $$ = new IntExp(); }
 | TK_CONSTFLOAT  { $$ = new FloatExp(); }
 | TK_TRUE        { $$ = new BoolExp(); }
 | TK_FALSE       { $$ = new BoolExp(); }
 | TK_CONSTSTRING { $$ = new StringExp(); }
 | funcallexp

funcallexp:
   TK_ID "(" explist ")" { $$ = new FunCallExp(); }

explist:
   /* empty */ { $$ = new std::list<Expression*>(); }
 | nonempty_explist

nonempty_explist:
   expr    { $$ = new std::list<Expression*>(); $$->push_back($1); }
 | explist "," expr { $1->push_back($3); $$ = $1; }

 // ** Fin gramática de las expresiones

%%

void yyerror (char const *s) {
  std::cerr << "Error: " << s << std::endl;
}

// Por ahora el main está aquí, pero luego hay que moverlo
int main (int argc, char **argv) {
  if (argc == 2) {
    yyin = fopen(argv[1], "r");
  }

  program.errorCount = 0;

  yyparse();

  // Segunda vuelta haciendo chequeos semánticos

  // Si hay muchos errores, no imprimir el árbol ni nada
  if (program.errorCount > 0) {
    return 1;
  }

  std::cout << "-- Variables globales --" << std::endl << std::endl;

  for (std::list<VariableDec*>::iterator it = program.globalinits.begin();
       it != program.globalinits.end(); it++) {
    (**it).print(0);
  }

  std::cout << std::endl << "-- Funciones definidas --" << std::endl << std::endl;

  for (std::list<SymFunction*>::iterator it = program.functions.begin();
       it != program.functions.end(); it++) {
    (**it).print();
  }

  return 0;
}
