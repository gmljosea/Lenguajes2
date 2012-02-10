#include <string>
#include <iostream>
#include "symbol.hh"


Symbol::Symbol(std::string id,int line,int col){
  this->id=id;
  this->line= line;
  this->col=col;
}

std::string Symbol::getId(){
  return this->id;
}

int Symbol:: getnumScope(){
  return this->numScope;
}

SymFunction::SymFunction(std::string id,int line, int col,Block *block,
                         listSymPairs *arguments) : Symbol(id,line,col){
  std::cout << "SymFunction creado";
}

SymVar::SymVar(std::string id,int line,int col,
               bool isParam) : Symbol(id,line,col){
  this->isParameter= isParam;
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

void SymTable::insert(SymVar sym){
  this->varTable.insert(varSymtable::value_type(sym.getId(),&sym));
}

void SymTable::insert(SymFunction sym){
  this->funTable.insert(funcSymtable::value_type(sym.getId(),&sym));
}

int SymTable::leave_scope(){
  this->stack.pop_back();
}

int SymTable::enter_scope(){
  this->stack.push_back((this->nextscope)++);
}

SymFunction* SymTable::lookup_function(std::string nombreID){
  funcSymtable::iterator it= this->funcTable.find(nombreID);
  
  if (it!= funcTable.end())
    return it->second;
  else 
    return NULL;
}
 
SymVar* SymTable::lookup_variable(std::string nombreID){
  /*Buscar en cualquier contexto*/
  SymVar *best=NULL;
  SymVar *pervasive=NULL;

  std::pair<varSymtable::iterator, varSymtable::iterator> pair1;
  pair1= this->varTable.equal_range(nombreID);
  
  /*Recorrer todos los nombres encontrados. Tomado del complemento
    del capitulo 3 del Scott*/ 
  for (; pair1.first != pair1.second; ++pair1.first){
    if (pair1.first->second->getnumScope()==0)
      pervasive= pair1.first->second;
    else{ 
      int i=this->stack.size()-1;
      for(i;i>0;i--){
        if (this->stack[i]==pair1.first->second->getnumScope()){
          best=pair1.first->second;
          break;
        }else if(best!=NULL && this->stack[i]==best->getnumScope())
          break;       
      }    
    }
  }

  if (best!=NULL)
    return best;
  else if (pervasive!=NULL)
    return pervasive;
  else 
    return NULL;

}


