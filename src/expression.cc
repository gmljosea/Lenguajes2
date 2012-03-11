#include <iostream>
#include <string>
#include "expression.hh"
#include "program.hh"

extern Program program;

// Expression
void Expression::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Expresión" << std::endl;
}

void Expression::setLocation(int fline, int fcol, int lline, int lcol) {
  this->fline = fline;
  this->fcol = fcol;
  this->lline = lline;
  this->lcol = lcol;
}

void Expression::check() {
  // Cada check chequea recursivamente las subexpresiones, si hay
  // y setea el campo type de manera correspondiente
  // Si no se hace check antes de getType, el tipo podría quedar nulo
}

Type* Expression::getType() { return this->type; }
bool Expression::isBad() { return false; }
Expression* Expression::reduce() { return this; }
bool Expression::isConstant() { return false; }
int Expression::getInteger() { return 0; }
double Expression::getFloat() { return 0.0; }
bool Expression::getBool() { return true; }


// BadExp
BadExp::BadExp() {
  this->type = &(ErrorType::getInstance());
}

bool BadExp::isBad() {
  return true;
}


// VarExp
VarExp::VarExp(SymVar* symv) {
  this->symv = symv;
  this->type = symv->getType();
}

void VarExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << symv->getId() << std::endl;
}


// Constant
bool Constant::isConstant() { return true; }


// IntExp
IntExp::IntExp(int value) {
  this->value = value;
  this->type = &(IntType::getInstance());
}

void IntExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

int IntExp::getInteger() {
  return this->value;
}


// FloatExp
FloatExp::FloatExp(float value) {
  this->value = value;
  this->type = &(FloatType::getInstance());
}

void FloatExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

double FloatExp::getFloat() {
  return this->value;
}


// BoolExp
BoolExp::BoolExp(bool value) {
  this->value = value;
  this->type = &(BoolType::getInstance());
}

void BoolExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  if (value) {
    std::cout << padding << "true" << std::endl;
  } else {
    std::cout << padding << "false" << std::endl;
  }
}

bool BoolExp::getBool() {
  return this->value;
}


// StringExp
StringExp::StringExp(std::string str) {
  this->str = str;
  this->type = new StringType(str.length());
}

void StringExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "\"" << this->str << "\"" << std::endl;
}


// CharExp
CharExp::CharExp(std::string ch) {
  this->ch = ch;
  this->type = &(CharType::getInstance());
}

void CharExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "'" << this->ch << "'" << std::endl;
}


// BinaryOp
void BinaryOp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->exp1->print(nesting+1);
  std::cout << padding << this->op << std::endl;
  this->exp2->print(nesting+1);
}

// Sum
void Sum::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  if (*t1 == *t2 and
      (*t1 == IntType::getInstance() or *t1 == FloatType::getInstance())) {
    this->type = t1;
    return;
  }
  // Errores
  program.error("Suma no cuadra", this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
  return;
}

Expression* Sum::reduce() {
  this->exp1 = this->exp1->reduce();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->reduce();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()+exp2->getInteger());
  } else {
    result = new FloatExp(exp1->getFloat()+exp2->getFloat());
  }
  //result->setLocation(exp1->fline,exp1->fcol,exp2->lline,exp2->lcol);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// FunCall
FunCallExp::FunCallExp(SymFunction* symf, std::list<Expression*> args) {
  this->symf = symf;
  this->args = args;
  this->checkedFunction = true;
}

FunCallExp::FunCallExp(std::string name, std::list<Expression*> args) {
  this->symf = NULL;
  this->args = args;
  this->checkedFunction = false;
  this->name = name;
}


Type* FunCallExp::getType() {
  if (!this->checkedFunction) {
    // Buscar en la tabla de símbolos esta función
    // Si no existe, devuelve VoidType
  }
  // !!! return this->symf->getType();
  return &(VoidType::getInstance());
}

void FunCallExp::check() {
  if (checkedFunction) return;
  SymFunction* symf = program.symtable.lookup_function(name);
  if (symf == NULL) {
    program.error("llamada a función no declarada '"+name+"'", 0, 0);
  } else {
    checkedFunction = true;
  }
}
