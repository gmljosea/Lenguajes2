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
  int line;
  int col;
public:
  Symbol (std::string id,int line,int col);
  std::string getId();
  int getnumScope();
};

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
private:
  bool isParameter;
public:
  SymVar (std::string id, int line,int col,bool isParam);
};

/*Tipo de pasaje para los argumentos de funciones*/
enum PassType {
 normal, // Uso normal porque 'default' está reservado por C++ -.-
 value,
 reference
};

typedef std::list<std::pair<PassType, SymVar*>> listSymPairs;

// Clase SymFunction hereda de Symbol
class SymFunction: public Symbol{
private:
  Block *block;
  listSymPairs *arguments;
public:
  SymFunction (std::string id,int linea,int columna,
               Block *block,listSymPairs *arguments );
};

class SymTable{
private:
  std::unordered_multimap<std::string,Symbol*> table;
  int nextscope;
  std::deque<int> stack;
public:
  SymTable();
  void insert(Symbol sym);
  Symbol* lookup(std::string nombreID);
  Symbol* lookup_global(std::string nombreID);
  int current_scope();
  int leave_scope();
  int enter_scope();
  
};

#endif
