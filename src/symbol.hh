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
  std::unordered_map<std::string,Symbol*> hash;
  int nextscope;
  std::stack<int> duracell;
public:
  SymTable();
  void insert(Symbol sym);
  void lookup(Symbol sym);
  void lookup_global();
};
