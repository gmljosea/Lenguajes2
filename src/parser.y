%defines
%output "parser.cc"
%error-verbose

%locations

%code requires {
#include <cstdio>
#include <iostream>
#include <list>
#include <set>
#include <string>

#include "flowgraph.hh"
#include "expression.hh"
#include "program.hh"
#include "statement.hh"
#include "type.hh"
#include "IntermCode.hh"
}

%code {

int yylex();
void yyerror (char const *);
extern FILE *yyin;

// Resultado del parseo
Program program;

// Variables globales útiles para chequeos durante el parseo
SymFunction* currentfun; // Función parseada actual
std::list<Iteration*> loopstack;

/* Como todos los box se pueden ver entre si, puede que dentro de un box
   se declare una variable de un tipo box cuya declaracion aun no se ha
   encontrado, en este caso lo guardamos mientras en el hash de unknown
   con la condicion de que cuando se encuentre su definicion sea movido a
   la tabla de simbolos*/
 boxHash unknownBox;
 funcSymtable unknownFunc;

 /* Variable global que sera modificada por los metodos gen() de cada 
  * instruccion en el arbol sintactico.
   */
 IntermCode intCode;

/**
 * Extrae los campos de yylloc y los utiliza para inicializar los campos
 * de ubicación de un Statement cualquiera.
 */
void setLocation(Statement* stmt, YYLTYPE* yylloc) {
  stmt->setLocation(yylloc->first_line, yylloc->first_column, yylloc->last_line,
                    yylloc->last_column);
}

Iteration* findLabel(std::string label) {
  for (std::list<Iteration*>::iterator it = loopstack.begin();
       it != loopstack.end(); it++) {
    if (*((*it)->getLabel()) == label) {
      return *it;
    }
  }
  return NULL;
}

/**
 * En los std::to_string()
 * tuve que usar el horrible cast a long long int porque era la única manera de que compilara
 * en g++ 4.4.5 (el que viene en Debian estable), supongo que porque el soporte para C++ 2011
 * está un poco fail en esa versión.
 * Al menos desde g++ 4.5.2, la que tengo en mi desktop, compila sin problemas sin poner el cast,
 * Por eso nunca entendimos el error que nos decia el prof. sobre el to_string.
 */

void pushLoop(Iteration* loop, YYLTYPE* yylloc) {
  std::string* label = loop->getLabel();
  if (label) {
    Iteration* it = findLabel(*(loop->getLabel()));
    if (it) {
      program.error("etiqueta '"+*label+"' ya fue utilizada en "
                    +std::to_string((long long int) it->getFirstLine())+":"
                    +std::to_string((long long int) it->getFirstCol()),
                    yylloc->first_line, yylloc->first_column);
    }
  }
  loopstack.push_front(loop);
}

Iteration* popLoop() {
  Iteration* it = loopstack.front();
  loopstack.pop_front();
  return it;
}

Iteration* topLoopstack() {
  return loopstack.size()>0 ? loopstack.front() : NULL;
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
      +id+"' previamente declarada en "+std::to_string((long long int) symf->getLine())
      +":"+std::to_string((long long int) symf->getColumn());
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
      +id+"' previamente declarada en "+std::to_string((long long int) symv->getLine())
      +":"+std::to_string((long long int) symv->getColumn());
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
      +id+"' previamente declarado en "+std::to_string((long long int) boxtype->getLine())
      +":"+std::to_string((long long int) boxtype->getColumn());
    program.error(err, yylloc.first_line, yylloc.first_column);
    return true;
  }
  return false;
}

}

