#ifndef DEVANIX_EXPRESSION
#define DEVANIX_EXPRESSION

#include "symbol.hh"
#include "type.hh"
#include "Quad.hh"

/**
 * Representa una expresión en Devanix.
 * Las expresiones pueden actuar como r-values o l-values dependiendo del
 * contexto.
 */
class Expression {
protected:
  Type* type;
  int fline, fcol, lline, lcol; // Ubicación en el código fuente
  /* El tipo se inicializa en NULL porque si la expresión usa funciones o box
   * definidos más abajo sería imposible determinar el tipo al momento de
   * parsear la expresión, entonces se delega el cálculo del tipo al método
   * check()
   */
  Expression() : fline(0), fcol(0), lline(0), lcol(0), type(NULL) {};
public:
  virtual void print(int nesting);
  void setLocation(int fline, int fcol, int lline, int lcol);
  int getFirstLine();
  int getFirstCol();

  // Chequea que las subexpresiones sean correctos y determina el tipo de
  // esta expresión
  virtual void check();
  virtual Type* getType();

  // Devuelve una nueva expresión que es el resultado de hacer constant folding
  // sobre esta expresión.
  virtual Expression* cfold();
  // Determina si esta expresión es un valor constante
  virtual bool isConstant();
  // En caso de ser constante, estos métodos permiten obtener su valor
  // Hay que estar seguro del tipo de la expresión antes de invocar alguno de
  // estos métodos, pues si es un IntExp y se pide getFloat(), devolverá un
  // valor default probablemente inútil.
  virtual int getInteger();
  virtual double getFloat();
  virtual bool getBool();
  // faltaría un getChar(), lo haremos luego

  // Determina puede representar un lugar en memoria
  virtual bool isLvalue();
  // Devuelve el lugar en memoria que representa este l-value
  virtual int getLvalue();

  // Determina si la expresión es asignable, es decir, no es un l-value que
  // utilice una variable readonly.
  virtual bool isAssignable();
  // Crea la instruccion de tres direcciones asociada a la expresion
  virtual SymVar* gen();
  virtual void jumping(Label* lbltrue,Label* lblfalse);
};

// Expresión errónea (cuando se usa un símbolo que no existe)
class BadExp : public Expression {
public:
  BadExp();
  virtual bool isBad(); // obsoleto
};

// Una variable
class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
  SymVar* getSym();
  virtual void print(int nesting);
  virtual bool isLvalue();
  virtual bool isAssignable();
  virtual SymVar* gen();
};

// Expresiones con valor constantes
class Constant : public Expression {
public:
  virtual bool isConstant();
  
};

// Número entero
class IntExp : public Constant {
private:
  int value;
public:
  IntExp(int value);
  virtual void print(int nesting);
  virtual int getInteger();
  virtual SymVar* gen();
};

// Número decimal
class FloatExp : public Constant {
private:
  float value;
public:
  FloatExp(float value);
  virtual void print(int nesting);
  virtual double getFloat();
  virtual SymVar* gen();
};


// Valor booleano
class BoolExp : public Constant {
private:
  bool value;
public:
  BoolExp(bool value);
  virtual void print(int nesting);
  virtual bool getBool();
  virtual SymVar* gen();
};

// Una cadena de caracteres de longitud arbitraria
class StringExp : public Constant {
private:
  std::string str;
public:
  StringExp(std::string str);
  virtual int getLength();
  virtual void print(int nesting);
  virtual SymVar* gen();
};

// Un caracter
class CharExp : public Constant {
private:
  std::string ch; // cambiar a char
public:
  CharExp(std::string ch);
  virtual void print(int nesting);
  virtual SymVar* gen();
};

// Operadores binarios
// Realmente la mayor utilidad de esta clase es poder imprimir de manera fácil
// y sencilla las expresiones, sin tener que repetir demasiado código.
class BinaryOp : public Expression {
protected:
  Expression* exp1;
  Expression* exp2;
  std::string op;
  BinaryOp(Expression* exp1, Expression* exp2, std::string op)
    : exp1(exp1), exp2(exp2), op(op) {};
public:
  virtual void print(int nesting);
};

// Operadores Aritméticos
class Arithmetic : public BinaryOp {
protected:
  Operator opI;
  Operator opF;
  Arithmetic(Expression* e1, Expression* e2, std::string op, Operator opI,
	     Operator opF)
    : BinaryOp(e1,e2,op),opI(opI),opF(opF){};
public:
  virtual void check();
  virtual SymVar* gen();
};

class Sum : public Arithmetic {
public:
  Sum(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"+",sumI,sumF) {};
  virtual Expression* cfold();
  //virtual void gen();
};

