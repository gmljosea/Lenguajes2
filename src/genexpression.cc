#include <iostream>
#include <utility>
#include "expression.hh"
#include "IntermCode.hh"
#include "type.hh"

extern IntermCode intCode;

std::pair<SymVar*,SymVar*> Expression::genlvalue() {
  SymVar* t = new SymVar("lvalue_base",0,0,false,0);
  SymVar* o = new SymVar("lvalue_offset",0,0,false,0);
  return std::pair<SymVar*,SymVar*>(t,o);
}

std::pair<SymVar*,SymVar*> VarExp::genlvalue() {
  return std::pair<SymVar*,SymVar*>(this->symv,(SymVar*)NULL);
}

std::pair<SymVar*,SymVar*> Index::genlvalue() {
  SymVar* offset = this->index->gen();
  // Este cast debe funcionar porque ya debió chequearse el tipo en check()
  ArrayType* arrt = dynamic_cast<ArrayType*>(this->array->getType());
  int elem_size = arrt->getBaseType()->getSize();

  std::pair<SymVar*,SymVar*> location = this->array->genlvalue();
  if (location.second) {
    // offset := offset * tamaño_elemento
    std::cout << offset->getId() << " := "
	      << offset->getId() << " * "
	      << elem_size << std::endl;
    return std::pair<SymVar*,SymVar*>(location.first, offset);
  } else {
    // off := off * tamaño_elemento
    std::cout << offset->getId() << " := "
	      << offset->getId() << " * "
	      << elem_size << std::endl;
    // base := base + offset
    std::cout << (location.second)->getId() << " := "
	      << (location.second)->getId() << " + "
	      << offset->getId() << std::endl;
    return location;
  }
}

std::pair<SymVar*,SymVar*> Dot::genlvalue() {
  SymVar* t = new SymVar("lvalue_base",0,0,false,0);
  SymVar* o = new SymVar("lvalue_offset",0,0,false,0);
  return std::pair<SymVar*,SymVar*>(t,o);
}

// Expression 

SymVar* Expression::gen(){
  std::cout << "gen exp" << std::endl;
  return new SymVar("test_gen",0,0,false,0);
};

void Expression::jumping(Label* ltrue, Label* lfalse) {
  std::cout << "jumping exp" << std::endl;
}

// VarExp

SymVar* VarExp::gen(){
  if(this->symv->isReference()){
    SymVar *result;
    result= intCode.newTemp();
    intCode.addInst(new AsignmentPointQ(this->symv,result));
    std::cout << "temp = varExpReferencia";
    return result;
  }else{
    std::cout << "temp = varExp";
    return this->symv;
  }
}

// IntExp

SymVar* IntExp::gen(){
  SymVar *result;
  result= intCode.newTemp();
  Args cInt;
  cInt.constint= this->value;
  intCode.addInst(new AsignmentQ(constint,cInt,result));
  result->setType(&(IntType::getInstance()));
  return result;
}

// FloatExp

SymVar* FloatExp::gen(){
 SymVar *result;
  result= intCode.newTemp();
  Args cFloat;
  cFloat.constfloat= this->value;
  intCode.addInst(new AsignmentQ(constfloat,cFloat,result));
  result->setType(&(FloatType::getInstance()));
  return result;
std::cout << "temp = float";
}

// BoolExp

SymVar* BoolExp::gen(){
  SymVar *result;
  result= intCode.newTemp();
  Args cBool;
  cBool.constbool= this->value;
  intCode.addInst(new AsignmentQ(constbool,cBool,result));
  result->setType(&(BoolType::getInstance()));
  return result;
  std::cout << "temp = bool";
}

// StringExp

SymVar* StringExp::gen(){
  SymVar *result;
  result= intCode.newTemp();
  Args cString;
  cString.conststring= &(this->str);
  intCode.addInst(new AsignmentQ(conststring,cString,result));
  result->setType(new StringType(this->str.length()));
  return result;
  std::cout << "temp = string";
}

// CharExp

SymVar* CharExp::gen(){
  SymVar *result;
  result= intCode.newTemp();
  Args cChar;
  cChar.constchar= this-> ch;
  intCode.addInst(new AsignmentQ(constchar,cChar,result));
  result->setType(&(CharType::getInstance()));
  return result;
  std::cout << "temp = char";
}

// Arithmetic: Sum, Substraction, Multiplication, Division  

SymVar* Arithmetic::gen(){
  SymVar *r1,*r2,*result;
  r1= this->exp1->gen();
  r2= this->exp2->gen();
  result= intCode.newTemp();
  if(*(this->type)==IntType::getInstance()){
    intCode.addInst(new AsignmentOpQ(r1,this->opI,r2,result));
    result->setType(&(IntType::getInstance()));
  }else{ 
    intCode.addInst(new AsignmentOpQ(r1,this->opF,r2,result));
    result->setType(&(FloatType::getInstance()));
  }
  return result;
  std::cout << "temp = aritmetico+-*//";
}


// Remainder 

SymVar* Remainder::gen(){
  SymVar *r1,*r2,*result;
  r1= this->exp1->gen();
  r2= this->exp2->gen();
  result= intCode.newTemp();
  intCode.addInst(new AsignmentOpQ(r1,remainder,r2,result));
  result->setType(&(IntType::getInstance()));
  return result;

  std::cout << "temp = mod";
}

// Minus

