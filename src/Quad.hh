#ifndef DEVANIX_QUAD
#define DEVANIX_QUAD

#include "symbol.hh"

class Quad{
public:
  Quad(){};
};

enum Operator{
  sumI,
  sumF,
  substractionI,
  substractionF,
  multiplicationI,
  multiplicationF,
  divisionI,
  divisionF,
  remainder, // Operador 'resto' o mod
  minusI, // Menos unario
  minusF,
  AND,
  OR,
  greater,  // >
  greaterEq,// >=
  equal,   // ==
  notEqual,// !=
  less,    // <
  lessEq  // <=
};


union Args{
  SymVar *id;
  int constint;
  float constfloat;
  bool constbool;
  std::string* conststring;
  char constchar;
};

enum ArgType{	
id,
constint,
constfloat,
constchar,
constbool,
conststring
};

/** 
 * Representa una etiqueta asociada a una instruccion 
 */
class Label {
private:
  Quad* instruction;
  int id;
public:
  Label(int id): id(id){};
  void setInstruction(Quad* quad);
  int getId();
};

/** 
 * Clase que representa la asignacion de 3 o 2 direcciones
 * ejemplo: result:= arg1 op arg2  
 *          result:= op arg1
 **/ 
class AsignmentOpQ:public Quad{
private:
  Operator op;
  SymVar *arg1;
  SymVar *arg2;
  SymVar *result; 
public:
  AsignmentOpQ(SymVar* arg1,Operator op,SymVar* result): 
    Quad(), op(op), arg1(arg1),result(result){};
  AsignmentOpQ(SymVar* arg1,Operator op,SymVar* arg2,SymVar* result):
    Quad(), arg1(arg1),op(op),arg2(arg2),result(result){};
};

/** 
 * Clase que representa la asignacion de copia
 * ejemplo:  result:= arg1
 **/ 
class AsignmentQ:public Quad{
private:
  Args arg1;
  ArgType arg1Type;
  SymVar *result; 
public:
  AsignmentQ(ArgType arg1Type,Args arg1,SymVar* result): 
    Quad(), arg1Type(arg1Type), arg1(arg1),result(result){};
};


/**
 * Representa la instruccion de salto incondicional
 * ejemplo: goto label
 */
class JumpQ:public Quad{
private:
  Label *label;
public:
  JumpQ(Label* label): Quad(), label(label){};
};

class ConditionalJumpQ:public Quad{
private:
  Operator op;
  SymVar* arg1;
  SymVar* arg2;
  Label *label;
public:
 ConditionalJumpQ(SymVar* arg1,Operator op,SymVar* arg2,Label* label):
   Quad(), arg1(arg1),op(op),arg2(arg2),label(label){};
}; 

class ConditionalNJumpQ:public Quad{
private:
  Operator op;
  SymVar* arg1;
  SymVar* arg2;
  Label *label;
public:
 ConditionalNJumpQ(SymVar* arg1,Operator op,SymVar* arg2,Label* label):
   Quad(), arg1(arg1),op(op),arg2(arg2),label(label){};
};


#endif
