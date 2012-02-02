#include <string>
#include <stack>
#include <unordered_map>

// Clase abstracta que representa un objeto de la tabla de simbolos 
class Symbol{
private:
  std::string id;
  int numScope;
public:
  Symbol (std::string id);
  std::string getId();

};

// Clase SymFunction hereda de Symbol
class SymFunction: public Symbol{
public:
  SymFunction (std::string id);
};

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
private:
  int line;
  int col;
public:
  SymVar (std::string id,int linea,int columna);
};

class SymTable{
private:
  std::unordered_map<std::string,Symbol*> tabla;
  int nextscope;
  std::stack<int> duracell;
public:
  SymTable();
  void insert(Symbol sym);
  Symbol lookup(std::string nombreID,int linea,int columna);
  Symbol lookup_global(std::string);
  int current_scope();
  int leave_scope();
  int enter_scope(int scope);
  
};
