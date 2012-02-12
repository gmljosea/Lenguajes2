#include <iostream>
#include <string>
#include "expression.hh"

void Expression::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Expresión" << std::endl;
}

VarExp::VarExp(SymVar* symv) {
  this->symv = symv;
}

IntExp::IntExp(int value) {
  this->value = value;
}

FloatExp::FloatExp(float value) {
  this->value = value;
}

BoolExp::BoolExp(bool value) {
  this->value = value;
}

StringExp::StringExp(std::string str) {
  this->str = str;
}

FunCallExp::FunCallExp(SymFunction* symf, std::list<Expression*> args) {
  this->symf = symf;
  this->args = args;
  this->checkedFunction = true;
}

FunCallExp::FunCallExp(std::list<Expression*> args) {
  this->symf = NULL;
  this->args = args;
  this->checkedFunction = false;
}
