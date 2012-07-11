#include "Quad.hh"
#include "program.hh"
#include <iostream>

extern Program program;

// Funciones declaradas al final para simplificar la vida
void printArg(ArgType argType,Args arg);
std::string argToString(ArgType argtype, Args arg);
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
  this->label->setActive(true);
}

Label* ConditionalJumpQ::getTargetLabel() {
  return this->label;
}

void ConditionalJumpQ::replaceTargetLabel(Label* l) {
  this->label = l;
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
  this->label->setActive(true);
}

Label* ConditionalNJumpQ::getTargetLabel() {
  return this->label;
}

void ConditionalNJumpQ::replaceTargetLabel(Label* l) {
  this->label = l;
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

JumpQ::JumpQ(Label* label) {
  this->label = label;
  this->label->setActive(true);
}

Label* JumpQ::getTargetLabel() {
  return this->label;
}

void JumpQ::replaceTargetLabel(Label* l) {
  this->label = l;
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
  std::cout << this->result->getId() << " := ";
  //Nombre del argumento
  printArg(this->arg1Type,this->arg1);
  std::cout << std::endl;
}

std::string AsignmentQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(std::string(" := "));
  result.append(argToString(this->arg1Type, this->arg1));
  return result;
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

std::string AsignmentOpQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := ");
  if (this->arg2Type == ArgType::null) {
    result.append(opToString(this->op));
    result.append(" ");
    result.append(argToString(this->arg1Type, this->arg1));
  } else {
    result.append(argToString(this->arg1Type, this->arg1));
    result.append(" ");
    result.append(opToString(this->op));
    result.append(" ");
    result.append(argToString(this->arg2Type, this->arg2));
  }
  return result;
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

std::string AsignmentPointQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := *");
  result.append(this->arg1->getId());
  return result;
}

/**
 * AsignmentToPointQ
 * Imprime la instruccion *result:= arg1
 */
void AsignmentToPointQ::printQuad(){
  std::cout << "*";
  std::cout << this->result->getId() << " := ";
  printArg(this->arg1Type,this->arg1);
  std::cout << std::endl;
}

std::string AsignmentToPointQ::toString() {
  std::string result = Instruction::toString();
  result.append("*");
  result.append(this->result->getId());
  result.append(" := ");
  result.append(argToString(this->arg1Type, this->arg1));
  return result;
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

std::string AsignmentAddQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := &");
  result.append(this->arg1->getId());
  return result;
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
  std::cout << this->label->toString();
  //  printf("l%d",this->label->getId());
  std::cout << std::endl;

}

std::string ConditionalJumpQ::toString() {
  std::string result = Instruction::toString();
  result.append("if ");
  result.append(argToString(this->arg1Type, this->arg1));
  result.append(" ");
  result.append(opToString(this->op));
  result.append(" ");
  result.append(argToString(this->arg2Type, this->arg2));
  result.append(" goto ");
  result.append(label->toString());
  return result;
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
  std::cout << this->label->toString();
  std::cout << std::endl;

}

std::string ConditionalNJumpQ::toString() {
  std::string result = Instruction::toString();
  result.append("ifnot ");
  result.append(argToString(this->arg1Type, this->arg1));
  result.append(" ");
  result.append(opToString(this->op));
  result.append(" ");
  result.append(argToString(this->arg2Type, this->arg2));
  result.append(" goto ");
  result.append(label->toString());
  return result;
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
  std::cout << this->label->toString();
  //  printf("l%d",this->label->getId());
  std::cout << std::endl;
}

std::string JumpQ::toString() {
  std::string result = Instruction::toString();
  result.append("goto ");
  result.append(this->label->toString());
  return result;
}

bool JumpQ::isJump() {
  return true;
}

