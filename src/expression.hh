#ifndef AST_EXPRESSION
#define AST_EXPRESSION

#include <list>
#include <string>

#include "symbol.hh"
#include "type.hh"

/**
 * Este archivo define las clases que representan las expresiones en el lenguaje
 * Devanix, y que son usadas en la construcción del árbol de sintaxis.
 * La clase Expression es la base de todas las expresiones.
 */

/**
 * Representa una expresión cualquiera.
 */
class Expression {
public:
  /**
   * Imprime por stdout una representación de esta expresión.
   */
  void print(int nesting);

  /**
   * Determina si esta expresión tiene sentido.
   * Un error del programador detectado durante el parsing, por ejemplo usar una
   * variable no declarada, conduce a que se instancien expresiones marcadas como
   * erróneas.
   * Durante la fase de chequeo estático es necesario revisar que las expresiones
   * tengan sentido antes de realizar otra operaciones sobre ellas.
   */
  virtual bool isBad();

  /**
   * Devuelve el tipo de esta expresión.
   * Se asume que el usuario no va a modificar ni borrar este objeto.
   */
  virtual Type* getType() = 0;
};

/**
 * Representa una expresión errónea.
 * Esto puede ocurrir cuando el programador intenta utilizar una variable no
 * declarada, entonces es necesario instanciar una expresión dummy para continuar
 * con el parsing y otros chequeos.
 */
class BadExp : public Expression {
private:
  VoidType type;
public:
  virtual bool isBad();
  virtual Type* getType();
};

/**
 * Representa una expresión compuesta de una variable cualquiera.
 */
class VarExp : public Expression {
private:
  SymVar* symv;
public:
  VarExp(SymVar* symv);
  virtual Type* getType();
};

/**
 * Representa una expresión compuesta por una constante de tipo número entero.
 */
class IntExp : public Expression {
private:
  int value;
  IntType type;
public:
  IntExp(int value);
  virtual Type* getType();
};

/**
 * Representa una expresión compuesta por una constante de tipo número decimal.
 */
class FloatExp : public Expression {
private:
  float value;
  FloatType type;
public:
  FloatExp(float value);
  virtual Type* getType();
};

/**
 * Representa una expresión compuesta por una constante booleana (true o false)
 */
class BoolExp : public Expression {
private:
  bool value;
  BoolType type;
public:
  BoolExp(bool value);
  virtual Type* getType();
};

/**
 * Representa una expresión compuesta por una cadena de caracteres entre comillas.
 */
class StringExp : public Expression {
private:
  std::string str;
  StringType type;
public:
  StringExp(std::string str);
  virtual Type* getType();
};

class CharExp : public Expression {
private:
  std::string ch; // cambiar a char
  CharType type;
public:
  CharExp(std::string ch);
  virtual Type* getType();
};

/**
 * Representa una llamada a función.
 */
class FunCallExp : public Expression {
private:
  SymFunction* symf;
  std::list<Expression*> args;

  /**
   * Las funciones son visibles en todo el programa, lo que permite usar
   * funciones que aún no han sido parseadas y por lo tanto no están en la tabla
   * de símbolos al momento de parsear una expresión que la llame.
   * Durante el parsing se intenta asociar de una vez esta expresión con su
   * símbolo de función correspondiente, pero si no se puede, se difiere el
   * el chequeo, y para ello se guarda el nombre de la función llamada en 'name'
   */
  std::string name;
  bool checkedFunction;
public:
  FunCallExp (SymFunction* symf, std::list<Expression*> args);
  FunCallExp (std::string name, std::list<Expression*> args);
  virtual Type* getType();
};

#endif
