#include "Quad.hh"
#include <iostream>

// Funciones declaradas al final para simplificar la vida
void printArg(ArgType argType,Args arg);
std::string opToString(Operator op);

   /*****************
    * CONSTRUCTORES *
    *****************/

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

ConditionalNJumpQ::ConditionalNJumpQ(SymVar* arg1,Operator op,SymVar* arg2,
				     Label* label){
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

IndexAsigQ::IndexAsigQ(SymVar *array,SymVar *index,SymVar *arg){
  this->array=array;
  // Index
  this->indexType= id;
  Args sym;
  sym.id= index;
  this->index=sym;
  // Argumento
  this->argType= id;
  Args symv;
  symv.id= arg;
  this->arg=symv;

}

ParamValQ::ParamValQ(SymVar *param){
  this->paramType= id;
  Args sym;
  sym.id= param;
  this->param=sym;
}

AsignmentToPointQ::AsignmentToPointQ(SymVar* arg1,SymVar* result){
  this->result= result;
  
  this->arg1Type= id;
  Args sym;
  sym.id= arg1;
  this->arg1=sym;
}


       /******************************
        * METODOS PRINT DE LOS QUADS *
        ******************************/

// Para que no fastidie el compilador, mientras.
void Quad::printQuad(){}

/** 
 * AsignmentQ
 * Imprime la instruccion result:= arg
 */
void  AsignmentQ::printQuad(){
  // Nombre de la variable destino
  std::cout << this->result->getId() << ":= ";
  //Nombre del argumento
  printArg(this->arg1Type,this->arg1);
  std::cout << std::endl;
}

/** 
 * AsignmentOpQ
 * Imprime la instruccion result:= arg1 OP arg2
 *                        result:= OP arg1
 */
void AsignmentOpQ::printQuad(){
  std::cout << this->result->getId() << ":= ";

  // Si el segundo argumento es tipo null es operacion unaria
  if(this->arg2Type==null){
    std::cout << opToString(this->op);
    printArg(this->arg1Type,this->arg1);
  }else{
    printArg(this->arg1Type,this->arg1);
    std::cout << " "<< opToString(this->op)<< " ";
    printArg(this->arg2Type,this->arg2);
  }
  std::cout << std::endl;
}

/** 
 * AsignmentPointQ
 * Imprime la instruccion result:= *arg1
 */
void AsignmentPointQ::printQuad(){
  std::cout << this->result->getId() << ":= *";
  std::cout << this->arg1->getId();
  std::cout << std::endl; 
} 

/** 
 * AsignmentToPointQ
 * Imprime la instruccion *result:= arg1
 */
void AsignmentToPointQ::printQuad(){
  std::cout << "*";
  std::cout << this->result->getId() << ":= ";
  printArg(this->arg1Type,this->arg1);
  std::cout << std::endl; 
} 

/** 
 * AsignmentAddQ
 * Imprime la instruccion result:= &arg1
 */
void AsignmentAddQ::printQuad(){
  std::cout << this->result->getId() << ":= &";
  std::cout << this->arg1->getId();
  std::cout << std::endl; 
} 

/** 
 * ConditionalJumpQ
 * Imprime la instruccion if arg1 REL arg2 goto label
 */
void ConditionalJumpQ::printQuad(){

  printf("if ");
  printArg(this->arg1Type,this->arg1);
  std::cout << opToString(this->op);
  printArg(this->arg2Type,this->arg2);
  printf(" goto ");
  printf("l%d",this->label->getId());
  std::cout << std::endl;
 
}

/** 
 * ConditionalNJumpQ
 * Imprime la instruccion ifnot arg1 REL arg2 goto label
 */
void ConditionalNJumpQ::printQuad(){

  printf("ifnot ");
  printArg(this->arg1Type,this->arg1);
  std::cout << opToString(this->op);
  printArg(this->arg2Type,this->arg2);
  printf(" goto ");
  printf("l%d",this->label->getId());
  std::cout << std::endl;
 
}

/** 
 * JumpQ
 * Imprime la instruccion goto label
 */
void JumpQ::printQuad(){
  printf(" goto ");
  printf("l%d",this->label->getId());
  std::cout << std::endl; 
}

/** 
 * ParamValQ
 * Imprime la instruccion paramVal arg
 */
void ParamValQ::printQuad(){
  printf("paramVal ");
  printArg(this->paramType,this->param);
  std::cout << std::endl; 
}

/** 
 * ParamRefQ
 * Imprime la instruccion paramRef arg
 */
void ParamRefQ::printQuad(){
  printf("paramRef ");
  std::cout << this->param->getId();
  std::cout << std::endl; 
}

/** 
 * PrologueQ
 * Imprime la instruccion prologue fuc 
 */
void PrologueQ::printQuad(){
  printf("prologue ");
  std::cout << this->func->getId();
  std::cout << std::endl; 
}

/** 
 * CallQ
 * Imprime la instruccion returnVal:= call func numParam
 */
void CallQ::printQuad(){
  printf("call ");
  std::cout << this->func->getId() << " " << this->numParam;
  std::cout << std::endl; 
}

/** 
 * ReturnQ
 * Imprime la instruccion return result
 */
void ReturnQ::printQuad(){
  printf("return ");
  std::cout << this->result->getId();
  std::cout << std::endl;
}

/** 
 * IndexQ
 * Imprime la instruccion result:= a[index]
 */
void IndexQ::printQuad(){
  std::cout <<this->result->getId();
  printf(":= ");
  std::cout <<this->array->getId();
  printf("[");
  printArg(this->indexType,this->index);
  printf("]");
  std::cout << std::endl;
}

/** 
 * IndexAsigQ
 * Imprime la instruccion a[index]:= arg
 */
void IndexAsigQ::printQuad(){
  std::cout <<this->array->getId();
  printf("[");
  printArg(this->indexType,this->index);
  printf("]:= ");
  printArg(this->argType,this->arg);
  std::cout << std::endl;
}


/* Dado un union 'Args' y su tipo lo imprime por pantalla
 * (El 'tipo' viene del enum ArgType, para saber a que campo
 * del union se debe acceder) 
 */ 
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

/* Dado el nombre de un operador (del enum Operator) devuelve el string 
 * asociado a el. Este metodo es para usar en los printQuad y no repetir
 * el codigo*/
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
  }else if(op==greater){
    oper= ">";
  }else if(op==greaterEq){
    oper= ">=";
  }else if(op==equal){
    oper= "==";
  }else if(op==notEqual){
    oper= "!=";
  }else if(op==less){
    oper="<";
  }else if(op==lessEq){
    oper="<=";
  }
  return oper;
}


