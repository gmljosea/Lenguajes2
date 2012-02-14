#ifndef AST_EXPRESSION
#define AST_EXPRESSION

#include <list>
#include <string>

#include "symbol.hh"
#include "type.hh"

class Expression {
public:
  void print(int nesting);
  virtual bool isBad();
  virtual Type* getType() = 0;
};

class BadExp : public Expression {
private:
  VoidType type;
public:
  virtual bool isBad();
  virtual Type* getType();
};

class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
  virtual Type* getType();
};

class IntExp : public Expression {
private:
  int value;
  IntType type;
public:
  IntExp(int value);
  virtual Type* getType();
};

class FloatExp : public Expression {
private:
  float value;
  FloatType type;
public:
  FloatExp(float value);
  virtual Type* getType();
};

class BoolExp : public Expression {
private:
  bool value;
  BoolType type;
public:
  BoolExp(bool value);
  virtual Type* getType();
};

class StringExp : public Expression {
private:
  std::string str;
  StringType type;
public:
  StringExp(std::string str);
  virtual Type* getType();
};

class FunCallExp : public Expression {
private:
  SymFunction* symf;
  std::list<Expression*> args;
  bool checkedFunction;
public:
  FunCallExp (SymFunction* symf, std::list<Expression*> args);
  FunCallExp (std::list<Expression*> args);
  virtual Type* getType();
};

#endif
