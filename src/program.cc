#include <iostream>

#include "program.hh"

void Program::error(std::string error, int line, int col) {
  this->errorCount++;
  std::cout << line << ":" << col << ": error: " << error << std::endl;
}

void Program::check(){
    for(std::list<VariableDec*>::iterator varIt=this->globalinits.begin();  
	varIt != this->globalinits.begin(); varIt++){
      (*varIt)->check();
    }

    for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
	 funIt != this->functions.begin(); funIt++){
      ((*funIt)->getBlock())->check();
    }

}
