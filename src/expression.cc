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

FunCallExp::FunCallExp(std::list<Expression*> args) {
  this->args = args;
}
