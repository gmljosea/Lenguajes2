#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include <string>
#include <utility>
#include "expression.hh"
#include "symbol.hh"

class Lvalue {};

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
  Statement* getEnclosing();
  void setLocation(int first_line, int first_column, int last_line,
		   int last_column);
  virtual void print(int) = 0;
  // virtual void check() = 0;
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
  // virtual void check();
};

// Representa una instrucción vacía, o sea, que no hace nada
class Null : public Statement {
public:
  virtual void print(int);
  // virtual void check();
};

class If : public Statement {
private:
  Expression *cond;
  Block *block_true;
  Block *block_false;
public:
  If (Expression*, Block*, Block* bf = NULL);
  virtual void print(int);
  // virtual void check();
};

class Iteration : public Statement {
protected:
  std::string* label;
public:
  Iteration (std::string* label);
  std::string* getLabel();
};

class BoundedFor : public Iteration {
private:
  std::string* varsym;
  Expression* lowerb;
  Expression* upperb;
  Expression* step;
  Block* block;
public:
  BoundedFor (std::string* label, std::string* varsym, Expression* lowerb,
	      Expression* upperb, Expression* step, Block* block);
  virtual void print(int);
  // virtual void check();
};

class While : public Iteration {
private:
  Expression* cond;
  Block* block;
public:
  While (std::string* label, Expression* cond, Block* block);
  virtual void print(int);
  // virtual void check();
};

class Asignment : public Statement {
private:
  std::list<Lvalue*> lvalues;
  std::list<Expression*> exps;
public:
  Asignment ();
  void push_back_lvalue(Lvalue* lvalue);
  void push_back_exp(Expression* exp);
  virtual void print(int nesting);
  // virtual void check();
};

class VariableDec : public Statement {
private:
  // Representa una declaración con múltiples variables
  // asignadas de una vez
  std::list<std::pair<SymVar*,Expression*>> decs;
  bool isGlobal;
public:
  VariableDec ();
  void push_back(SymVar* sym, Expression* init);
  virtual void print(int nesting);
  // virtual void check();
};

class Break : public Statement {
private:
  Iteration* loop;
  std::string* label;
public:
  Break (std::string* label = NULL);
  virtual void print(int nesting);
};

class Next : public Statement {
private:
  Iteration *loop;
  std::string* label;
public:
  Next (std::string* label = NULL);
  virtual void print(int nesting);
};

class Return : public Statement {
private:
  Expression* exp;
  //* funblock; // Función que termina este return
public:
  Return (Expression* exp = NULL);
  virtual void print(int nesting);
};

class FunctionCall : public Statement {
private:
  Expression *exp; // Cambiar por el tipo de expresión FunCallExp
public:
  FunctionCall (Expression* exp);
  virtual void print(int nesting);
};

#endif
// Falta hacer IO!!!
