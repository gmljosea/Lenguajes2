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
protected:
  std::string id;
  int numScope;
  Type type;
  int line;
  int col;
public:
  Symbol (std::string id,int line,int col);
  std::string getId();
  void setType(Type t);
  int getnumScope();
};

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
private:
  bool isParameter;
public:
  SymVar (std::string id, int line,int col,bool isParam);
  void print();
};

/*Tipo de pasaje para los argumentos de funciones*/
enum PassType {
 normal, // Uso normal porque 'default' está reservado por C++ -.-
 readonly,
 reference
};

typedef std::list<std::pair<PassType, SymVar*>> listSymPairs;

// Clase SymFunction hereda de Symbol
class SymFunction: public Symbol{
private:
  Block *block;
  listSymPairs *arguments;
public:
  SymFunction (std::string id, int linea, int columna,
               listSymPairs *arguments);
  void print();
  void setBlock(Block* block);
};

typedef std::unordered_multimap<std::string,SymVar*> varSymtable;
typedef std::unordered_map<std::string,SymFunction*> funcSymtable;

class SymTable{
private:
  varSymtable varTable;
  funcSymtable funcTable;
  int nextscope;
  std::deque<int> stack;
public:
  SymTable();
  void insert(SymVar *sym);
  void insert(SymFunction *sym);
  SymVar* lookup_variable(std::string nombreID);
  SymFunction* lookup_function(std::string nombreID);
  int current_scope();
  int leave_scope();
  int enter_scope();
  
};

#endif
