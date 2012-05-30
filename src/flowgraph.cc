#include <string>

#include "flowgraph.hh"

BasicBlock::BasicBlock() {
  static int name_counter;
  this->name = std::string("B")
    + std::to_string((long long int) name_counter++);
}

void BasicBlock::addInst(Instruction* i) {
  this->insts.push_back(i);
}

bool BasicBlock::isEmpty() {
  return this->insts.empty();
}

void BasicBlock::addEdge(BasicBlock* b) {
  this->children.push_back(b);
}

Instruction* BasicBlock::getLastInst() {
  // Asumo que no es vacio
  return this->insts.back();
}

void BasicBlock::addEdges(std::list<BasicBlock*> bs) {
  this->children.insert(this->children.end(), bs.begin(), bs.end());
}

void BasicBlock::outputAsDot(std::ofstream& output) {
}
