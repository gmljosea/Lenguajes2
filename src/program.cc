#include <iostream>

#include "program.hh"

void Program::error(std::string error, int line, int col) {
  this->errorCount++;
  std::cout << line << ":" << col << ": error: " << error << std::endl;
}

void Program::check(){
    for(std::list<VariableDec*>::iterator varIt=this->globalinits.begin();  
	varIt != this->globalinits.end(); varIt++){
      (*varIt)->check();
    }

    for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
	 funIt != this->functions.end(); funIt++){
      (*funIt)->check();
    }
    /*
      for (std::list<BoxType*>::iterator BoxIt= this->boxes.begin();
	 BoxIt != this->boxes.end(); BoxIt++){
      ((*BoxIt)->check();
    }

      
     */
}
