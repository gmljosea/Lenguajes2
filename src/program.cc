#include <iostream>

#include "program.hh"

void Program::error(std::string error, int line, int col) {
  this->errorCount++;
  std::cout << line << ":" << col << ": error: " << error << std::endl;
}

void Program::check(){

  /* Chequear que todos los boxes usados han sido correctamente definidos
   * y que no existen ciclos en sus declarariones */
  for (std::list<BoxType*>::iterator BoxIt= this->boxes.begin();
       BoxIt != this->boxes.end(); BoxIt++){
    (*BoxIt)->check();  
  }

  if (this->errorCount==0){
    // Calcular offsets de los box
    this->calcOffsets();
  }

  this->offsetVarDec=0;
  // Chequear las variables gobales 
  for(std::list<VariableDec*>::iterator varIt=this->globalinits.begin();  
      varIt != this->globalinits.end(); varIt++){
    (*varIt)->check();
  }

  // Chequeo de las funciones 
  for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
       funIt != this->functions.end(); funIt++){
    this->offsetVarDec=0;
    (*funIt)->check();
  }
    
}

/* Calcular offsets (Por ahora de los box)
 * La precondicion es que todos los boxes esten completamente definidos
 * y que no existan ciclos en la definicion de los mismos.
 */ 
void Program::calcOffsets(){
  // Recorrer los box para calcular los offsets
  for (std::list<BoxType*>::iterator BoxIt= this->boxes.begin();
       BoxIt != this->boxes.end(); BoxIt++){
    if(!(*BoxIt)->areOffsetsDone()){ 
      (*BoxIt)->calcOffsets();
    }
  }

}

       
