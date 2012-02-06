#ifndef DEVANIX_PROGRAM
#define DEVANIX_PROGRAM

#include <list>
#include "statement.hh"
#include "symbol.hh"

/**
 * No me gusta para nada esta clase Program que hice. Espero cambiarla por algo
 * mejor. Me parece que tiene más sentido que la lista de SymFunction, y
 * eventualmente boxes, se guarde dentro de la tabla de símbolos y que provea
 * un iterador sobre ellos.
 * Program debería ser más bien un struct que agrupe la tabla de símbolos
 * y una lista de Asignment que tiene las asignaciones a variables globales.
 */

class Program {
private:
  std::list<VariableDec*> globalvars;
  std::list<SymFunction*> functions;
  //std::list<SymBox> boxes;
public:
  Program ();
  void push_back_global(VariableDec* var);
  void push_back_global(SymFunction* func);
};

#endif
