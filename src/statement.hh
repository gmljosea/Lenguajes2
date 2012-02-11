#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include <string>
#include <utility>
#include "expression.hh"
#include "symbol.hh"

class Lvalue {};

class NormalLvalue : public Lvalue {
private:
  SymVar* variable;
public:
  NormalLvalue (SymVar* sym);
};

class BadLvalue : public Lvalue {
};

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
  SymVar* varsym;
  Expression* lowerb;
  Expression* upperb;
  Expression* step;
  Block* block;
public:
  BoundedFor (std::string* label, SymVar* varsym, Expression* lowerb,
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
  Asignment (std::list<Lvalue*> lvalues, std::list<Expression*> exps);
  virtual void print(int nesting);
  // virtual void check();
};

class VariableDec : public Statement {
private:
  // Representa una declaración con múltiples variables
  // asignadas de una vez
  Type type;
  std::list<std::pair<SymVar*,Expression*>> decls;
  bool isGlobal;
public:
  VariableDec (Type type, std::list<std::pair<SymVar*,Expression*>> decls);
  void setGlobal(bool g);
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

class Write : public Statement {
private:
  std::list<Expression*> exps;
  bool isLn; // Representa si es Write o Writeln
public:
  Write (std::list<Expression*> exps, bool isLn);
  virtual void print(int nesting);
};

class Read : public Statement {
private:
  Lvalue* lval;
public:
  Read (Lvalue* lval);
  virtual void print(int nesting);
};

#endif
