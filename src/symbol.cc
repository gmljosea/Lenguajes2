#include <string>
#include <iostream>
#include "symbol.hh"

typedef std::unordered_multimap<std::string,Symbol*> symtable;

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
  this->stack.push_back(0);
  this->stack.push_back(1);
}

int SymTable::current_scope(){
  return this->stack[stack.size()-1];
}

void SymTable::insert(Symbol sym){
  /*Verificar si ya existe en el alcance*/
  this->table.insert(symtable::value_type(sym.getId(),&sym));
}

int SymTable::leave_scope(){
  this->stack.pop_back();
}

int SymTable::enter_scope(int scope){
  this->stack.push_back(scope);
}

Symbol SymTable::lookup_global(std::string nombreID){
  Symbol *best=NULL;
  Symbol *pervasive=NULL;

  std::pair<symtable::iterator, symtable::iterator> pair;
  pair= this->table.equal_range(nombreID);
  
  /*Recorrer todos los nombres encontrados. Tomado del complemento
    del capitulo 3 del Scott*/ 
  for (; pair.first != pair.second; ++pair.first){
    /*  if (pair.first->numScope=0)
      pervasive= pair.first;
    else{ 
      int i=this->stack.size()-1;
      for(i;i>0;i--){
        if (this->stack[i]==pair.first->numScope){
          best=pair.first;
          break;
        }else if(best!=-1 && this->stack[i]==best->numScope)
          break;       
      }    
      }*/
  }

  if (best!=NULL)
    return best;
  else if (pervasive!=NULL)
    return pervasive;
  else 
    return NULL;

}


Symbol SymTable::lookup(std::string nombreID,int linea,int columna){
  /*Buscar en cualquier contexto*/
}


