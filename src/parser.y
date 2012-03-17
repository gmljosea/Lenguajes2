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
std::string currentbox;

/* Como todos los box se pueden ver entre si, puede que dentro de un box
   se declare una variable de un tipo box cuya declaracion aun no se ha 
   encontrado, en este caso lo guardamos mientras en el hash de unknown 
   con la condicion de que cuando se encuentre su definicion sea movido a
   la tabla de simbolos*/ 
 boxHash unknownBox;
 funcSymtable unknownFunc;

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

/**
 * Recibe el nombre de una funcion y su yyloc.
 * Devuelve true si determina que la función ya ha sido declarada antes,
 * devuelve false si no ha sido declarada aún.
 */
bool functionRedeclared(std::string id, YYLTYPE yylloc) {
  SymFunction* symf = program.symtable.lookup_function(id);
  if (symf != NULL) {
    std::string err = "redeclaración de función '"
      +id+"' previamente declarada en "+std::to_string(symf->getLine())
      +":"+std::to_string(symf->getColumn());
    program.error(err, yylloc.first_line, yylloc.first_column);
    symf->setDuplicated(true);
    return true;
  }
  return false;
}

/**
 * Recibe el nombre de una variable y su yyloc.
 * Devuelve true si determina que la variable ya ha sido declarada antes,
 * devuelve false si no ha sido declarada aún.
 */
bool variableRedeclared(std::string id, YYLTYPE yylloc) {
  SymVar* symv = program.symtable.lookup_variable(id);
  if (symv != NULL && symv->getnumScope() == program.symtable.current_scope()) {
    std::string err = "redeclaración de variable '"
      +id+"' previamente declarada en "+std::to_string(symv->getLine())
      +":"+std::to_string(symv->getColumn());
    program.error(err, yylloc.first_line, yylloc.first_column);
    symv->setDuplicated(true);
    return true;
  }
  return false;
}

/**
 * Recibe el nombre de una box y su yyloc.
 * Devuelve true si determina que tipo box ya ha sido declarado antes,
 * devuelve false si no ha sido declarado aún.
 */
bool boxRedeclared(std::string id, YYLTYPE yylloc) {
  BoxType* boxtype = program.symtable.lookup_box(id);
  if (boxtype != NULL) {
    std::string err = "redeclaración de tipo box '"
      +id+"' previamente declarado en "+std::to_string(boxtype->getLine())
      +":"+std::to_string(boxtype->getColumn());
    program.error(err, yylloc.first_line, yylloc.first_column);
    return true;
  }
  return false;
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
  BoxType *box;
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
%token TK_WRITELN     "writeln"

// Tokens de símbolos especiales

%token TK_PLUS        "+"
%token TK_MINUS       "-"
%token TK_TIMES       "*"
%token TK_DIV         "/"
%token TK_MOD         "%"
%token TK_EQU         "="
%token TK_NEQ         "!="
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
%token TK_DOT         "."

%token TK_DPERIOD     ".."

%token TK_DOLLAR      "$"
%token TK_DDOLLAR     "$$"
%token TK_COLON       ":"

// Token identificador (de variable, función o box)
%token <str> TK_ID

// Token de un string cualquiera encerrado entre comillas
 // No confundirse con TK_STRING que se refiere a la palabra reservada 'string'
%token <str> TK_CONSTSTRING
%token <str> TK_CONSTCHAR

// Tokens de constantes numéricas
%token <ival> TK_CONSTINT
%token <fval> TK_CONSTFLOAT

%type <stmt> statement if while for variabledec asignment
%type <blk> stmts block else
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
%type <box> box


%left "or"
%left "and"
%left "=" "!="
%left "<" "<=" ">=" ">"
%left "+" "-"
%left "*" "/" "%"
%right NEG "not"
%left "[" "."

%% /* Gramática */

// Nota: estoy marcando las áreas que faltan por hacer con !!!

/*
 * NOTA IMPORTANTE
 * Las reglas utilizan las variables numeradas de bison ($1, $2, etc)
 * en vez de las mucho más legibles variables nombradas ($expr, $if, etc)
 * Esto es porque por alguna bizarra razón bison se niega a reconocer
 * las variables nombradas y los [ ], y no compila.
 */

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
    { /* Si una función se redeclara, no se inserta en la tabla de símbolos,
         Pero si instanciamos el SymFunction y actualizamos la variable
         currentfun para por lo menos poder chequear los return.
       */
      SymFunction* sym = new SymFunction(*$2, @2.first_line,
					 @2.first_column, $5);
      // Este código a mitad de la regla permite que los return sepan
      // en qué función se encuentran con ver la variable currentfun
      currentfun = sym;
      if (!functionRedeclared(*$2, @2)) {
        program.symtable.insert(sym);
      }
    }
block leavescope
    {
      currentfun->setType($1);
      currentfun->setBlock($8);
      program.functions.push_back(currentfun);
    }
| "box" TK_ID box "{" boxdecs variantpart "}"
{
  if(!boxRedeclared(*$2,@2)){
    $3->setLine(@1.first_line);
    $3->setColumn(@1.first_column);
    program.symtable.insert($3);
  }
}

box:
/*empty*/
/* Regla dummy para crear el TypeBox en caso de que no exista en 
   el hash de BoxTypes 'unknownBox'  */
{
  boxHash::iterator it= unknownBox.find(*($<str>0));
  if(it!= unknownBox.end()){
    it->second->setIncomplete(false);
    $$= it->second;
  }else{ 
    $$= new BoxType(currentbox,false);
  }
}

boxdecs:
  type TK_ID ";" 
  {
    /* Se agregan los campos del box usando el BoxType almacenado en
     la pila. Se accede a traves de $<box>-1 */
    BoxField *field= $<box>-1->getField(*$2);
    if(field==NULL){
      $<box>-1->addFixedField($1,*$2);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @2.first_line, @2.first_column);
    }
      
  }  
