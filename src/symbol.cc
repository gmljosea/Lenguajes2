#include <string>
#include <iostream>
#include "symbol.hh"


Symbol::Symbol(std::string id){
  this->id=id;
}

std::string Symbol::getId(){
  return this->id;
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
  this->nextscope=2;
  this->duracell.push(0);
  this->duracell.push(1);
}

int SymTable::current_scope(){
  return this->duracell.top();
}

void SymTable::insert(Symbol sym){
  this->tabla[sym.getId()]= &sym;
}

int SymTable::leave_scope(){
  this->duracell.pop();
}

int SymTable::enter_scope(int scope){
  this->duracell.push(scope);
}

Symbol SymTable::lookup_global(std::string){
  /*Buscar en el contexto global*/
}

Symbol SymTable::lookup(std::string nombreID,int linea,int columna){
  /*Buscar en cualquier contexto*/
}


