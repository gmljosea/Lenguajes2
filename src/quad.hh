#ifndef DEVANIX_QUAD
#define DEVANIX_QUAD

#include "symbol.hh"

class Quad{

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
 * Clase que representa la asignacion de 3 o 2 direcciones
 * ejemplo: result:= arg1 op arg2  
 *          result:= op arg1
 **/ 
class AsignmentOpQ: Quad{
private:
  Operator op;
  SymVar *arg1;
  SymVar *arg2;
  SymVar *result; 
public:
  AsignmentOpQ(SymVar* arg1,Operator op,SymVar* result): 
    op(op), arg1(arg1),result(result){};
  AsignmentOpQ(SymVar* arg1,Operator op,SymVar* arg2,SymVar* result):
    arg1(arg1),op(op),arg2(arg2),result(result){};
};

/** 
 * Clase que representa la asignacion de copia
 * ejemplo:  result:= arg1
 **/ 
class AsignmentQ: Quad{
private:
  Args arg1;
  ArgType arg1Type;
  SymVar *result; 
public:
  AsignmentQ(ArgType arg1Type,Args arg1,SymVar* result): 
    arg1Type(arg1Type), arg1(arg1),result(result){};
};


/*
class JumpQ: Quad{
private:
  Label *label;
public:
  JumpQ(Label* label): label(label);
};

class ConditionalJumpQ: Quad{
private:
  Operator op;
  Args arg1;
  ArgType arg1Type;
  Args arg2;
  ArgType arg2Type;
  Label *label;
public:
 ConditionalJumpQ(ArgType arg1Type,Args arg1,Operator op,ArgType arg2Type,
		  Args arg2,Label* label): arg1Type(arg1Type),arg1(arg1),op(op),
			     arg2Type(arg2Type),arg2(arg2),label(label);
}; 
  */

#endif
