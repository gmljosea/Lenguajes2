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
  // Ubicación en el archivo
  int first_line, first_column;
  int last_line, last_column;
public:
  Statement ();
  void setEnclosing(Statement *stmt);
  void setLocation(int first_line, int first_column, int last_line,
		   int last_column);
  virtual void print(int) = 0;
};

// Que Block sea Statement permite que eventualmente podamos definir un bloque
// como una instrucción cualquiera sin mucho esfuerzo
class Block : public Statement {
private:
  std::list<Statement*> stmts;
  int scope_number;
public:
  Block (int, Statement*);
  void push_back(Statement *stmt);
  void push_back(std::list<Statement*> stmts);
  virtual void print(int);
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

#endif