std::list<BasicBlock*> JumpQ::getTargetBlocks() {
  BasicBlock* b = this->label->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

bool JumpQ::isHardJump() {
  return true;
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

std::string ParamValQ::toString() {
  std::string result = Instruction::toString();
  result.append("param val ");
  result.append(argToString(this->paramType, this->param));
  return result;
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

std::string ParamRefQ::toString() {
  std::string result = Instruction::toString();
  result.append("param ref ");
  result.append(this->param->getId());
  return result;
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

std::string PrologueQ::toString() {
  std::string result = Instruction::toString();
  result.append("prologue ");
  result.append(this->func->getId());
  return result;
}

/**
 * CallQ
 * Imprime la instruccion call func numParam
 */
void CallQ::printQuad(){
  std::cout << "call ";
  std::cout << this->func->getId() << ", " << this->numParam;
  std::cout << std::endl;
}

std::string CallQ::toString() {
  std::string result = Instruction::toString();
  result.append("call ");
  result.append(this->func->getId());
  result.append(" ");
  result.append(std::to_string((long long int) this->numParam));
  return result;
}

bool CallQ::isJumpTarget() {
  return true;
}

// OBSOLETO
SymFunction* CallQ::getCallTarget() {
  return this->func;
}

// OBSOLETO
std::list<BasicBlock*> CallQ::getTargetBlocks() {
  BasicBlock* b = this->func->getLabel()->getInstruction()->getBlock();
  std::list<BasicBlock*> list;
  list.push_back(b);
  return list;
}

// RetrieveQ
void RetrieveQ::printQuad() {
  std::cout << "retrieve " << var->getId() << std::endl;
}

std::string RetrieveQ::toString() {
  std::string result = Instruction::toString();
  result.append("retrieve ");
  result.append(var->getId());
  return result;
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

bool ReturnQ::isReturn() {
  return true;
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
  }
  std::cout << std::endl;
}

std::string ReturnQ::toString() {
  std::string result = Instruction::toString();
  result.append("return ");
  if (this->argt != ArgType::null) {
    result.append(argToString(this->argt, this->arg));
  }
  return result;
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

std::string IndexQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := ");
  result.append(this->array->getId());
  result.append("[");
  result.append(argToString(this->indexType, this->index));
  result.append("]");
  return result;
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

std::string IndexAsigQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->array->getId());
  result.append("[");
  result.append(argToString(this->indexType, this->index));
  result.append("] := ");
  result.append(argToString(this->argType, this->arg));
  return result;
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

std::string argToString(ArgType argtype, Args arg) {
  switch (argtype) {
  case ArgType::id:
    return std::string(arg.id->getId());
  case ArgType::constint:
    return std::to_string((long long int) arg.constint);
  case ArgType::constfloat:
    return std::to_string((long double) arg.constfloat);
  case ArgType::constchar:
    return std::string("<char>");
  case ArgType::constbool:
    return (arg.constbool) ? std::string("1") : std::string("0");
  case ArgType::conststring:
    return *arg.conststring;
  }
}

/* Dado el nombre de un operador (del enum Operator) devuelve el string
 * asociado a el. Este metodo es para usar en los printQuad y no repetir
 * el codigo*/
std::string opToString(Operator op){
  std::string oper;
  if(op == sumI){
    oper= "+";
  }else if(op== sumF){
    oper= "f_sum";
  }else if(op== substractionI){
    oper="-";
  }else if(op== substractionF){
    oper= "f_sub";
  }else if(op== multiplicationI){
    oper= "*";
  }else if(op== multiplicationF){
    oper= "f_mul";
  }else if(op== divisionI){
    oper= "/";
  }else if(op== divisionF){
    oper= "f_div";
  }else if(op== remainder){
    oper= "%";
  }else if(op== minusI){
    oper= "-";
  }else if(op== minusF){
    oper="f_minus";
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
  }else if(op==greaterF){
    oper= ">f";
  }else if(op==greaterEqF){
    oper= ">=f";
  }else if(op==equalF){
    oper= "==f";
  }else if(op==notEqualF){
    oper= "!=f";
  }else if(op==lessF){
    oper="<f";
  }else if(op==lessEqF){
    oper="<=f";
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

std::string WriteQ::toString() {
  std::string result = Instruction::toString();
  if (this->isLn)
    result.append("write ");
  else
    result.append("writeln ");
  result.append(this->type->toString());
  result.append(" ");
  result.append(argToString(this->argt, this->arg));
  return result;
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

std::string ReadQ::toString() {
  std::string result = Instruction::toString();
  if (deref)
    result.append("*");
  result.append(this->result->getId());
  result.append(" := read ");
  result.append(this->type->toString());
  return result;
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

std::string ReadIndexQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append("[");
  result.append(argToString(this->indext, this->index));
  result.append("] := read ");
  result.append(this->type->toString());
  return result;
}

// CastFtoIQ
void CastFtoIQ::printQuad() {
  std::cout << result->getId() << " := (float to int) ";
  printArg(argt, arg);
  std::cout << std::endl;
}

std::string CastFtoIQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := (float to int) ");
  result.append(argToString(argt, arg));
  return result;
}

// CastItoFQ
void CastItoFQ::printQuad() {
  std::cout << result->getId() << " := (int to float) ";
  printArg(argt, arg);
  std::cout << std::endl;
}

std::string CastItoFQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := (int to float) ");
  result.append(argToString(argt, arg));
  return result;
}

// CastCtoIQ
void CastCtoIQ::printQuad() {
  std::cout << result->getId() << " := (char to int) ";
  printArg(argt, arg);
  std::cout << std::endl;
}

std::string CastCtoIQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := (char to int) ");
  result.append(argToString(argt, arg));
  return result;
}

// CastItoCQ
void CastItoCQ::printQuad() {
  std::cout << result->getId() << " := (int to char) ";
  printArg(argt, arg);
  std::cout << std::endl;
}

std::string CastItoCQ::toString() {
  std::string result = Instruction::toString();
  result.append(this->result->getId());
  result.append(" := (int to char) ");
  result.append(argToString(argt, arg));
  return result;
}


// recalcIN - función de transferencia del cálculo de temporales vivas
std::set<SymVar*> AsignmentOpQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result); // Matar variable asignada

  // Agregar temporales usados
  if (arg1Type == ArgType::id && arg1.id->isTemp()) {
    in.insert(arg1.id);
  }
  if (arg2Type == ArgType::id && arg2.id->isTemp()) {
    in.insert(arg2.id);
  }
  return in;
}

std::set<SymVar*> AsignmentQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result); // Matar variable asignada

  // Agregar temporales usados
  if (arg1Type == ArgType::id && arg1.id->isTemp()) {
    in.insert(arg1.id);
  }
  return in;
}