SymVar* Minus::gen(){
  SymVar *r1,*result;
  r1= this->exp1->gen();
  result= intCode.newTemp();
  if(*(this->type)==IntType::getInstance()){
    intCode.addInst(new AsignmentOpQ(r1,opI,result));
    result->setType(&(IntType::getInstance()));
  }else{
    intCode.addInst(new AsignmentOpQ(r1,opF,result));
    result->setType(&(FloatType::getInstance()));
  }
  return result;
  std::cout << "temp = menos unario";
}

// Logical: AND OR 

SymVar* Logical::gen(){
  
  Label* lblfalse;
  Label* lblFin;
  lblfalse= intCode.newLabel();
  lblFin=intCode.newLabel();

  SymVar* result;
  result= intCode.newTemp();
  Args tempTrue;
  Args tempFalse;
  tempTrue.constbool= (bool) true;
  tempFalse.constbool=(bool) false;
    
  this->jumping(NULL,lblfalse);

  intCode.addInst(new AsignmentQ(constbool,tempTrue,result));
  intCode.addInst(new JumpQ(lblFin));
  intCode.emitLabel(lblfalse);
  intCode.addInst(new AsignmentQ(constbool,tempFalse,result));
  intCode.emitLabel(lblFin);
  result->setType(&(BoolType::getInstance()));
  return result;
}

void And::jumping(Label *lbltrue,Label *lblfalse){

  Label *lblFALSE;
  lblFALSE= (lblfalse==NULL)? (intCode.newLabel()):lblfalse; 
  this->exp1->jumping(NULL,lblFALSE);
  this->exp2->jumping(lbltrue,lblfalse);
  if(lblfalse==NULL) intCode.emitLabel(lblFALSE);

}

void Or::jumping(Label *lbltrue,Label *lblfalse){

  Label *lblTRUE;
  lblTRUE= (lbltrue==NULL)? (intCode.newLabel()):lbltrue; 
  this->exp1->jumping(lblTRUE,NULL);
  this->exp2->jumping(lbltrue,lblfalse);
  if(lbltrue==NULL) intCode.emitLabel(lblTRUE);

}

// NOT

SymVar* Not::gen(){
  
    Label* lblfalse;
    Label* lblFin;
    lblfalse= intCode.newLabel();
    lblFin=intCode.newLabel();

    SymVar* result;
    result= intCode.newTemp();
    Args cTrue;
    Args cFalse;
    cTrue.constbool= (bool) true;
    cFalse.constbool=(bool) false;
 
    this->jumping(NULL,lblfalse);

    intCode.addInst(new AsignmentQ(constbool,cFalse,result));
    intCode.addInst(new JumpQ(lblFin));
    intCode.emitLabel(lblfalse);
    intCode.addInst(new AsignmentQ(constbool,cTrue,result));
    intCode.emitLabel(lblFin);
    result->setType(&(BoolType::getInstance()));
    return result;
}

void Not::jumping(Label *lbltrue,Label *lblfalse){
 
  this->exp1->jumping(lblfalse,lbltrue);

}

// Relational: Greater, GraterEq, Equal, NotEqual, Less, LessEq

SymVar* Relational::gen(){
  // Verificar que tipo de operador es
  Operator op= this->operatortype();

  SymVar *r1,*r2,*result;
  r1= this->exp1->gen();
  r2= this->exp2->gen();
  result=intCode.newTemp();

  Label *lbltrue,*lblFin;
  lbltrue=intCode.newLabel();
  lblFin=intCode.newLabel();

  Args cTrue;
  Args cFalse;
  cTrue.constbool= (bool) true;
  cFalse.constbool=(bool) false;
 
  intCode.addInst(new ConditionalJumpQ(r1,op,r2,lbltrue));
  intCode.addInst(new AsignmentQ(constbool,cFalse,result));
  intCode.addInst(new JumpQ(lblFin));
  intCode.emitLabel(lbltrue);
  intCode.addInst(new AsignmentQ(constbool,cTrue,result));
  intCode.emitLabel(lblFin);
  return result;
  std::cout << "temp = relational";
}

void Relational::jumping(Label* lbltrue,Label* lblfalse){
  // Verificar que tipo de operador es
  Operator op= this->operatortype();

  SymVar *r1,*r2;
  r1= this->exp1->gen();
  r2= this->exp2->gen();
  
  if(lblfalse!=NULL & lbltrue!=NULL){
    intCode.addInst(new ConditionalJumpQ(r1,op,r2,lbltrue));
    intCode.addInst(new JumpQ(lblfalse));
  }else if(lbltrue!=NULL){
    intCode.addInst(new ConditionalJumpQ(r1,op,r2,lbltrue));
  }else if(lblfalse!=NULL){
     intCode.addInst(new ConditionalNJumpQ(r1,op,r2,lbltrue));
  }
}

// FunCallExp

SymVar* FunCallExp::gen(){
  // Generar las instrucciones para cargar los parametros
  std::list<Expression*>::iterator arg= this->args.begin();
  for(arg; arg!=this->args.end(); arg++){
    SymVar *temp= (*arg)->gen();
    if(temp->isReference()){
      intCode.addInst(new ParamRefQ(temp));
    }else{
      intCode.addInst(new ParamVarQ(temp));
    }
  }
  SymVar *result= intCode.newTemp();
  // Llamada a la funcion
  intCode.addInst(new CallQ(this->symf,this->symf->getArgumentCount(),result));
  return result;
}
