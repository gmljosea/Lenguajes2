#ifndef DEVANIX_EXPRESSION
#define DEVANIX_EXPRESSION

#include "symbol.hh"
#include "type.hh"

// Clase base
class Expression {
protected:
  Type* type;
  int fline, fcol, lline, lcol;
  Expression() : fline(0), fcol(0), lline(0), lcol(0), type(NULL) {};
public:
  virtual void print(int nesting);
  void setLocation(int fline, int fcol, int lline, int lcol);
  int getFirstLine();
  int getFirstCol();

  virtual void check();
  virtual Type* getType();
  virtual bool isBad(); // obsoleto

  virtual Expression* cfold();
  virtual bool isConstant();
  virtual int getInteger();
  virtual double getFloat();
  virtual bool getBool();

};
/**
 * Cosas antes de que se me olviden:
 * El método check chequea los tipos
 */

// Expresión errónea (cuando se usa un símbolo que no existe)
class BadExp : public Expression {
public:
  BadExp();
  virtual bool isBad(); // obsoleto
};

// Variable
class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
  virtual void print(int nesting);
};

// Expresiones con valor constantes
class Constant : public Expression {
public:
  virtual bool isConstant();
};

class IntExp : public Constant {
private:
  int value;
public:
  IntExp(int value);
  virtual void print(int nesting);
  virtual int getInteger();
};

class FloatExp : public Constant {
private:
  float value;
public:
  FloatExp(float value);
  virtual void print(int nesting);
  virtual double getFloat();
};

class BoolExp : public Constant {
private:
  bool value;
public:
  BoolExp(bool value);
  virtual void print(int nesting);
  virtual bool getBool();
};

class StringExp : public Constant {
private:
  std::string str;
public:
  StringExp(std::string str);
  virtual void print(int nesting);
};

class CharExp : public Constant {
private:
  std::string ch; // cambiar a char
public:
  CharExp(std::string ch);
  virtual void print(int nesting);
};


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
  Arithmetic(Expression* e1, Expression* e2, std::string op)
    : BinaryOp(e1,e2,op) {};
public:
  virtual void check();
};

class Sum : public Arithmetic {
public:
  Sum(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"+") {};
  virtual Expression* cfold();
};

class Substraction : public Arithmetic {
public:
  Substraction(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"-") {};
  virtual Expression* cfold();
};

class Multiplication : public Arithmetic {
public:
  Multiplication(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"*") {};
  virtual Expression* cfold();
};

class Division : public Arithmetic {
public:
  Division(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"/") {};
  virtual Expression* cfold();
};

class Remainder : public Arithmetic {
public:
  Remainder(Expression* e1, Expression* e2) : Arithmetic(e1,e2,"%") {};
  virtual void check();
  virtual Expression* cfold();
};

class Minus : public Arithmetic {
public:
  Minus(Expression* e) : Arithmetic(e,e,"-") {};
  virtual void check();
  virtual Expression* cfold();
  void print(int nesting);
};

// Operadores lógicos AND, OR, NOT
class Logical : public BinaryOp {
protected:
  Logical(Expression* e1, Expression* e2, std::string op)
    : BinaryOp(e1,e2,op) {};
  virtual void check();
};

class And : public Logical {
public:
  And(Expression* e1, Expression* e2) : Logical(e1,e2,"and") {};
  virtual Expression* cfold();
};

class Or : public Logical {
public:
  Or(Expression* e1, Expression* e2) : Logical(e1,e2,"or") {};
  virtual Expression* cfold();
};

class Not : public Logical {
public:
  Not(Expression* e) : Logical(e,e,"not") {};
  virtual void check();
  virtual Expression* cfold();
  void print(int nesting);
};

// Operadores relacionales
class Relational : public Logical {
protected:
  Relational(Expression* e1, Expression* e2, std::string op)
    : Logical(e1,e2,op) {};
  virtual void check();
};

class Greater : public Relational {
public:
  Greater(Expression* e1, Expression* e2) : Relational(e1,e2,">") {};
  virtual Expression* cfold();
};

class GreaterEq : public Relational {
public:
  GreaterEq(Expression* e1, Expression* e2) : Relational(e1,e2,">=") {};
  virtual Expression* cfold();
};

class Equal : public Relational {
public:
  Equal(Expression* e1, Expression* e2) : Relational(e1,e2,"=") {};
  virtual void check();
  virtual Expression* cfold();
};

class NotEqual : public Relational {
public:
  NotEqual(Expression* e1, Expression* e2) : Relational(e1,e2,"!=") {};
  virtual void check();
  virtual Expression* cfold();
};

class Less : public Relational {
public:
  Less (Expression* e1, Expression* e2) : Relational(e1,e2,"<") {};
  virtual Expression* cfold();
};

class LessEq : public Relational {
public:
  LessEq(Expression* e1, Expression* e2) : Relational(e1,e2,"<=") {};
  virtual Expression* cfold();
};

// Acceso a un arreglo
class Index : public Expression {
private:
  Expression* array;
  Expression* index;
public:
  Index(Expression* array, Expression* index)
    : array(array), index(index) {};
  virtual void check();
  void print(int nesting);
};

// Acceso a un campo de un box
class Dot : public Expression {
private:
  Expression* box;
  std::string field;
public:
  Dot(Expression* box, std::string field) : box(box), field(field) {};
  virtual void check();
  void print(int nesting);
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
