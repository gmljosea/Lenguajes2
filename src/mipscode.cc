#include <iostream>
#include <string>

#include "mipscode.hh"

Label* MIPSCode::emitString(std::string str) {
  Label* l = new Label(std::string("str")
		       +std::to_string((long long int) str_count++));
  std::cout << l->toString() << ": .asciiz " << str << std::endl;
  return l;
}

Label* MIPSCode::emitVar(std::string name, int size, int alignment) {
  Label* l = new Label(std::string("g_")+name);
  std::cout << "  .align " << alignment << std::endl;
  std::cout << l->toString() << ": .space " << size << std::endl;
  return l;
}

void MIPSCode::emitData() {
  std::cout << ".data" << std::endl;
}

void MIPSCode::emitText() {
  std::cout << ".text" << std::endl;
}

void MIPSCode::emitComment(std::string comm) {
  std::cout << "# " << comm << std::endl;
}

void MIPSCode::emitLabel(Label* l) {
  std::cout << l->toString() << ":" << std::endl;
}

void MIPSCode::emitInst(Instruction* inst) {
  std::cout << "  " << inst->toString() << std::endl;
}