%union {
  int ival;
  char chr;
  double fval;
  std::string *str;
  Statement *stmt;
  Block *blk;
  Expression *exp;
  Type *type;
  std::list<Expression*> *exps;
  std::list<std::pair<SymVar*,Expression*>> *decls;
  std::pair<SymVar*,Expression*> *decl;
  PassType passtype;
  ArgList *argsdec;
  BoxType *box;
  SymVar* arg;
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
%token TK_EQU         "=="
%token TK_NEQ         "!="
%token TK_LT          "<"
%token TK_GT          ">"
%token TK_GTE         ">="
%token TK_LTE         "<="
%token TK_AND         "and"
%token TK_OR          "or"
%token TK_NOT         "not"

%token TK_ASIGN       "="

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
%token <str> TK_ID    "identificador"

// Token de un string cualquiera encerrado entre comillas
 // No confundirse con TK_STRING que se refiere a la palabra reservada 'string'
%token <str> TK_CONSTSTRING "cadena de caracteres"
%token <str> TK_CONSTCHAR   "caracter"

// Tokens de constantes numéricas
%token <ival> TK_CONSTINT   "número entero"
%token <fval> TK_CONSTFLOAT "número decimal"

// Segun el manual este token es para errores mas bonitos
%token END 0 "fin de archivo"

%type <stmt> statement if while for variabledec asignment foreach
%type <blk> stmts block else
%type <exp> expr funcallexp step
%type <str> label brk_nxt_label
%type <type> type
%type <exps> explist nonempty_explist
%type <decls> vardec_items
%type <decl> vardec_item
%type <passtype> passby
%type <argsdec> args nonempty_args
%type <box> box
%type <arg> argument

%left "or"
%left "and"
%left "+" "-"
%left "*" "/" "%"
%right NEG "not"
%left "==" "!="
%left "<" "<=" ">=" ">"
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
| error
| globals global
| globals error

 /* Produce una declaración de función, variable global o box */
global:
  variabledec
   { VariableDec* vd = dynamic_cast<VariableDec*>($1);
     vd->setGlobal(true);
     program.globalinits.push_back(vd); }

| type TK_ID enterscope "(" args ")"
    { /* Si una función se redeclara, no se inserta en la tabla de símbolos,
         Pero si instanciamos el SymFunction y actualizamos la variable
         currentfun para por lo menos poder chequear los return.
       */
      SymFunction* sym = new SymFunction(*$2, $5, $1, @2.first_line,
                                         @2.first_column);
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
| "box" TK_ID box "{" boxdecsa variantpart "}"
{
  program.boxes.push_back($3);
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
    $$= new BoxType(*($<str>0),false);
  }
  $$->setLine(@0.first_line);
  $$->setColumn(@0.first_line);
  if(!boxRedeclared(*($<str>0),@$)){
    program.symtable.insert($$);
  }
}

boxdecsa:
/*empty*/
|boxdecs

boxdecs:
  type TK_ID ";"
  {
    /* Se agregan los campos del box usando el BoxType almacenado en
     la pila. Se accede a traves de $<box>-1 */
    BoxField *field= $<box>-1->getField(*$2);
    if(field==NULL){
      $<box>-1->addFixedField($1,*$2,@2.first_line,@2.first_column);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
	+":"+(std::string) std::to_string((long long int) field->column);
    program.error(err, @2.first_line, @2.first_column);
    }

  }
| boxdecs type TK_ID ";"
{
 BoxField *field= $<box>-1->getField(*$3);
    if(field==NULL){
      $<box>-1->addFixedField($2,*$3,@3.first_line,@3.first_column);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$3)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
	+":"+(std::string) std::to_string((long long int) field->column);
    program.error(err, @3.first_line, @3.first_column);
    }
}
| error ";"


variantpart:
/*empty*/
|"variant" ":" variantpart_decs

variantpart_decs:
  dummy "{" variantdecs "}"
