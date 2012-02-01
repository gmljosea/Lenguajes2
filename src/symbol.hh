#include <string>

// Clase abstracta que representa un objeto de la tabla de simbolos 
class Symbol{
private:
  std::string id;
public:
  Symbol (string id);

}

// Clase SymFunction hereda de Symbol
class SymFunction: public Symbol{
public:
  SymFunction ();
}

// Clase SymVar hereda de Symbol 
class SymVar: public Symbol{
public:
  SymVar ();
}
