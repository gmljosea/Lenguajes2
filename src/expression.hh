#ifndef DEVANIX_EXPRESSION
#define DEVANIX_EXPRESSION

#include "symbol.hh"
#include "type.hh"

class Expression {
public:
  virtual void print(int nesting);
  virtual bool isBad();
  virtual Type* getType() = 0;
};

class BadExp : public Expression {
public:
  virtual bool isBad();
  virtual Type* getType();
};

// Expresiones base
class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
  virtual Type* getType();
  virtual void print(int nesting);
};

/**
 * Representa una expresión compuesta por una constante de tipo número entero.
 */
class IntExp : public Expression {
private:
  int value;
public:
  IntExp(int value);
  virtual Type* getType();
  virtual void print(int nesting);
};

/**
 * Representa una expresión compuesta por una constante de tipo número decimal.
 */
class FloatExp : public Expression {
private:
  float value;
public:
  FloatExp(float value);
  virtual Type* getType();
  virtual void print(int nesting);
};

/**
 * Representa una expresión compuesta por una constante booleana (true o false)
 */
class BoolExp : public Expression {
private:
  bool value;
public:
  BoolExp(bool value);
  virtual Type* getType();
  virtual void print(int nesting);
};

/**
 * Representa una expresión compuesta por una cadena de caracteres entre comillas.
 */
class StringExp : public Expression {
private:
  std::string str;
public:
  StringExp(std::string str);
  virtual Type* getType();
};

class CharExp : public Expression {
private:
  std::string ch; // cambiar a char
public:
  CharExp(std::string ch);
  virtual Type* getType();
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


class SumExp : public Expression {
private:
  Expression* op1;
  Expression* op2;
public:
  SumExp(Expression* op1, Expression* op2) : op1(op1), op2(op2) {};
  virtual Type* getType();
  virtual void print(int nesting);
};

#endif
