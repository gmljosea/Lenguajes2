#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include <string>
#include <utility>
#include "expression.hh"

class Lvalue;

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

class BoundedFor : public Statement {
private:
  std::string* varsym;
  Expression* lowerb;
  Expression* upperb;
  Expression* step;
  Block* block;
public:
  BoundedFor (std::string* varsym, Expression* lowerb, Expression* upperb,
	      Expression* step, Block* block);
  virtual void print(int);
};

class While : public Statement {
private:
  Expression* cond;
  Block* block;
public:
  While (Expression* cond, Block* block);
  virtual void print(int);
};

class Asignment : public Statement {
private:
  std::list<Lvalue*> lvalue;
  std::list<Expression*> exp;
public:
  Asignment (Lvalue* lvalue, Expression* exp);
  void push_back(Lvalue* lvalue, Expression* exp);
  virtual void print(int nesting);
};

class Declaration : public Statement {
private:
  // Representa una declaración con múltiples variables
  // asignadas de una vez
  std::list<Asignment*> asigns;
public:
  Declaration ();
  void push_back(Asignment* asg);
  virtual void print(int nesting);
};

class Block {
private:
  std::list<Statement*> stmts;
  int scope_number;
public:
  Block (int, Statement*);
  void push_back(Statement *stmt);
  void push_back(std::list<Statement*> stmts);
  virtual void print(int);
};

#endif
