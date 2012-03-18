#ifndef AST_STATEMENT
#define AST_STATEMENT

#include <list>
#include <string>
#include <utility>
#include "expression.hh"
#include "symbol.hh"

/**
 * Este archivo define las clases que representan una instrucción del lenguaje
 * Devanix y que son usadas en la construcción del árbol de sintaxis.
 * La clase Statement es la base de la cual derivan las instrucciones
 * específicas del lenguaje.
 *
 */

/**
 * Representa una instrucción en Devanix.
 */
class Statement {
protected:
  // Instrucción que anida a esta
  Statement *enclosing;
  // Ubicación en el archivo
  int first_line, first_column;
  int last_line, last_column;
public:
  Statement ();
  // Establece la instrucción que anida a esta.
  void setEnclosing(Statement *stmt);
  // Devuelve la instrucción que anida a esta.
  Statement* getEnclosing();
  // Establece la ubicación en el archivo de la instrucción.
  void setLocation(int first_line, int first_column, int last_line,
		   int last_column);
  // Imprime recursivamente esta instrucción y sus hijos.
  virtual void print(int) = 0;
  virtual void check()=0;

  int getFirstLine();
  int getFirstCol();
};

/**
 * Representa una secuencia de instrucciones.
 * Aunque en la especificación del lenguaje no se menciona el bloque como una
 * instrucción, implementarla como una permite que eventualmente podamos
 * permitir cosas como bloques arbitrarios o condicionales e iteraciones
 * de una sola instrucción sin usar llaves, sin dar muchas vueltas.
 */
class Block : public Statement {
private:
  std::list<Statement*> stmts;
  int scope_number; // Número de alcance asignado
public:
  Block (int, Statement*);
  // Encola una instrucción
  void push_back(Statement *stmt);
  // Encola una lista de instrucciones
  void push_back(std::list<Statement*> stmts);
  virtual void print(int);
  virtual void check();
};

// Representa una instrucción vacía
class Null : public Statement {
public:
  virtual void check();
  virtual void print(int);
};

/**
 * Representa una selección condicional (un If), con o sin bloque else.
 */
class If : public Statement {
private:
  Expression *cond;
  Block *block_true;
  Block *block_false;
public:
  If (Expression*, Block*, Block* bf = NULL);
  virtual void print(int);
  virtual void check();
};

/**
 * Clase abstracta que representa una iteración.
 * Por ahora la única razón para hacerlo es que todas las iteraciones
 * pueden llevar una etiqueta que puede usarse con break o next.
 */
class Iteration : public Statement {
protected:
  std::string* label;
  Block* block;
public:
  Iteration (std::string* label, Block* block);
  void setBlock(Block* block);
  std::string* getLabel();
};

/**
 * Representa una iteración acotada por un rango de enteros, el
 * for i in x..y step z del lenguaje, con o sin 'step'.
 */
class BoundedFor : public Iteration {
private:
  SymVar* varsym;
  Expression* lowerb;
  Expression* upperb;
  Expression* step;
public:
  BoundedFor (std::string* label, SymVar* varsym, Expression* lowerb,
	      Expression* upperb, Expression* step, Block* block);
  virtual void print(int);
  virtual void check();
};

/**
 * Representa una iteración condicional, el While.
 */
class While : public Iteration {
private:
  Expression* cond;
public:
  While (std::string* label, Expression* cond, Block* block);
  virtual void print(int);
  virtual void check();
};

class ForEach : public Iteration {
private:
  SymVar* loopvar;
  Expression* array;
public:
  ForEach (std::string* label, SymVar* loopvar, Expression* array, Block* block)
    : loopvar(loopvar), array(array), Iteration(label,block) {};
  virtual void print(int nesting);
  virtual void check();
};

/**
 * Representa una asignación, múltiple o simple.
 * Para manejar la asignación múltiple se usan dos listas, una de
 * l-values y otra de expresiones. Cuando una asignación es sencilla,
 * cada lista debe tener tamaño 1.
 */
class Asignment : public Statement {
private:
  std::list<Expression*> lvalues;
  std::list<Expression*> exps;
public:
  Asignment (std::list<Expression*> lvalues, std::list<Expression*> exps);
  virtual void print(int nesting);
  virtual void check();
};

/**
 * Representa una declaración de variables.
 * Se representa como una instrucción en sí misma porque a veces se puede
 * inicializar una variable justo al declararla, lo cual tiene una semántica
 * distinta a una asignación normal, y por lo tanto los chequeos son distintos.
 * Si no hay inicializaciones, esta instrucción se debe comportar como la
 * instrucción vacía.
 */
class VariableDec : public Statement {
private:
  Type* type;
  // Se representa con una lista de pares símbolo-inicialización.
  // Si no se inicializa, el lado de expresión es NULL
  std::list<std::pair<SymVar*,Expression*>> decls;
  // Determina si es una declaración de variable global.
  bool isGlobal;
public:
  VariableDec (Type* type, std::list<std::pair<SymVar*,Expression*>> decls);
  virtual void check();
  // Marca esta declaración como de caracter global.
  // Esto cambia la semántica de la instrucción y los chequeos que deben realizarse.
  void setGlobal(bool g);
  virtual void print(int nesting);
};

/**
 * Representa la instrucción break, con o sin etiqueta.
 */
class Break : public Statement {
private:
  // Iteración que afecta esta instrucción.
  // Esto inicialmente es NULL y se actualiza después del parsing.
  Iteration* loop;
  // Etiqueta de la iteración. Si no tiene es NULL.
  std::string* label;
public:
  Break (std::string* label, Iteration* loop);
  virtual void check();
  virtual void print(int nesting);
};

/**
 * Representa la instrucción next, con o sin etiqueta.
 */
class Next : public Statement {
private:
  // Iteración que afecta esta instrucción.
  // Esto inicialmente es NULL y se actualiza después del parsing.
  Iteration* loop;
  // Etiqueta de la iteración. Si no tiene es NULL.
  std::string* label;
public:
  Next (std::string* label, Iteration* loop);
  virtual void check();
  virtual void print(int nesting);
};

/**
 * Representa la instrucción return, con o sin expresión.
 */
class Return : public Statement {
private:
  Expression* exp;
  SymFunction* symf; // Función que termina este return
public:
  Return (SymFunction* symf, Expression* exp = NULL);
  virtual void check();
  virtual void print(int nesting);
};

/**
 * Representa una llamada a función.
 */
class FunctionCall : public Statement {
private:
  Expression *exp; // Cambiar por el tipo de expresión FunCallExp
public:
  FunctionCall (Expression* exp);
  virtual void check();
  virtual void print(int nesting);
};

/**
 * Representa la instrucción write y writeln.
 */
class Write : public Statement {
private:
  std::list<Expression*> exps;
  bool isLn; // Representa si es Write o Writeln
public:
  Write (std::list<Expression*> exps, bool isLn);
  virtual void check();
  virtual void print(int nesting);
};

/**
 * Representa la instrucción read.
 */
class Read : public Statement {
private:
  Expression* lval;
public:
  Read (Expression* lval);
  virtual void check();
  virtual void print(int nesting);
};

#endif
