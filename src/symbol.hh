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
  Type* type;
  int line;
  int col;
  bool duplicated;
public:
  Symbol (std::string id,int line,int col);
  std::string getId();
  void setDuplicated(bool dup);
  bool isDuplicated();
  void setType(Type* t);
  Type *getType();
  int getnumScope();
  int getLine();
  int getColumn();
};

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
private:
  bool isParameter;
  bool readonly;
public:
  SymVar (std::string id, int line,int col,bool isParam, int scope);
  void setReadonly(bool readonly);
  bool isReadonly();
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
  void setBlock(Block* block);
  Block* getBlock();
  int getArgumentCount();
  void print();
};

typedef std::unordered_multimap<std::string,SymVar*> varSymtable;
typedef std::unordered_map<std::string,SymFunction*> funcSymtable;

/* Clase SymTable, representa la Tabla de simbolos con el manejo 
   adecuado del anidamiento de los alcances.Metodo Leblanc-Cook */
class SymTable{
private:
  // tabla para las variables
  varSymtable varTable;
  // tabla para las funciones 
  funcSymtable funcTable;
  int nextscope;
  // Pila para el manejo de los alcances
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
