#ifndef DEVANIX_SYMBOLS
#define DEVANIX_SYMBOLS

#include <list>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "type.hh"

class Block;

// Clase abstracta que representa un objeto de la tabla de simbolos 
class Symbol {
private:
  std::string id;
  int numScope;
  Type type;
public:
  Symbol (std::string id);
  std::string getId();
  int getnumScope();
};

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
private:
  int line; // Realmente creo que esto debería ir en Symbol
  int col;  // Quizás sea útil saber dónde se declaró una función, por ejemplo
public:
  SymVar (std::string id, int linea, int columna);
};

enum PassType {
  normal, // Uso normal porque 'default' está reservado por C++ -.-
 value,
 reference
};

// Clase SymFunction hereda de Symbol
class SymFunction: public Symbol{
private:
  Block *block;
  std::list<std::pair<PassType, SymVar*>> arguments;
public:
  SymFunction (std::string id);
};

class SymTable{
private:
  std::unordered_multimap<std::string,Symbol*> table;
  int nextscope;
  std::deque<int> stack;
public:
  SymTable();
  void insert(Symbol sym);
  Symbol* lookup(std::string nombreID,int linea,int columna);
  Symbol* lookup_global(std::string);
  int current_scope();
  int leave_scope();
  int enter_scope(int scope);
  
};

#endif
