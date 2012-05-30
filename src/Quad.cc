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


