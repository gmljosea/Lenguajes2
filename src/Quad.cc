#include "Quad.hh"
#include "program.hh"
#include <iostream>

extern Program program;

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

bool ConditionalJumpQ::isJump() {
  return true;
}

std::list<BasicBlock*> ConditionalJumpQ::getTargetBlocks() {
  BasicBlock* b = this->label->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

bool ConditionalJumpQ::isHardJump() {
  return false;
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


bool ConditionalNJumpQ::isJump() {
  return true;
}

std::list<BasicBlock*> ConditionalNJumpQ::getTargetBlocks() {
  BasicBlock* b = this->label->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

bool ConditionalNJumpQ::isHardJump() {
  return false;
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

bool JumpQ::isJump() {
  return true;
}

std::list<BasicBlock*> JumpQ::getTargetBlocks() {
  std::cout << "Target: " << this->label->getId() << ", ";
  Quad* p = dynamic_cast<Quad*>(this->label->getInstruction());
  p->printQuad();
  BasicBlock* b = this->label->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

bool JumpQ::isHardJump() {
  return false;
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

bool CallQ::isJump() {
  return true;
}

bool CallQ::isCall() {
  return true;
}

SymFunction* CallQ::getCallTarget() {
  return this->func;
}

std::list<BasicBlock*> CallQ::getTargetBlocks() {
  BasicBlock* b = this->func->getLabel()->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

bool CallQ::isHardJump() {
  return false;
}

// ReturnQ
ReturnQ::ReturnQ(SymVar* res, SymFunction* symf) {
  this->argt = ArgType::id;
  this->arg.id = res;
  this->symf = symf;
}

ReturnQ::ReturnQ(ArgType argt, Args arg, SymFunction* symf) {
  this->argt = argt;
  this->arg = arg;
  this->symf = symf;

}

ReturnQ::ReturnQ(SymFunction* symf) {
  this->argt = ArgType::null;
  this->arg.id = NULL;
  this->symf = symf;
}

/** 
 * ReturnQ
 * Imprime la instruccion return result
 */
void ReturnQ::printQuad(){
  printf("return ");
  if (this->argt != ArgType::null) {
    // std::cout << this->result->getId();
    printArg(this->argt, this->arg);
    std::cout << std::endl;
  }
}

bool ReturnQ::isJump() {
  return true;
}

std::list<BasicBlock*> ReturnQ::getTargetBlocks() {
  return this->symf->getReturnTargets();
}

bool ReturnQ::isHardJump() {
  return true;
}

bool ReturnQ::isMainReturn() {
  return this->symf == program.main;
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

WriteQ::WriteQ(ArgType argt, Args arg, Type* type, bool isLn) {
  this->argt = argt;
  this->arg = arg;
  this->type = type;
  this->isLn = isLn;
}

void WriteQ::printQuad() {
  if (this->isLn) {
    std::cout << "write ";
  } else {
    std::cout << "writeln ";
  }
  std::cout << this->type->toString() << " ";
  printArg(this->argt, this->arg);
  std::cout << std::endl;
}

ReadQ::ReadQ(SymVar* result, Type* type, bool deref) {
  this->result = result;
  this->type = type;
  this->deref = deref;
}

void ReadQ::printQuad() {
  if (deref) std::cout << "*";
  std::cout << this->result->getId() << " := read "
	    << this->type->toString() << std::endl;
}

ReadIndexQ::ReadIndexQ(SymVar* result, ArgType indext, Args index, Type* type) {
  this->indext = indext;
  this->index = index;
  this->result = result;
  this->type = type;
}

void ReadIndexQ::printQuad() {
  std::cout << this->result->getId() << "[";
  printArg(this->indext, this->index);
  std::cout << "] := read " << this->type->toString() << std::endl;
}
