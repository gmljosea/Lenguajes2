#include <string>
#include <iostream>
#include "symbol.hh"
#include "statement.hh"

/*******************************/
/* Metodos de la clase Symbol */
/*****************************/

Symbol::Symbol(std::string id,int line,int col){
  this->id=id;
  this->line= line;
  this->col=col;
}

std::string Symbol::getId(){
  return this->id;
}

void Symbol::setDuplicated(bool dup){
  this->duplicated=dup;
}

bool Symbol::isDuplicated(){
  return this->duplicated;
}

void Symbol::setType(Type* t) {
  this->type = t;
}

Type* Symbol::getType(){
  return this->type;
}

int Symbol:: getnumScope(){
  return this->numScope;
}

int Symbol::getLine() {
  return this->line;
}

int Symbol::getColumn() {
  return this->col;
}

/***********************************/
/* Metodos de la clase SymFunction*/
/**********************************/

SymFunction::SymFunction(std::string id,int line, int col,
                         listSymPairs *arguments) : Symbol(id,line,col){
  this->arguments = arguments;
}

void SymFunction::setBlock(Block* block) {
  this->block = block;
}

Block* SymFunction::getBlock(){
  return this->block;
}

int SymFunction::getArgumentCount(){
  return this->arguments->size();
}

void SymFunction::print() {
  std::cout << "Función " << id << " (" << line << ":" << col << ")" << std::endl;
  std::cout << "  Tipo: ";
  type->print();
  std::cout << std::endl;
  if (arguments->empty()) {
    std::cout << "  Sin argumentos" << std::endl;
  } else {
    std::cout << "  Argumentos:" << std::endl;
    for (listSymPairs::iterator it = arguments->begin();
	 it != arguments->end(); it++) {
      
      std::cout << "    Argumento: ";
      it->second->print();
    }
  }
  // Imprimir bloque con nivel de anidamiento 1
  block->print(1);
}

/**********************************/
/*** Metodos de la clase SymVar ***/
/**********************************/

SymVar::SymVar(std::string id,int line,int col,
               bool isParam, int scope) : Symbol(id,line,col){
  this->numScope = scope;
  this->isParameter= isParam;
  //  std::cout << "SymVar creado";
}

void SymVar::print(){
  std::cout << id << " (" << line << ":" << col << ")"<< std::endl;
}

void SymVar::setReadonly(bool readonly){
  this->readonly= readonly;
}

bool SymVar::isReadonly(){
  return this->readonly;
}

SymTable::SymTable(){
  this->nextscope=2;
  this->stack.push_back(0);
  this->stack.push_back(1);
}

int SymTable::current_scope(){
  return this->stack[stack.size()-1];
}

void SymTable::insert(SymVar *sym){
  //std::cout << "insertando var "<<sym->getId()<<std::endl;
  this->varTable.insert(varSymtable::value_type(sym->getId(),sym));

}

void SymTable::insert(SymFunction *sym){
  //  std::cout << "insertando fun "<<sym->getId()<<std::endl;
  this->funcTable.insert(funcSymtable::value_type(sym->getId(),sym));
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

void SymTable::print(){

  std::cout << std::endl << "**** Tabla de simbolos ****" << std::endl<< std::endl;
  std::cout << "Variables:" << std::endl;
 
  for(varSymtable::iterator it= this->varTable.begin(); 
      it!= this->varTable.end(); it++){
    (*it).second->print();
  }

}


