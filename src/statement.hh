#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include <string>
#include <utility>
#include "expression.hh"
#include "symbol.hh"

/* Chequeos inusuales:
- Que las funciones, si no son void, tengan return apropiado en todos sus caminos.

 */
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
  // El check es solo hacerle check() a todas las subinstrucciones
  // Si la función es de tipo void(), el bloque debe terminar con un
};

// Representa una instrucción vacía, o sea, que no hace nada
class Null : public Statement {
public:
  virtual void print(int);
  // virtual void check();
  // El check no hace nada aqui
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
  // Ver que la expr. sea de tipo bool, y chequear los bloques
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
  // chequear que las exp. sean int y chequear el bloque
  // La variable de iteración se chequea implícitamente porque debe instanciarse
  // con el flag 'readonly' prendido.
};

class While : public Iteration {
private:
  Expression* cond;
  Block* block;
public:
  While (std::string* label, Expression* cond, Block* block);
  virtual void print(int);
  // virtual void check();
  // chequear que cond sea de tipo bool y chequear el bloque
};

class Asignment : public Statement {
private:
  std::list<Lvalue*> lvalues;
  std::list<Expression*> exps;
public:
  Asignment (std::list<Lvalue*> lvalues, std::list<Expression*> exps);
  virtual void print(int nesting);
  // virtual void check();
  // Chequear que las listas sean del mismo tamaño, y cada par lvalue-expr.
  // sean del mismo tipo
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
  // Chequear que la exp. sean del tipo Type
};

class Break : public Statement {
private:
  Iteration* loop;
  std::string* label;
public:
  Break (std::string* label = NULL);
  virtual void print(int nesting);
  // Subir el apuntador *enclosing y castear hasta conseguir un Iteration
  // apropiado (el primero que haga match del label) y actualizar la variable
  // loop para que apunte allí. Explota si llega a la raíz del árbol y no hay
  // Iteration que lo logre.
};

class Next : public Statement {
private:
  Iteration *loop;
  std::string* label;
public:
  Next (std::string* label = NULL);
  virtual void print(int nesting);
  // Igualito que el Break
};

class Return : public Statement {
private:
  Expression* exp;
  SymFunction* symf; // Función que termina este return
public:
  Return (SymFunction* symf, Expression* exp = NULL);
  virtual void print(int nesting);
  // El chequeo es ver que la expresión concuerda con la función.
};

class FunctionCall : public Statement {
private:
  Expression *exp; // Cambiar por el tipo de expresión FunCallExp
public:
  FunctionCall (Expression* exp);
  virtual void print(int nesting);
  // Chequear que la llamada cuadre con la firma
};

class Write : public Statement {
private:
  std::list<Expression*> exps;
  bool isLn; // Representa si es Write o Writeln
public:
  Write (std::list<Expression*> exps, bool isLn);
  virtual void print(int nesting);
  // Creo que no hay nada que chequear
};

class Read : public Statement {
private:
  Lvalue* lval;
public:
  Read (Lvalue* lval);
  virtual void print(int nesting);
  // Creo que aqui tampoco hay nada que chequear
};

#endif
