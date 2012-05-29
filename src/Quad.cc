#include "Quad.hh"
#include <iostream>

void Quad::printQuad(){}

void  AsignmentQ::printQuad(){
  // Nombre del temporal
  std::cout << this->result->getId() << ":= ";
  if(arg1Type == id){
    std::cout << this->arg1.id;
  }else if(arg1Type ==constint){
    std::cout << this->arg1.constint;
  }else if(arg1Type == constfloat){
    std::cout << this->arg1.constfloat;
  }else if(arg1Type == constchar){
    printf("%c",this->arg1.constchar);
  }else if(arg1Type == constbool){
    std::cout << this->arg1.constbool;
  }else if(arg1Type == conststring){
    std::cout << *(this->arg1.conststring);
  }
  std::cout << std::endl;
}
