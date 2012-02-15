#include <iostream>
#include <string>
#include "expression.hh"

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
  return &(this->type);
}

VarExp::VarExp(SymVar* symv) {
  this->symv = symv;
}

Type* VarExp::getType() {
  return symv->getType();
}

IntExp::IntExp(int value) {
  this->value = value;
}

Type* IntExp::getType(){
  return &(this->type);
}

FloatExp::FloatExp(float value) {
  this->value = value;
}

Type* FloatExp::getType() {
  return &(this->type);
}

BoolExp::BoolExp(bool value) {
  this->value = value;
}

Type* BoolExp::getType() {
  return &(this->type);
}

StringExp::StringExp(std::string str) {
  this->str = str;
}

Type* StringExp::getType() {
  return &(this->type);
}

CharExp::CharExp(std::string ch) {
  this->ch = ch;
}

Type* CharExp::getType() {
  return &(this->type);
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
  return new VoidType();
}
