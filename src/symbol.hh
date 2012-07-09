#ifndef DEVANIX_SYMBOLS
#define DEVANIX_SYMBOLS

#include <list>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "flowgraph.hh"
//#include "Quad.hh"
#include "type.hh"

class BasicBlock;
class Block;
class Label;

/**
 * Clase abstracta que representa un objeto de la tabla de simbolos.
 */
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
  Symbol() {};
  std::string getId();
  void setDuplicated(bool dup); // obsoleto
  bool isDuplicated(); // obsoleto
  void setType(Type* t);
  Type *getType();
  int getnumScope();
  int getLine();
  int getColumn();
};

 /**
  * Clase SymVar hereda de Symbol. Representa una variable declarada.
  */
class SymVar: public Symbol{
private:
  bool isParameter;
  bool readonly; // Se pasó como solo lectura
  bool reference; // Se pasó por referencia
  int context;
  int size;
  int offset;

  int idTemp;
  bool temp;

public:
  SymVar (std::string id, int line,int col, bool isParam, int scope);
  SymVar (int idTemp);
  void setReadonly(bool readonly);
  void setReference(bool ref);
  bool isReadonly();
  bool isReference();
  void setContext(int num);
  void setOffset(int offset);
  int getSize();
  int getAlignment();
  int getOffset();
  void setType(Type* type);
  bool isTemp();
  void print();
};

/*Tipo de pasaje para los argumentos de funciones (Se usa solo en el parser)*/
enum PassType {
 normal, // Uso por 'default' (reservado por c++)
 readonly,
 reference
 };

typedef std::list<SymVar*> ArgList;

/**
 * Clase SymFunction hereda de Symbol. Representa una funcion declarada.
 */
class SymFunction: public Symbol {
private:
  Block *block;
  ArgList *args; // Lista de argumentos (SymVar)
  std::list<BasicBlock*> ret_targets;
  Label* start;

  bool generated;

public:
  SymFunction (std::string id, ArgList* arguments, Type* rtype,
               int line, int col);
  void setBlock(Block* block);
  Block* getBlock();
  int getArgumentCount();
  void print();
  ArgList* getArguments();
  void check();
  void gen();

  Label* getLabel();

  void addReturnTarget(BasicBlock* b);
  std::list<BasicBlock*> getReturnTargets();
};

typedef std::unordered_multimap<std::string,SymVar*> varSymtable;
typedef std::unordered_map<std::string,SymFunction*> funcSymtable;
typedef std::unordered_multimap<std::string,BoxType*> boxHash;

/* Clase SymTable, representa la Tabla de simbolos con el manejo
   adecuado del anidamiento de los alcances.Metodo Leblanc-Cook */
class SymTable{
private:
  // Tabla para las variables
  varSymtable varTable;
  // Tabla para las funciones
  funcSymtable funcTable;
  // Tabla para los box
  boxHash boxTable;

  int nextscope;
  // Pila para el manejo de los alcances
  std::deque<int> stack;

public:
  SymTable();
  void insert(SymVar *sym);
  void insert(SymFunction *sym);
  void insert(BoxType *sym);
  SymVar* lookup_variable(std::string nombreID);
  SymFunction* lookup_function(std::string nombreID);
  BoxType* lookup_box(std::string nombreID);
  int current_scope();
  int leave_scope();
  int enter_scope();
  void print();

};

#endif
