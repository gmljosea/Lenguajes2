#include "symbol.hh"
#include <string>

Symbol::Symbol(string id){
  this->id=id;
}

SymFunction::SymFunction(string id): Symbol(id){
  std::cout << "SymFunction creado";
}

SymFunction::SymFunction(string id): Symbol(id){
  std::cout << "SymFunction creado";
}
