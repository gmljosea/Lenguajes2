#ifndef AST_EXPRESSION
#define AST_EXPRESSION

#include <list>

class Expression {
public:
  void print(int nesting);
};

class VarExp : public Expression {
};

class IntExp : public Expression {
};

class FloatExp : public Expression {
};

class BoolExp : public Expression {
};

class StringExp : public Expression {
};

class FunCallExp : public Expression {
private:
  std::list<Expression*> args;
public:
  FunCallExp (std::list<Expression*> args);
};

#endif
