#ifndef DEVANIX_QUAD
#define DEVANIX_QUAD

#include "instruction.hh"
#include "symbol.hh"
#include "label.hh"

class Quad : public Instruction {
public:
  Quad(){};
  virtual void printQuad();
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
  NOT,
  greater,  // >
  greaterEq,// >=
  equal,   // ==
  notEqual,// !=
  less,    // <
  lessEq,  // <=

  greaterF,
  greaterEqF,
  equalF,
  notEqualF,
  lessF,
  lessEqF
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
conststring,
null
};

/**
 * Clase que representa la asignacion de 3 o 2 direcciones
 * ejemplo: result:= arg1 op arg2
 *          result:= op arg1
 **/
class AsignmentOpQ:public Quad{
private:
  Operator op;
  Args arg1;
  ArgType arg1Type;
  Args arg2;
  ArgType arg2Type;
  SymVar *result;
public:
  AsignmentOpQ(SymVar* arg1,Operator op,SymVar* arg2,SymVar* result);
  AsignmentOpQ(ArgType arg1Type,Args arg1,Operator op,ArgType arg2Type,Args arg2,
	     SymVar* result):arg1Type(arg1Type),arg1(arg1),op(op),
			     arg2Type(arg2Type),arg2(arg2),result(result){};
  virtual void printQuad();
  virtual std::string toString();

  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
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
  virtual void printQuad();
  virtual std::string toString();

  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa la asignacion de referencias
 * ejemplo:  result:= *arg1
 **/
class AsignmentPointQ:public Quad{
private:
  SymVar *arg1;
  SymVar *result;
public:
  AsignmentPointQ(SymVar* arg1,SymVar* result):
    Quad(), arg1(arg1),result(result){};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa la asignacion a referencias de temporales
 * ejemplo:  *result:= arg1
 **/
class AsignmentToPointQ:public Quad{
private:
  Args arg1;
  ArgType arg1Type;
  SymVar *result;
public:
  AsignmentToPointQ(SymVar* arg1,SymVar* result);
  AsignmentToPointQ(ArgType arg1Type,Args arg1,SymVar* result):
    Quad(), arg1Type(arg1Type),arg1(arg1),result(result){};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa la asignacion de direccion
 * ejemplo:  result:= &arg1
 **/
class AsignmentAddQ:public Quad{
private:
  SymVar *arg1;
  SymVar *result;
public:
  AsignmentAddQ(SymVar* arg1,SymVar* result):
    Quad(), arg1(arg1),result(result){};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Representa la instruccion de salto incondicional
 * ejemplo: goto label
 */
class JumpQ:public Quad{
private:
  Label *label;
public:
  JumpQ(Label* label);
  virtual void printQuad();

  virtual bool isJump();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();
  virtual std::string toString();

  virtual Label* getTargetLabel();
  virtual void replaceTargetLabel(Label* l);

  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa el salto condicional
 * ejemplo: if arg1 op arg2 goto label
 * En donde op es un operador relacional.
 */
class ConditionalJumpQ:public Quad{
private:
  Operator op;
  Args arg1;
  ArgType arg1Type;
  Args arg2;
  ArgType arg2Type;
  Label *label;
public:
  ConditionalJumpQ(SymVar* arg1,Operator op,SymVar* arg2,Label* label);
  ConditionalJumpQ(ArgType arg1Type,Args arg1,Operator op,ArgType arg2Type,Args arg2,
		   Label* label): arg1Type(arg1Type),arg1(arg1),op(op),
				  arg2Type(arg2Type),arg2(arg2),label(label){};
  virtual void printQuad();

  virtual bool isJump();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();
  virtual std::string toString();

  virtual Label* getTargetLabel();
  virtual void replaceTargetLabel(Label* l);
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa el salto condicional negado
 * ejemplo: ifnot arg1 op arg2 goto label
 * En donde op es un operador relacional.
 */
class ConditionalNJumpQ:public Quad{
private:
  Operator op;
  Args arg1;
  ArgType arg1Type;
  Args arg2;
  ArgType arg2Type;
  Label *label;
public:
  ConditionalNJumpQ(SymVar* arg1,Operator op,SymVar* arg2,Label* label);
  ConditionalNJumpQ(ArgType arg1Type,Args arg1,Operator op,ArgType arg2Type,Args arg2,
		   Label* label): arg1Type(arg1Type),arg1(arg1),op(op),
				  arg2Type(arg2Type),arg2(arg2),label(label){};
  virtual void printQuad();

  virtual bool isJump();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();

  virtual std::string toString();

