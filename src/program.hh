#ifndef DEVANIX_PROGRAM
#define DEVANIX_PROGRAM

#include <list>
#include <string>

#include "statement.hh"
#include "symbol.hh"

/**
 * Representa un programa en Devanix. Es el resultado del proceso de parsing.
 *
 * Después del parsing ya no importa el orden entre la declaración de variables
 * globales y las funciones, pues ya se debieron asociar todos los símbolos de
 * variables al símbolo que les corresponde en la tabla. Entonces mantenemos
 * las declaraciones de variables en una lista y las funciones en otra.
 */
struct Program {
  SymTable symtable;
  std::list<VariableDec*> globalinits;
  std::list<SymFunction*> functions;

  int errorCount; // Número de errores encontrados
  /**
   * Muestra por pantalla el error indicado e incrementa el contador de errores
   * de este Program.
   */
  void error(std::string error, int line, int col);
};

#endif
