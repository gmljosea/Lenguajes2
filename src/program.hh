#ifndef DEVANIX_PROGRAM
#define DEVANIX_PROGRAM

#include <list>
#include "statement.hh"
#include "symbol.hh"

struct Program {
  SymTable symtable;
  std::list<VariableDec*> globalinits;
  std::list<SymFunction*> functions;

  bool isValid; // Si este programa no tiene errores
  int errorCount; // Número de errores encontrados
};

#endif
