#ifndef DEVANIX_QUAD
#define DEVANIX_QUAD

class Quad{

};

enum Operator{
  sum,
  Substraction,
  Multiplication,
  Division,
  Remainder, // Operador 'resto' o mod
  Minus, // Menos unario
  And,
  Or,
  Greater,  // >
  GreaterEq,// >=
  Equal,   // ==
  NotEqual,// !=
  Less,    // <
  LessEq  // <=
};

enum ArgType{
  id,
  constint,
  contfloat
  //temporal
};

union Args{
  SymVar *id;
  int constint;
  float constfloat;
  bool constbool;
  std::string conststring;
  char constchar;
  // temporal?
};

/** 
 * Clase que representa la asignacion de copia o la de 3 o 2 direcciones
 * ejemplo: result:= arg1 op arg2  
 *          result:= op arg1
 *          result:= arg1
 **/ 
/*
class AsignmentQ: Quad{
private:
  Operator op;
  Args arg1;
  ArgType arg1Type;
  Args arg2;
  ArgType arg2Type;
  SymVar *result; // O temporal si decidimos hacer la clase 
public:
  AsignmentQ(ArgType arg1Type,Args arg1,SymVar* result): 
    arg1Type(arg1Type), arg1(arg1),result(result);
  AsignmentQ(Operator op,ArgType arg1Type,Args arg1,SymVar* result): 
    op(op),arg1Type(arg1Type), arg1(arg1),result(result);
  AsignmentQ(ArgType arg1Type,Args arg1,Operator op,ArgType arg2Type,Args arg2,
	     SymVar* result):arg1Type(arg1Type),arg1(arg1),op(op),
			     arg2Type(arg2Type),arg2(arg2),result(result);
};

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
