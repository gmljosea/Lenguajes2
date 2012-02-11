#include <iostream>
#include <string>
#include "expression.hh"

void Expression::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Expresión" << std::endl;
}

FunCallExp::FunCallExp(std::list<Expression*> args) {
  this->args = args;
}