| boxdecs type TK_ID ";"
{
 BoxField *field= $<box>-1->getField(*$3);
    if(field==NULL){
      $<box>-1->addFixedField($2,*$3);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$3)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @3.first_line, @3.first_column);
    }
} 


variantpart:
/*empty*/
|"variant" ":" variantpart_decs

variantpart_decs:
  dummy "{" variantdecs "}"
|  type TK_ID ";" 
{ 
  BoxField *field= $<box>-4->getField(*$2);
    if(field==NULL){
      $<box>-4->addVariantField($1,*$2,false);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @2.first_line, @2.first_column);
    } 
}
|  variantpart_decs "{" variantdecs "}"
|  variantpart_decs type TK_ID ";"
{
  BoxField *field= $<box>-4->getField(*$3);
  if(field==NULL){
    $<box>-4->startGrouping();
    $<box>-4->addVariantField($2,*$3,true);
  }else{
    std::string err = "redeclaración de variable '"
      +(*$3)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @3.first_line, @3.first_column);
  } 
}

 variantdecs:
type TK_ID ";"
{/*En esta regla se sabe que estamos en una agrupacion de campos union
   por lo tanto hacemos */
  BoxField *field= $<box>-6->getField(*$2);
    if(field==NULL){
      $<box>-6->startGrouping();
      $<box>-6->addVariantField($1,*$2,true);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @2.first_line, @2.first_column);
    } 
}
|variantdecs type TK_ID ";"
{
  BoxField *field= $<box>-6->getField(*$3);
  if(field==NULL){
    $<box>-6->startGrouping();
    $<box>-6->addVariantField($2,*$3,true);
  }else{
    std::string err = "redeclaración de variable '"
      +(*$3)+"' previamente declarada en "+std::to_string(field->line)
      +":"+std::to_string(field->column);
    program.error(err, @3.first_line, @3.first_column);
  } 
}



dummy:
/*empty*/ // Regla dummy para 'emparejar' la pila   

 /* Produce una lista de declaraciones de argumentos (<PassType,SymVar*>)
  * de una función, posiblemente vacía. */
args:
  /* empty */
    { $$ = new listSymPairs(); }
| nonempty_args

 /* Produce una lista de declaraciones de argumentos de una función con al menos
    un argumento. */
nonempty_args:
  passby type TK_ID
    { $$ = new listSymPairs();
      if (!variableRedeclared(*$3, @3)) {
        SymVar* arg = new SymVar(*$3, @3.first_line, @3.first_column, true,
				 program.symtable.current_scope());
        arg->setType($2);
        if ($1 == PassType::readonly) arg->setReadonly(true);
        program.symtable.insert(arg);
        $$->push_back(std::pair<PassType,SymVar*>($1,arg));
      }
    }

| nonempty_args "," passby type TK_ID
    { if (!variableRedeclared(*$5, @5)) {
        SymVar* arg = new SymVar(*$5, @5.first_line, @5.first_column, true,
				 program.symtable.current_scope());
        arg->setType($4);
        if ($3 == PassType::readonly) arg->setReadonly(true);
        program.symtable.insert(arg);
        $1->push_back(std::pair<PassType,SymVar*>($3,arg));
      }
      $$ = $1;
    }

passby:
  /* empty */  { $$ = PassType::normal; }
