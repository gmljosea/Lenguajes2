#include "program.hh"

Program::Program() {}

void Program::push_back_global(VariableDec* var) {
  this->globalvars.push_back(var);
}

void Program::push_back_global(SymFunction* func) {
  this->functions.push_back(func);
}