  virtual Label* getTargetLabel();
  virtual void replaceTargetLabel(Label* l);
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);

  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa un parametro de una funcion por valor
 *
 */
class ParamValQ: public Quad{
private:
  ArgType paramType;
  Args param;
public:
  ParamValQ(ArgType paramType,Args param):Quad(),param(param),paramType(paramType){};
  ParamValQ(SymVar *param);
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa un parametro de una funcion por referencia
 *
 */
class ParamRefQ: public Quad{
private:
  SymVar *param;
public:
  ParamRefQ(SymVar *param):Quad(),param(param){};
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa el prologo de la funcion func
 */
class PrologueQ: public Quad{
private:
  SymFunction *func;
public:
  PrologueQ(SymFunction *func): func(func) {};
  virtual void printQuad();

  virtual std::string toString();
};

/**
 * Clase que representa la llamada a una funcion
 * ejemplo: call func numParam
 */
class CallQ: public Quad{
private:
  int numParam;
  SymFunction *func;
public:
  CallQ(SymFunction *func,int n):func(func),numParam(n){};
  virtual void printQuad();

  virtual SymFunction* getCallTarget(); // Obsoleto
  virtual std::list<BasicBlock*> getTargetBlocks(); // Obsoleto

  virtual std::string toString();
  virtual bool isJumpTarget(); // Hack necesario para no dañar los registros
  // cuando se traduzcan las llamadas a funciones
  virtual std::list<Instruction*> gen();
};

class RetrieveQ : public Quad {
private:
  SymVar* var;
public:
  RetrieveQ(SymVar* var) : var(var) {};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa el retorno de una funcion
 */
class ReturnQ: public Quad{
private:
  ArgType argt;
  Args arg;
  SymFunction* symf;
public:
  ReturnQ(SymVar *result, SymFunction* symf);
  ReturnQ(ArgType argt, Args arg, SymFunction* symf);
  ReturnQ(SymFunction* symf);
  virtual void printQuad();

  virtual bool isJump();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();
  virtual bool isMainReturn();
  virtual bool isReturn();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa el acceso a un arreglo
 * ejemplo: result:= a[index]
 */
class IndexQ: public Quad{
private:
  SymVar *array;
  Args index;
  ArgType indexType;
  SymVar *result;
public:
  IndexQ(SymVar *array,ArgType indexType,Args index,SymVar *result):
    Quad(),array(array),indexType(indexType),index(index),result(result){};
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

/**
 * Clase que representa la asignacion a un arreglo
 * ejemplo: a[index]:= arg
 */
class IndexAsigQ: public Quad{
private:
  SymVar *array;
  Args index;
  ArgType indexType;
  Args arg;
  ArgType argType;
public:
  IndexAsigQ(SymVar *array,SymVar *index,SymVar *arg);
  IndexAsigQ(SymVar *array,ArgType indexType,Args index,ArgType argType,Args arg):
    Quad(),array(array),indexType(indexType),index(index),argType(argType),arg(arg){};
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

// write <type> <arg>
class WriteQ : public Quad {
private:
  ArgType argt;
  Args arg;
  Type* type;
  bool isLn;
public:
  WriteQ(ArgType argt, Args arg, Type* type, bool isLn);
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
  virtual std::list<Instruction*> gen();
};

// <result> := read <type>, si deref = false
// *<result> := read <type>, si deref = true
class ReadQ : public Quad {
private:
  SymVar* result;
  Type* type;
  bool deref;
public:
  ReadQ(SymVar* result, Type* type, bool deref);
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

// <result>[<index>] := read <type>
class ReadIndexQ : public Quad {
private:
  ArgType indext;
  Args index;
  SymVar* result;
  Type* type;
public:
  ReadIndexQ(SymVar* result, ArgType indext, Args index, Type* type);
  virtual void printQuad();

  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

// a := (float to int) b
class CastFtoIQ : public Quad {
private:
  SymVar* result;
  ArgType argt;
  Args arg;
public:
  CastFtoIQ(SymVar* result, ArgType argt, Args arg) : result(result),
						      argt(argt),
						      arg(arg) {};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

// a := (int to float) b
class CastItoFQ : public Quad {
private:
  SymVar* result;
  ArgType argt;
  Args arg;
public:
  CastItoFQ(SymVar* result, ArgType argt, Args arg) : result(result),
						      argt(argt),
						      arg(arg) {};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

// a := (char to int) b
class CastCtoIQ : public Quad {
private:
  SymVar* result;
  ArgType argt;
  Args arg;
public:
  CastCtoIQ(SymVar* result, ArgType argt, Args arg) : result(result),
						      argt(argt),
						      arg(arg) {};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

// a := (int to char) b
class CastItoCQ : public Quad {
private:
  SymVar* result;
  ArgType argt;
  Args arg;
public:
  CastItoCQ(SymVar* result, ArgType argt, Args arg) : result(result),
						      argt(argt),
						      arg(arg) {};
  virtual void printQuad();
  virtual std::string toString();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> out);
};

#endif
