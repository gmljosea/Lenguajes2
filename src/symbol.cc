#include <string>
#include <iostream>
#include "symbol.hh"

Symbol::Symbol(std::string id){
  this->id=id;
}

SymFunction::SymFunction(std::string id) : Symbol(id){
  std::cout << "SymFunction creado";
}

SymVar::SymVar(std::string id) : Symbol(id){
  std::cout << "SymVar creado";
}
