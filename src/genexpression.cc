#include <iostream>
#include <utility>
#include "expression.hh"
#include "IntermCode.hh"
#include "type.hh"

extern IntermCode intCode;

GenLvalue Expression::genlvalue() {
  return { new SymVar(500), new SymVar(501), 0 };
}

GenLvalue VarExp::genlvalue() {
  return { this->symv, NULL, 0 };
}

GenLvalue Index::genlvalue() {
  Args arg1;
  Args arg2;

  // Listo pero falta probar
  ArrayType* arrayt = dynamic_cast<ArrayType*>(this->array->getType());
  int elemsize = arrayt->getBaseType()->getSize();

  GenLvalue arrayloc = this->array->genlvalue();

  IntExp* cind;
  if (cind = dynamic_cast<IntExp*>(this->index)) {
    return { arrayloc.base, arrayloc.doff,
	arrayloc.coff + (cind->getInteger() * elemsize) };
  } else {
    SymVar* indexaddr = this->index->gen();
    SymVar* newindex = intCode.newTemp();
    // DONE QUAD: newindex := indexaddr * elemsize
    arg1.id = indexaddr;
    arg2.constint = elemsize;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::multiplicationI,
				     ArgType::constint, arg2,
				     newindex));

    if (arrayloc.doff == NULL) {
      return { arrayloc.base, newindex, arrayloc.coff };
    } else {
      // DONE QUAD: doff := doff + newindex
      arg1.id = arrayloc.doff;
      arg2.id = newindex;
      intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				       Operator::sumI,
				       ArgType::id, arg2,
				       arrayloc.doff));
      return arrayloc;
    }
  }
}

SymVar* Index::gen() {
  Args arg1;
  Args arg2;

  ArrayType* arrayt = dynamic_cast<ArrayType*>(this->array->getType());
  int elemsize = arrayt->getBaseType()->getSize();

  GenLvalue arrayloc = this->array->genlvalue();

  if (arrayloc.doff == NULL) {
    arrayloc.doff = intCode.newTemp();
    // DONE QUAD: doff := 0
    arg1.constint = 0;
    intCode.addInst(new AsignmentQ(ArgType::constint, arg1, arrayloc.doff));
  }

  SymVar* addr = intCode.newTemp();
  IntExp* cind;
  if (cind = dynamic_cast<IntExp*>(this->index)) {
    // DONE QUAD: doff := doff + (coff + <index * elemsize>)
    arg1.id = arrayloc.doff;
    arg2.constint = arrayloc.coff + (cind->getInteger() * elemsize);
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::constint, arg2,
				     arrayloc.doff));
  } else {
    SymVar* indaddr = this->index->gen();
    // DONE QUAD: indaddr := indaddr * elemsize
    arg1.id = indaddr;
    arg2.constint = elemsize;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::multiplicationI,
				     ArgType::constint, arg2,
				     indaddr));

    // DONE QUAD: doff := doff + coff
    arg1.id = arrayloc.doff;
    arg2.constint = arrayloc.coff;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::constint, arg2,
				     arrayloc.doff));

    // DONE QUAD: doff := doff + indaddr
    arg1.id = arrayloc.doff;
    arg2.id = indaddr;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     arrayloc.doff));

  }

  if (arrayloc.base->isReference()) {
    // DONE QUAD: doff := doff + base
    arg1.id = arrayloc.doff;
    arg2.id = arrayloc.base;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     arrayloc.doff));

    // DONE QUAD: addr := *doff
    arg1.id = arrayloc.doff;
    intCode.addInst(new AsignmentPointQ(arrayloc.doff, addr));

  } else {
    // DONE QUAD: addr := base[doff]
    arg1.id = arrayloc.doff;
    intCode.addInst(new IndexQ(arrayloc.base, ArgType::id, arg1, addr));
  }
  return addr;
}

GenLvalue Dot::genlvalue() {
  GenLvalue boxloc = this->box->genlvalue();
  BoxType* boxt = dynamic_cast<BoxType*>(this->box->getType());
  BoxField* boxf = boxt->getField(this->field);
  int offset = boxf->offset;

  return { boxloc.base, boxloc.doff, boxloc.coff+offset };
}

SymVar* Dot::gen() {
  Args arg1;
  Args arg2;

  GenLvalue boxloc = this->box->genlvalue();
  BoxType* boxt = dynamic_cast<BoxType*>(this->box->getType());
  BoxField* boxf = boxt->getField(this->field);
  int offset = boxf->offset;

 if (boxloc.doff == NULL) {
    boxloc.doff = intCode.newTemp();
    // DONE QUAD: doff := 0
    arg1.constint = 0;
    intCode.addInst(new AsignmentQ(ArgType::constint, arg1, boxloc.doff));
  }

  SymVar* addr = intCode.newTemp();
  // DONE QUAD: doff := doff + <(coff+offset)>
  arg1.id = boxloc.doff;
  arg2.constint = boxloc.coff + offset;
  intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				   Operator::sumI,
				   ArgType::constint, arg2,
				   boxloc.doff));

  if (boxloc.base->isReference()) {
    // DONE QUAD: doff := doff + base
    arg1.id = boxloc.doff;
    arg2.id = boxloc.base;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     boxloc.doff));

    // DONE QUAD: addr := *doff
    intCode.addInst(new AsignmentPointQ(boxloc.doff, addr));

  } else {
    // DONE QUAD: addr := base[doff]
    arg1.id = boxloc.doff;
    intCode.addInst(new IndexQ(boxloc.base, ArgType::id, arg1, addr));

  }
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
    return result;
  }else{
    return this->symv;
  }
}

void VarExp::jumping(Label* lbltrue,Label* lblfalse){
  Args sym;
  sym.id= this->symv;

  Args ctrue;
  ctrue.constbool= true;

  if(lblfalse!=NULL & lbltrue!=NULL){
    intCode.addInst(new ConditionalJumpQ(id,sym,equal,constbool,ctrue,lbltrue));
    intCode.addInst(new JumpQ(lblfalse));
  }else if(lbltrue!=NULL){
    intCode.addInst(new ConditionalJumpQ(id,sym,equal,constbool,ctrue,lbltrue));
  }else if(lblfalse!=NULL){
    intCode.addInst(new ConditionalJumpQ(id,sym,notEqual,constbool,ctrue,lblfalse));
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
}

void BoolExp::jumping(Label* lbltrue,Label* lblfalse){
  if(this->value & lbltrue!=NULL){
    intCode.addInst(new JumpQ(lbltrue));
  }else if(!(this->value) & lblfalse!=NULL){
    intCode.addInst(new JumpQ(lblfalse));			     
  }
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
}

// Minus

SymVar* Minus::gen(){
  SymVar *r1,*result;
  r1= this->exp1->gen();
  result= intCode.newTemp();
  if(*(this->type)==IntType::getInstance()){
    intCode.addInst(new AsignmentOpQ(r1,opI,NULL,result));
    result->setType(&(IntType::getInstance()));
  }else{
    intCode.addInst(new AsignmentOpQ(r1,opF,NULL,result));
    result->setType(&(FloatType::getInstance()));
  }
  return result;
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
     intCode.addInst(new ConditionalNJumpQ(r1,op,r2,lblfalse));
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
      intCode.addInst(new ParamValQ(temp));
    }
  }
  SymVar *result= intCode.newTemp();
  // Llamada a la funcion
  intCode.addInst(new CallQ(this->symf,this->symf->getArgumentCount(),result));
  return result;
}