|  type TK_ID ";"
{
  BoxField *field= $<box>-4->getField(*$2);
    if(field==NULL){
      $<box>-4->addVariantField($1,*$2,false,@2.first_line,@2.first_column);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
	+":"+(std::string) std::to_string((long long int) field->column);
    program.error(err, @2.first_line, @2.first_column);
    }
}
|  variantpart_decs "{" variantdecs "}"
|  variantpart_decs type TK_ID ";"
{
  BoxField *field= $<box>-4->getField(*$3);
  if(field==NULL){
    $<box>-4->startGrouping();
    $<box>-4->addVariantField($2,*$3,true,@2.first_line,@2.first_column);
  }else{
    std::string err = "redeclaración de variable '"
      +(*$3)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
      +":"+(std::string) std::to_string((long long int) field->column);
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
      $<box>-6->addVariantField($1,*$2,true,@2.first_line,@2.first_column);
    }else{
      std::string err = "redeclaración de variable '"
        +(*$2)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
	+":"+(std::string) std::to_string((long long int) field->column);
    program.error(err, @2.first_line, @2.first_column);
    }
}
|variantdecs type TK_ID ";"
{
  BoxField *field= $<box>-6->getField(*$3);
  if(field==NULL){
    /*    $<box>-6->startGrouping();*/
    $<box>-6->addVariantField($2,*$3,true,@2.first_line,@2.first_column);
  }else{
    std::string err = "redeclaración de variable '"
      +(*$3)+"' previamente declarada en "+(std::string) std::to_string((long long int) field->line)
      +":"+(std::string) std::to_string((long long int) field->column);
    program.error(err, @3.first_line, @3.first_column);
  }
}
| error 

dummy:
/*empty*/ // Regla dummy para 'emparejar' la pila

 /* Produce una lista de declaraciones de argumentos (SymVar*)
  * de una función, posiblemente vacía. */
args:
  /* empty */    { $$ = new ArgList(); }
| nonempty_args

 /* Produce una lista de declaraciones de argumentos de una función con al menos
    un argumento. */
nonempty_args:
  argument
    { ArgList* args = new ArgList();
      if ($1) args->push_back($1); $$ = args; }
| nonempty_args "," argument
    { if ($3) $1->push_back($3); $$ = $1; }

