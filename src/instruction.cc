#include <list>

#include "instruction.hh"
#include "symbol.hh"
#include "Quad.hh"

Instruction::Instruction() {
  isTarget = false;
}

std::list<Instruction*> Instruction::gen() {
  std::list<Instruction*> temp;
  return temp;
}

std::set<SymVar*> Instruction::recalcIN(std::set<SymVar*> set) {
  return set;
}

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

void Instruction::setLabels(std::list<Label*> labels) {
  this->labels.splice(this->labels.end(), labels);
}

bool Instruction::setJumpTarget(bool set) {
  this->isTarget = set;
}

bool Instruction::isJumpTarget() {
  for (std::list<Label*>::iterator it = this->labels.begin();
       it != this->labels.end(); it++) {
    if ((*it)->isActive()) return true;
  }
  return false;
}

std::string Instruction::toString() {
  std::string result("");
  for (std::list<Label*>::iterator it = this->labels.begin();
       it != this->labels.end(); it++) {
    result.append("l");
    result.append(std::to_string((long long int) (*it)->getId()));
    result.append(": ");
  }
  return result;
}
