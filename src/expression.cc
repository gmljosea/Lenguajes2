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


// FloatExp
FloatExp::FloatExp(float value) {
  this->value = value;
  this->type = &(FloatType::getInstance());
}

void FloatExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
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
