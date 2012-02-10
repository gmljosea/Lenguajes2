#include <string>
#include <iostream>
#include "symbol.hh"

typedef std::unordered_multimap<std::string,Symbol*> symtable;

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

void Symbol::setType(Type t) {
  this->type = t;
}

SymFunction::SymFunction(std::string id,int line, int col,
                         listSymPairs *arguments) : Symbol(id,line,col){
  std::cout << "SymFunction creado";
}

void SymFunction::setBlock(Block* block) {
  this->block = block;
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

void SymTable::insert(Symbol sym){
  /*Verificar si ya existe en el alcance*/
  this->table.insert(symtable::value_type(sym.getId(),&sym));
}

int SymTable::leave_scope(){
  this->stack.pop_back();
}

int SymTable::enter_scope(){
  this->stack.push_back((this->nextscope)++);
}

// Esto realmente es lo que antes se llamaba lookup_global
Symbol* SymTable::lookup_function(std::string nombreID){
  Symbol *sym=NULL;
  std::pair<symtable::iterator, symtable::iterator> pair1;
  pair1= this->table.equal_range(nombreID);
  
  for (; pair1.first != pair1.second; ++pair1.first){
    if (pair1.first->second->getnumScope()==0)
      sym= pair1.first->second;
  }

  if (sym!=NULL)
    return sym;
  else 
    return NULL;
}
 
Symbol* SymTable::lookup_variable(std::string nombreID){
  /*Buscar en cualquier contexto*/
 Symbol *best=NULL;
  Symbol *pervasive=NULL;

  std::pair<symtable::iterator, symtable::iterator> pair1;
  pair1= this->table.equal_range(nombreID);
  
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