| "$"          { $$ = PassType::reference; }
| "$$"         { $$ = PassType::readonly; }

 /* Produce un Block (secuencia de instrucciones) */
block:
  "{" stmts "}"
    { setLocation($2,&@$); $$ = $2; }
| "{" "}"
    { $$ = new Block(program.symtable.current_scope(),
		     new Null());
      setLocation($$, &@$);
    }

 /* Regla dummy para abrir un nuevo contexto en la tabla de símbolos */
enterscope:
   /* empty */ { program.symtable.enter_scope(); }

 /* Regla dummy para desempilar un contexto en la tabla de símbolos */
leavescope:
   /* empty */ { program.symtable.leave_scope(); }

 /* Produce una secuencia de instrucciones (Statement) */
stmts:
  statement
    { Block* b = new Block(program.symtable.current_scope(), $1);
      $1->setEnclosing(b);
      setLocation($1, &@1);
      $$ = b;
    }
| stmts statement
    { $1->push_back($2);
      $2->setEnclosing($1);
      setLocation($2, &@2);
      $$ = $1;
    }

 /* Produce una instrucción del lenguaje */
statement:
  ";"
    { $$ = new Null(); }
| funcallexp ";"
    { $$ = new FunctionCall($1); }
| "break" TK_ID ";"
    { $$ = new Break($2); }
| "break" ";"
    { $$ = new Break(NULL); }
| "next" TK_ID ";"
    { $$ = new Next($2); }
| "next" ";"
    { $$ = new Next(NULL); }
| "return" expr ";"
    { $$ = new Return(currentfun, $2); }
| "return" ";"
    { $$ = new Return(currentfun); }
| "write" nonempty_explist ";"
    { $$ = new Write(*$2,false); }
| "writeln" nonempty_explist ";"
    { $$ = new Write(*$2,true); }
| "read" lvalue ";"
    { $$ = new Read($2); }
| if
| while
| for
| variabledec
| asignment

 /* Produce una instrucción If, con o sin bloque else */
if:
  "if" expr enterscope block leavescope else
    { $$ = new If($2, $4, $6);
      $4->setEnclosing($$);
    }

 /* Produce un Block que representa el 'else' de un If, podría ser vacío */
else:
  /* empty */ { $$ = NULL; }
| "else" enterscope block leavescope
    { $$ = $3; }

 /* Produce una instrucción While, duh */
while: 
 label  // Chequear la etiqueta antes de seguir procesando el while
 { if ($1) pushLoopLabel(*$1, &yylloc); }
 "while" expr enterscope block leavescope
 { if ($1) popLoopLabel();
   if ($1 == NULL) { 
     @$.first_line = @3.first_line;
     @$.first_column = @3.first_column;
     @$.last_line = @7.last_line;
     @$.last_column = @7.last_column;
   }
   $$ = new While($1, $4, $6);
 }

 /* Produce un For, ya sea un for de enteros o un foreach sobre un array.
    El for de enteros puede o no tener un paso (step) definido. */
for:
  label // Chequear la etiqueta antes de seguir procesando el For
    { if ($1) pushLoopLabel(*$1, &yylloc); }
  "for" TK_ID "in" expr ".." expr step enterscope
    { /* Meter variable de iteración en la tabla antes de revisar las
         instrucciones */
      SymVar* loopvar = new SymVar(*$4, @4.first_line, @4.first_column, false,
				   program.symtable.current_scope());
      loopvar->setType(&(IntType::getInstance()));
      loopvar->setReadonly(true);
      program.symtable.insert(loopvar);
    }
  block leavescope
    { if ($1) popLoopLabel();
      /* Esto es bastante chimbo, pero es la manera menos chimba que se me
	 ocurrió de volver a conseguir el SymVar de la iteración para
         poder instanciar el For.
         La otra manera sería llevar una pila de variables de iteración. */
      /* Otra manera que se me ocurre es que el constructor de BoundedFor 
       * no tenga como argumento el bloque, que se instancie en las acciones 
       * de arriba, y en esta parte se haga un setBlock() */
      SymVar* loopvar = program.symtable.lookup_variable(*$4);
      if ($1 == NULL) {
        @$.first_line = @3.first_line;
        @$.first_column = @3.first_column;
        @$.last_line = @13.last_line;
        @$.last_column = @13.last_column;
      }
      $$ = new BoundedFor($1, loopvar, $6, $8, $9, $12);
    }

 /* Produce la parte opcional del For 'step' */
step:
  /* empty */  { $$ = NULL; }
| "step" expr  { $$ = $2; }

 /* Produce la etiqueta opcional de los For y While */
label:
  /* empty */  { $$ = NULL; }
