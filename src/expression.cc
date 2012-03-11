#include <iostream>
#include <string>
#include "expression.hh"
#include "program.hh"

extern Program program;

void Expression::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Expresión" << std::endl;
}

bool Expression::isBad() {
  return false;
}

bool BadExp::isBad() {
  return true;
}

Type* BadExp::getType() {
  return &(ErrorType::getInstance());
}

VarExp::VarExp(SymVar* symv) {
  this->symv = symv;
}

void VarExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << symv->getId() << std::endl;
}

Type* VarExp::getType() {
  return symv->getType();
}

IntExp::IntExp(int value) {
  this->value = value;
}

Type* IntExp::getType(){
  return &(IntType::getInstance());
}

void IntExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

FloatExp::FloatExp(float value) {
  this->value = value;
}

Type* FloatExp::getType() {
  return &(FloatType::getInstance());
}

void FloatExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

BoolExp::BoolExp(bool value) {
  this->value = value;
}

Type* BoolExp::getType() {
  return &(BoolType::getInstance());
}

void BoolExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  if (value) {
    std::cout << padding << "true" << std::endl;
  } else {
    std::cout << padding << "false" << std::endl;
  }
}

StringExp::StringExp(std::string str) {
  this->str = str;
}

// !!! Nota, esto esta mal, lo hice solo para que compilara
Type* StringExp::getType() {
  return new StringType(1);
}

CharExp::CharExp(std::string ch) {
  this->ch = ch;
}

Type* CharExp::getType() {
  return &(CharType::getInstance());
}

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

// Suma
Type* SumExp::getType() {
  return &(IntType::getInstance());
}

// !!!! Nota: cambiar a un print de ops. binarios generalizado
void SumExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->op1->print(nesting+1);
  std::cout << padding << "+" << std::endl;
  this->op2->print(nesting+1);
}
