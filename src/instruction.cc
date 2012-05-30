#include "instruction.hh"
#include "symbol.hh"

void Instruction::setBlock(BasicBlock* b) {
  this->b = b;
}

BasicBlock* Instruction::getBlock() {
  return this->b;
}

bool Instruction::isJump() {
  return false;
}

bool Instruction::isCall() {
  return false;
}

SymFunction* Instruction::getCallTarget() {
  return NULL;
}

std::list<BasicBlock*> Instruction::getTargetBlocks() {
  std::list<BasicBlock*> bla;
  return bla;
}

bool Instruction::isHardJump() {
  return false;
}

bool Instruction::isMainReturn() {
  return false;
}

std::string Instruction::toString() {
  return std::string("hola");
}