class Substraction : public Arithmetic {
public:
  Substraction(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"-", substractionI,
							    substractionF) {};
  virtual Expression* cfold();
  //virtual void gen();
};

class Multiplication : public Arithmetic {
public:
  Multiplication(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"*",
							      multiplicationI,
							      multiplicationF) {};
  virtual Expression* cfold();
  //virtual void gen();
};

class Division : public Arithmetic {
public:
  Division(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"/",divisionI,
							divisionF) {};
  virtual Expression* cfold();
  //virtual void gen();
};

// Operador resto (%)
class Remainder : public Arithmetic {
public:
  Remainder(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"%",remainder,
							 remainder) {};
  virtual void check();
  virtual Expression* cfold();
  virtual SymVar* gen();
};

// Menos unario
class Minus : public Arithmetic {
public:
  Minus(Expression* e) : Arithmetic(e,e,"-",minusI,minusF) {};
  virtual void check();
  virtual Expression* cfold();
  void print(int nesting);
  virtual SymVar* gen();
};

// Operadores lógicos AND, OR, NOT
class Logical : public BinaryOp {
protected:
  Logical(Expression* e1, Expression* e2, std::string op)
    : BinaryOp(e1,e2,op) {};
  virtual void check();
  virtual SymVar* gen();
};

class And : public Logical {
public:
  And(Expression* e1, Expression* e2) : Logical(e1,e2,"and") {};
  virtual Expression* cfold();
  virtual void jumping(Label* lbltrue,Label* lblfalse);
};

class Or : public Logical {
public:
  Or(Expression* e1, Expression* e2) : Logical(e1,e2,"or") {};
  virtual Expression* cfold();
  virtual void jumping(Label* lbltrue,Label* lblfalse);
};

class Not : public Logical {
public:
  Not(Expression* e) : Logical(e,e,"not") {};
  virtual void check();
  virtual Expression* cfold();
  void print(int nesting);
  virtual SymVar* gen();
  virtual void jumping(Label* lbltrue,Label* lblfalse);
};

// Operadores relacionales
class Relational : public Logical {
protected:
  Relational(Expression* e1, Expression* e2, std::string op)
    : Logical(e1,e2,op) {};
  virtual void check();
  virtual SymVar* gen();
  virtual Operator operatortype()=0;
};

// >
class Greater : public Relational {
public:
  Greater(Expression* e1, Expression* e2) : Relational(e1,e2,">") {};
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// >=
class GreaterEq : public Relational {
public:
  GreaterEq(Expression* e1, Expression* e2) : Relational(e1,e2,">=") {};
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// ==
class Equal : public Relational {
public:
  Equal(Expression* e1, Expression* e2) : Relational(e1,e2,"==") {};
  virtual void check();
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// !=
class NotEqual : public Relational {
public:
  NotEqual(Expression* e1, Expression* e2) : Relational(e1,e2,"!=") {};
  virtual void check();
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// <
class Less : public Relational {
public:
  Less (Expression* e1, Expression* e2) : Relational(e1,e2,"<") {};
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// <=
class LessEq : public Relational {
public:
  LessEq(Expression* e1, Expression* e2) : Relational(e1,e2,"<=") {};
  virtual Expression* cfold();
  virtual Operator operatortype();
};

// Acceso a un arreglo, var[indice]
class Index : public Expression {
private:
  Expression* array;
  Expression* index;
public:
  Index(Expression* array, Expression* index)
    : array(array), index(index) {};
  virtual void check();
  void print(int nesting);
  virtual bool isLvalue();
  virtual bool isAssignable();
};

// Acceso a un campo de un box, box.campo
class Dot : public Expression {
private:
  Expression* box;
  std::string field;
public:
  Dot(Expression* box, std::string field) : box(box), field(field) {};
  virtual void check();
  void print(int nesting);
  virtual bool isLvalue();
  virtual bool isAssignable();
};


/**
 * Representa una llamada a función.
 */
class FunCallExp : public Expression {
private:
  SymFunction* symf;
  std::list<Expression*> args;

  /**
   * Las funciones son visibles en todo el programa, lo que permite usar
   * funciones que aún no han sido parseadas y por lo tanto no están en la tabla
   * de símbolos al momento de parsear una expresión que la llame.
   * Durante el parsing se intenta asociar de una vez esta expresión con su
   * símbolo de función correspondiente, pero si no se puede, se difiere el
   * el chequeo, y para ello se guarda el nombre de la función llamada en 'name'
   */
  std::string name;
  bool checkedFunction;
public:
  FunCallExp (SymFunction* symf, std::list<Expression*> args);
  FunCallExp (std::string name, std::list<Expression*> args);
  virtual Type* getType();
  virtual void check();
};

#endif