| TK_ID ":"    { $$ = $1; }

 /* Produce una instrucción Asignación */
asignment:
  lvalues "=" nonempty_explist ";"
  { $$ = new Asignment(*$1, *$3); }

 /* Produce una lista de l-values separados por comas */
lvalues:
  lvalue
    { $$ = new std::list<Lvalue*>();
      $$->push_back($1);
    }
| lvalues "," lvalue
    { $1->push_back($3);
      $$ = $1;
    }

 /* Produce un Lvalue */
lvalue:
  TK_ID
    { SymVar* symv = program.symtable.lookup_variable(*$1);
      if (symv == NULL) {
        program.error("variable '"+*$1+"' no declarada", @1.first_line,
		      @1.first_column);
        $$ = new BadLvalue(); // O un YYERROR?
      } else {
        $$ = new NormalLvalue(symv);
      }
    }

 /* Produce una instrucción Declaración de variables */
variabledec:
  type vardec_items ";"
    { for (std::list<std::pair<SymVar*,Expression*>>::iterator it = $2->begin();
	   it != $2->end(); it++) {
        (*it).first->setType($1);
      }
      $$ = new VariableDec($1,*$2);
    }

 /* Produce una lista de declaraciones de variables */
vardec_items:
  vardec_item
    { $$ = new std::list<std::pair<SymVar*,Expression*>>();
      if ($1) $$->push_back(*$1);
    }
  | vardec_items "," vardec_item
    { if ($1) $1->push_back(*$3);
      $$ = $1;
    }

 /* Produce una declaración de variable, la cual es un par que asocia la
    variable con su expresión de inicialización, la cual es opcional */
vardec_item:
  TK_ID
    { if (!variableRedeclared(*$1, @1)) {
        SymVar* sym = new SymVar(*$1, @1.first_line, @1.first_column,false,
				 program.symtable.current_scope());
        program.symtable.insert(sym);
        $$ = new std::pair<SymVar*,Expression*>(sym,NULL);
      } else {
        $$ = NULL;
      }
    }
 | TK_ID "=" expr
    { if (!variableRedeclared(*$1, @1)) {
        SymVar* sym = new SymVar(*$1, @1.first_line, @1.first_column,false,
				 program.symtable.current_scope());
        program.symtable.insert(sym);
        $$ = new std::pair<SymVar*,Expression*>(sym,$3);
      } else {
        $$ = NULL;
      }
    }

 /* Produce un tipo válido del lenguaje. Por ahora solo los tipos básicos. */
type:
"int"    { $$ = &(IntType::getInstance()); }
| "char"   { $$ = &(CharType::getInstance()); }
| "bool"   { $$ = &(BoolType::getInstance()); }
| "float"  { $$ = &(FloatType::getInstance()); }
| "string" { $$ = new StringType(1);}
| "void"   { $$ = &(VoidType::getInstance()); }

 // ** Gramática de las expresiones

 /* Produce una expresión.
    Por ahora las expresiones válidas son las constantes, las variables y las
    llamadas a funciones. */
expr:
TK_ID
  { SymVar* symv = program.symtable.lookup_variable(*$1);
    if (symv == NULL) {
      program.error("variable '"+*$1+"' no declarada", @1.first_line,
		    @1.first_column);
      $$ = new BadExp();
    } else {
      $$ = new VarExp(symv);
    }
    $$->setLocation(@1.first_line, @1.first_column,0,0);
  }
| TK_CONSTINT    { $$ = new IntExp($1);
                   $$->setLocation(@1.first_line, @1.first_column,0,0); }
| TK_CONSTFLOAT  { $$ = new FloatExp($1);
                   $$->setLocation(@1.first_line, @1.first_column,0,0); }
| TK_TRUE        { $$ = new BoolExp(true); }
| TK_FALSE       { $$ = new BoolExp(false); }
| TK_CONSTSTRING { $$ = new StringExp(*$1); }
| TK_CONSTCHAR   { $$ = new CharExp(*$1);
                   $$->setLocation(@1.first_line, @1.first_column,0,0); }