std::set<SymVar*> AsignmentPointQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (arg1->isTemp()) {
    in.insert(arg1);
  }
  return in;
}

std::set<SymVar*> AsignmentToPointQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  // Aqui nadie muere porque un apuntador nunca va a ser a un temporal
  // Así que *result no puede definir algún temporal
  if (arg1Type == ArgType::id && arg1.id->isTemp()) {
    in.insert(arg1.id);
  }
  if (result->isTemp()) {
    in.insert(result);
  }
  return in;
}

std::set<SymVar*> AsignmentAddQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  // No se usa a nadie porque es imposible que se pida la dirección de un
  // temporal
  return in;
}

std::set<SymVar*> ConditionalJumpQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (arg1Type == ArgType::id && arg1.id->isTemp()) {
    in.insert(arg1.id);
  }
  if (arg2Type == ArgType::id && arg2.id->isTemp()) {
    in.insert(arg2.id);
  }
  return in;
}

std::set<SymVar*> ConditionalNJumpQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (arg1Type == ArgType::id && arg1.id->isTemp()) {
    in.insert(arg1.id);
  }
  if (arg2Type == ArgType::id && arg2.id->isTemp()) {
    in.insert(arg2.id);
  }
  return in;
}

std::set<SymVar*> ParamValQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (paramType == ArgType::id && param.id->isTemp()) {
    in.insert(param.id);
  }
  return in;
}

std::set<SymVar*> ParamRefQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (param->isTemp()) {
    in.insert(param);
  }
  return in;
}

std::set<SymVar*> RetrieveQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(var);
  return in;
}

std::set<SymVar*> ReturnQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}

std::set<SymVar*> IndexQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (indexType == ArgType::id && index.id->isTemp()) {
    in.insert(index.id);
  }
  return in;
}

std::set<SymVar*> IndexAsigQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (indexType == ArgType::id && index.id->isTemp()) {
    in.insert(index.id);
  }
  if (argType == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}

std::set<SymVar*> WriteQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}

std::set<SymVar*> ReadQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (deref) {
    if (result->isTemp()) {
      in.insert(result);
    }
  } else {
    if (result->isTemp()) {
      in.erase(result);
    }
  }
  return in;
}

std::set<SymVar*> ReadIndexQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  if (indext == ArgType::id && index.id->isTemp()) {
    in.insert(index.id);
  }
  return in;
}

std::set<SymVar*> CastFtoIQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}

std::set<SymVar*> CastItoFQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}
std::set<SymVar*> CastCtoIQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}
std::set<SymVar*> CastItoCQ::recalcIN(std::set<SymVar*> out) {
  std::set<SymVar*> in = out;
  in.erase(result);
  if (argt == ArgType::id && arg.id->isTemp()) {
    in.insert(arg.id);
  }
  return in;
}
