#include "Quad.hh"
#include <iostream>

void Quad::printQuad(){}

AsignmentOpQ::AsignmentOpQ(SymVar* arg1,Operator op,SymVar* arg2,SymVar* result){
  this->op= op;

  //Argumento 1 
  this->arg1Type= id;
  Args sym;
  sym.id= arg1;
  this->arg1=sym;

  //Argumento 2
  this->arg2Type= (arg2==NULL)? null:id;
  Args sym2;
  sym2.id= arg2;
  this->arg2=sym2;

  // Result 
  this->result= result;
}

ConditionalJumpQ::ConditionalJumpQ(SymVar* arg1,Operator op,SymVar* arg2,Label* label){
  this->op= op;

  //Argumento 1 
  this->arg1Type= id;
  Args sym;
  sym.id= arg1;
  this->arg1=sym;

  //Argumento 2
  this->arg2Type= (arg2==NULL)? null:id;
  Args sym2;
  sym2.id= arg2;
  this->arg2=sym2;

  // Label
  this->label= label;
  
}

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

std::string opToString(Operator op){
  std::string oper;
  if(op == sumI){
    oper= "sumI";
  }else if(op== sumF){
    oper= "sumF";
  }else if(op== substractionI){
    oper="subsI";
  }else if(op== substractionF){
    oper= "subsF";
  }else if(op== multiplicationI){
    oper= "multI";
  }else if(op== multiplicationF){
    oper= "multF";
  }else if(op== divisionI){
    oper= "divI";
  }else if(op== divisionF){
    oper= "divF";
  }else if(op== remainder){
    oper= "%";
  }else if(op== minusI){
    oper= "minI";
  }else if(op== minusF){
    oper="minF";
  }else if(op== AND){
    oper= "and";
  }else if(op== OR){
    oper= "or";
  }else if(op== NOT){
    oper= "not";
  }
  return oper;
}

void printArg(ArgType argType,Args arg){

 if(argType == id){
   std::cout << arg.id->getId();
  }else if(argType ==constint){
    std::cout << arg.constint;
  }else if(argType == constfloat){
    std::cout << arg.constfloat;
  }else if(argType == constchar){
    printf("%c",arg.constchar);
  }else if(argType == constbool){
    std::cout << arg.constbool;
  }else if(argType == conststring){
    std::cout << *(arg.conststring);
  }
}

void AsignmentOpQ::printQuad(){

  if(this->arg2Type==null){
    std::cout << this->result->getId() << ":= "
	      << opToString(this->op);
    printArg(this->arg1Type,this->arg1);
    std::cout << std::endl;
  }else{
    std::cout << this->result->getId() << ":= ";
    printArg(this->arg1Type,this->arg1);
    std::cout << " "<< opToString(this->op)<< " ";
    printArg(this->arg2Type,this->arg2);
    std::cout << std::endl;
  }

}



