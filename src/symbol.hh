#include <string>

// Clase abstracta que representa un objeto de la tabla de simbolos 
class Symbol{
private:
  std::string id;
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
public:
  SymVar (std::string id);
};