| funcallexp
| expr "+" expr  { $$ = new Sum($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0); }
| expr "-" expr { $$ = new Substraction($1,$3); }
| expr "*" expr { $$ = new Multiplication($1,$3); }
| expr "/" expr { $$ = new Division($1,$3); }
| expr "%" expr { $$ = new Remainder($1,$3); }
| "-" expr %prec NEG { $$ = new Minus($2); }
| "(" expr ")" { $$ = $2; }
| expr "and" expr { $$ = new And($1,$3); }
| expr "or" expr { $$ = new Or($1,$3); }
| "not" expr { $$ = new Not($2); }
| expr ">" expr { $$ = new Greater($1,$3); }
| expr ">=" expr { $$ = new GreaterEq($1,$3); }
| expr "=" expr { $$ = new Equal($1,$3); }
| expr "!=" expr { $$ = new NotEqual($1,$3); }
| expr "<" expr { $$ = new Less($1,$3); }
| expr "<=" expr { $$ = new LessEq($1,$3); }
| expr "[" expr "]" { $$ = new Index($1,$3); }
| expr "." TK_ID { $$ = new Dot($1,*$3); }

 /* Produce una llamada a función */
funcallexp:
  TK_ID "(" explist ")"
    { SymFunction* symf = program.symtable.lookup_function(*$1);
      if (symf == NULL) {
        $$ = new FunCallExp(*$1, *$3);
      } else {
        $$ = new FunCallExp(symf, *$3);
      }
    }

 /* Produce una lista potencialmente vacía de expresiones separadas por comas */
explist:
  /* empty */  { $$ = new std::list<Expression*>(); }
| nonempty_explist

 /* Produce una lista no vacía de expresiones separadas por comas */
nonempty_explist:
  expr
  { std::list<Expression*> *l = new std::list<Expression*>();
    l->push_back($1);
    $$ = l;}
| nonempty_explist "," expr
    { $1->push_back($3);
      $$ = $1; }

%%

void yyerror (char const *s) {
  program.error(s, yylloc.first_line, yylloc.first_column);
  exit(1);
}

// Por ahora el main está aquí, pero luego hay que moverlo
int main (int argc, char **argv) {
  if (argc == 2) {
    yyin = fopen(argv[1], "r");
  }

  program.errorCount = 0;

  // Agregar las funciones embebidas del lenguaje
 
  //Argumentos Dummy de cada tipo
  SymVar *argInt= new SymVar("a",0,0,true,0);
  SymVar *argFloat= new SymVar("b",0,0,true,0);
  SymVar *argChar= new SymVar("c",0,0,true,0);

  /* De verdad hace falta agregarlos a la tabla? 
  program.symtable.insert(argInt);
  program.symtable.insert(argFloat);
  program.symtable.insert(argChar);
  */

  // Bueno, hacer &() a los getInstance se está poniendo fastidioso
  // pero ya me da fastidio cambiar las firmas de los getInstance - JA
  argInt->setType(&(IntType::getInstance()));
  argFloat->setType(&(FloatType::getInstance()));
  argChar->setType(&(CharType::getInstance()));

  listSymPairs *listargInt= new listSymPairs();
  listSymPairs *listargFloat= new listSymPairs();
  listSymPairs *listargChar= new listSymPairs();

  listargInt->push_back(std::pair<PassType,SymVar*>(PassType::normal,argInt));
  listargFloat->push_back(std::pair<PassType,SymVar*>(PassType::normal,argFloat));
  listargChar->push_back(std::pair<PassType,SymVar*>(PassType::normal,argChar));

  SymFunction *inttofloat= new SymFunction("inttofloat",0,0,listargInt);
  SymFunction *floattoint= new SymFunction("floattoint",0,0,listargFloat);
  SymFunction *chartoint= new SymFunction("chartofloat",0,0,listargChar);
  SymFunction *inttochar= new SymFunction("floattochar",0,0,listargInt);

  inttofloat->setType(&(FloatType::getInstance()));
  floattoint->setType(&(IntType::getInstance()));
  chartoint->setType(&(IntType::getInstance()));
  inttochar->setType(&(CharType::getInstance()));

  program.symtable.insert(inttofloat);
  program.symtable.insert(floattoint);
  program.symtable.insert(chartoint);
  program.symtable.insert(inttochar);

  yyparse();

  // Segunda vuelta haciendo chequeos semánticos

  // Chequear que existe una función llamada main()
  SymFunction *main= program.symtable.lookup_function("main");
  if(main==NULL){
    std::cerr << "Error: No se ha definido la función main." << std::endl;
  }else{
    int line= main->getLine();
    int col= main->getColumn();
    // Si existe, verificar que no tenga agumentos y que sea tipo int
    if(main->getArgumentCount()!=0){
      program.error("la funcion main no debe tener argumentos",line,col);
    }
    IntType& i = IntType::getInstance();
    if(!(*(main->getType()) == i)){
      main->getType()->print();
      program.error("La funcion main debe ser de tipo 'int'",line,col);
    }
  }
  // Chequear el AST 
  program.check();

  // Si hay algun error, no imprimir el árbol.
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

  program.symtable.print();

  return 0;
}
