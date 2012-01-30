#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include "expression.hh"

class Block;
class Statement;

// Clase abstracta que representa una instrucción
class Statement {
private:
  Statement *enclosing;
public:
  Statement ();
  virtual void print(int) = 0;
};

// Representa una instrucción vacía, o sea, que no hace nada
class Null : public Statement {
public:
  virtual void print(int);
};

class If : public Statement {
private:
  Expression *cond;
  Block *block_true;
  Block *block_false;
public:
  If (Expression*, Block*, Block* bf = NULL);
  virtual void print(int);
};

class Block {
private:
  std::list<Statement*> stmts;
  int scope_number;
public:
  Block (int, Statement*);
  void push_back(Statement *stmt);
  virtual void print(int);
};

#endif
