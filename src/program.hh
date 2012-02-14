#ifndef DEVANIX_PROGRAM
#define DEVANIX_PROGRAM

#include <list>
#include <string>

#include "statement.hh"
#include "symbol.hh"

struct Program {
  SymTable symtable;
  std::list<VariableDec*> globalinits;
  std::list<SymFunction*> functions;

  int errorCount; // Número de errores encontrados
  void error(std::string error, int line, int col);
};

#endif
