#include <string>
#include <iostream>
#include "symbol.hh"


Symbol::Symbol(std::string id){
  this->id=id;
}

SymFunction::SymFunction(std::string id) : Symbol(id){
  std::cout << "SymFunction creado";
}

SymVar::SymVar(std::string id,int linea,int columna) : Symbol(id){
  this->line= linea;
  this->col=columna;
  std::cout << "SymVar creado";
}

SymTable::SymTable(){
  this->nextscope=1;
}
