#ifndef AST_EXPRESSION
#define AST_EXPRESSION

#include <list>
#include <string>

#include "symbol.hh"

class Expression {
public:
  void print(int nesting);
};

class BadExp : public Expression {
};

class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
};

class IntExp : public Expression {
private:
  int value;
public:
  IntExp(int value);
};

class FloatExp : public Expression {
private:
  float value;
public:
  FloatExp(float value);
};

class BoolExp : public Expression {
private:
  bool value;
public:
  BoolExp(bool value);
};

class StringExp : public Expression {
private:
  std::string str;
public:
  StringExp(std::string str);
};

class FunCallExp : public Expression {
private:
  std::list<Expression*> args;
public:
  FunCallExp (std::list<Expression*> args);
};

#endif