argument:
  passby type TK_ID
    { if (!variableRedeclared(*$3, @3)) {
        SymVar* arg = new SymVar(*$3, @3.first_line, @3.first_column, true,
                                 program.symtable.current_scope());
        arg->setType($2);
        switch ($1) {
        case PassType::readonly:
          arg->setReadonly(true);
          break;
        case PassType::reference:
          arg->setReference(true);
          break;
        default:
          arg->setReference($2->alwaysByReference());
        }
        program.symtable.insert(arg);
        $$ = arg;
      } else {
	$$ = NULL;
      }
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
    { $$ = new Block(program.symtable.current_scope(), new Null());
      setLocation($$, &@$);
    }
| "{" error "}"
    { $$ = new Block(program.symtable.current_scope(), new Null());
      setLocation($$, &@$);
    }
| error "}"
    { $$ = new Block(program.symtable.current_scope(), new Null());
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
| "break" brk_nxt_label ";"
    { Iteration* enclosing = topLoopstack();
      if (!enclosing) {
        program.error("el break no está dentro de una iteración",
                      @1.first_line, @1.first_column);
        $$ = new Null();
      } else if ($2) {
        Iteration* it = findLabel(*$2);
        if (!it) {
          program.error("no se encuentra la etiqueta '"+*$2+"'",
                        @2.first_line, @2.first_column);
          $$ = new Null();
        } else {
          $$ = new Break($2, it);
        }
      } else {
        $$ = new Break($2, NULL);
      }
    }
| "next" brk_nxt_label ";"
    { Iteration* enclosing = topLoopstack();
      if (!enclosing) {
        program.error("el next no está dentro de una iteración",
                      @1.first_line, @1.first_column);
        $$ = new Null();
      } else if ($2) {
        Iteration* it = findLabel(*$2);
        if (!it) {
          program.error("no se encuentra la etiqueta '"+*$2+"'",
                        @2.first_line, @2.first_column);
          $$ = new Null();
        } else {
          $$ = new Next($2, it);
        }
      } else {
        $$ = new Next($2, NULL);
      }
    }
| "return" expr ";"
    { $$ = new Return(currentfun, $2); }
| "return" ";"
    { $$ = new Return(currentfun); }
| "write" nonempty_explist ";"
    { $$ = new Write(*$2,false); }
| "writeln" nonempty_explist ";"
    { $$ = new Write(*$2,true); }
| "read" expr ";"
    { $$ = new Read($2); }
| if
| while
| for
| foreach
| variabledec
| asignment
| error ";"    { $$ = new Null(); yyerrok; }

brk_nxt_label:
  TK_ID
| /* empty */ { $$ = NULL; }

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
 label "while" expr
  { While* w = new While($1, $3, NULL);
    pushLoop(w, &@1);
    setLocation(w, &@1);
  }
 enterscope block leavescope
  { if ($1 == NULL) {
      @$.first_line = @2.first_line;
      @$.first_column = @2.first_column;
      @$.last_line = @7.last_line;
      @$.last_column = @7.last_column;
    }
    Iteration* w = popLoop();
    w->setBlock($6);
    $$ = w;
 }

for:
  label "for" TK_ID "in" expr ".." expr step
    { /* Meter variable de iteración en la tabla antes de revisar las
         instrucciones */
      SymVar* loopvar = new SymVar(*$3, @3.first_line, @3.first_column, false,
                                   program.symtable.current_scope());
      loopvar->setType(&(IntType::getInstance()));
      loopvar->setReadonly(true);
      program.symtable.insert(loopvar);
      BoundedFor* bf = new BoundedFor($1, loopvar, $5, $7, $8, NULL);
      pushLoop(bf, &@1);
      setLocation(bf, &@1);
    }
  enterscope block leavescope
    { if ($1 == NULL) {
        @$.first_line = @2.first_line;
        @$.first_column = @2.first_column;
        @$.last_line = @12.last_line;
        @$.last_column = @12.last_column;
      }
      Iteration* w = popLoop();
      w->setBlock($11);
      $$ = w;
    }

foreach:
  label "for" TK_ID "in" expr
    {
      SymVar* loopvar = new SymVar(*$3, @3.first_line, @3.first_column, false,
                                   program.symtable.current_scope());
      loopvar->setType(&(ErrorType::getInstance()));
      loopvar->setReference(true);
      program.symtable.insert(loopvar);
      ForEach* fe = new ForEach($1, loopvar, $5, NULL);
      pushLoop(fe, &@1);
      setLocation(fe, &@1);
    }
  enterscope block leavescope
    { if ($1 == NULL) {
        @$.first_line = @2.first_line;
        @$.first_column = @2.first_column;
        @$.last_line = @9.last_line;
        @$.last_column = @9.last_column;
      }
      Iteration* w = popLoop();
      w->setBlock($8);
      $$ = w;
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
  nonempty_explist "=" nonempty_explist ";"
  { $$ = new Asignment(*$1, *$3); }

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
        $$ = new std::pair<SymVar*,Expression*>(sym,(Expression*) NULL);
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
 "int"     { $$ = &(IntType::getInstance()); }
| "char"   { $$ = &(CharType::getInstance()); }
| "bool"   { $$ = &(BoolType::getInstance()); }
| "float"  { $$ = &(FloatType::getInstance()); }
| "string" { $$ = new StringType(1);}
| "void"   { $$ = &(VoidType::getInstance()); }
| type "array" "[" TK_CONSTINT "]"
{
  $$ = new ArrayType($1,$4,@1.first_line,@1.first_column);
  $$->check();
}
| type "array" "[" "]"
{
  $$ = new ArrayType($1,0,@1.first_line,@1.first_column);
  $$->check();
}
| TK_ID
{
  if(($$= program.symtable.lookup_box(*$1))== NULL){
    boxHash::iterator it= unknownBox.find(*$1);
    if(it!= unknownBox.end())
      $$= it->second;
    else{
      BoxType *newbox= new BoxType(*$1,true);
      unknownBox.insert(boxHash::value_type(*$1,newbox));
      $$= newbox;
    }
  }
}

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
| TK_TRUE        { $$ = new BoolExp(true);
                   $$->setLocation(@1.first_line, @1.first_column,0,0);}
| TK_FALSE       { $$ = new BoolExp(false);
                   $$->setLocation(@1.first_line, @1.first_column,0,0);}
| TK_CONSTSTRING { $$ = new StringExp(*$1);
                   $$->setLocation(@1.first_line, @1.first_column,0,0);}
| TK_CONSTCHAR   { char chrv= $1->at(1);
                   $$ = new CharExp(chrv);
                   $$->setLocation(@1.first_line, @1.first_column,0,0); }

| funcallexp

| expr "+" expr  { $$ = new Sum($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0); }

| expr "-" expr { $$ = new Substraction($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "*" expr { $$ = new Multiplication($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "/" expr { $$ = new Division($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "%" expr { $$ = new Remainder($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| "-" expr %prec NEG { $$ = new Minus($2);
                       $$->setLocation(@1.first_line, @1.first_column,0,0);}
| "(" expr ")" { $$ = $2;
                 $$->setLocation(@2.first_line, @2.first_column,0,0);}
  /*
   * Las expresiones lógicas están juntas con las aritméticas, no las separamos
   * en reglas diferentes. La razón fue que como existe el tipo bool, entonces
   * cualquier variable, operador [] sobre arreglo o campo de box puede
   * potencialmente ser una expresión booleana. Aparte el frontend de ejemplo
   * del Aho no las separaba.
   * Si luego esto resulta problemático para generar código entonces lo
   * cambiaremos.
   */
| expr "and" expr { $$ = new And($1,$3);
                    $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "or" expr { $$ = new Or($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| "not" expr { $$ = new Not($2);
               $$->setLocation(@1.first_line, @1.first_column,0,0);}
| expr ">" expr { $$ = new Greater($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr ">=" expr { $$ = new GreaterEq($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "==" expr { $$ = new Equal($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "!=" expr { $$ = new NotEqual($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "<" expr { $$ = new Less($1,$3);
                  $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "<=" expr { $$ = new LessEq($1,$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "[" expr "]" { $$ = new Index($1,$3);
                      $$->setLocation(@2.first_line, @2.first_column,0,0);}
| expr "." TK_ID { $$ = new Dot($1,*$3);
                   $$->setLocation(@2.first_line, @2.first_column,0,0);}
| error { $$ = new BadExp();
              $$->setLocation(@1.first_line, @1.first_column,0,0);}

 /* Produce una llamada a función */
funcallexp:
  TK_ID "(" explist ")"
    { SymFunction* symf = program.symtable.lookup_function(*$1);
      if (symf == NULL) {
        $$ = new FunCallExp(*$1, *$3);
      } else {
        $$ = new FunCallExp(symf, *$3);
      }
      $$->setLocation(@1.first_line, @1.first_column,0,0);
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
     --> Cuando generemos código vemos como manejamos las funciones
     de cast, porque también sería ineficiente generar una llamada a
     función para una simple conversión.
  program.symtable.insert(argInt);
  program.symtable.insert(argFloat);
  program.symtable.insert(argChar);
  */

  // pero ya me da fastidio cambiar las firmas de los getInstance - JA
  argInt->setType(&(IntType::getInstance()));
  argInt->setReference(false);
  argFloat->setType(&(FloatType::getInstance()));
  argFloat->setReference(false);
  argChar->setType(&(CharType::getInstance()));
  argChar->setReference(false);

  ArgList *listargInt = new ArgList();
  ArgList *listargFloat = new ArgList();
  ArgList *listargChar = new ArgList();

  listargInt->push_back(argInt);
  listargFloat->push_back(argFloat);
  listargChar->push_back(argChar);

  SymFunction *inttofloat
    = new SymFunction("inttofloat",listargInt, &(FloatType::getInstance()),0,0);
  SymFunction *floattoint
    = new SymFunction("floattoint",listargFloat, &(IntType::getInstance()),0,0);
  SymFunction *chartoint
    = new SymFunction("chartoint",listargChar, &(IntType::getInstance()), 0,0);
  SymFunction *inttochar
    = new SymFunction("inttochar", listargInt, &(CharType::getInstance()),0,0);

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
  /*
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

  std::cout << std::endl << "-- Tipos Box definidos --" << std::endl << std::endl;

  for (std::list<BoxType*>::iterator it = program.boxes.begin();
       it != program.boxes.end(); it++) {
    (**it).printDetail();
  }

  //program.symtable.print();

  // Generar codigo intermedio */
  program.gen();

  return 0;

}
