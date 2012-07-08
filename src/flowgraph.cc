#include <string>

#include "flowgraph.hh"
#include "Quad.hh"

void FlowGraph::analyzeTemps() {
  // do nothing
  return;
}

FlowGraph::FlowGraph(std::list<Instruction*> insts) {
  // do nothing, por ahora
}

void FlowGraph::toMIPS() {
  return;
}

void FlowGraph::emitCode() {
  return;
}

BasicBlock::BasicBlock() {
  static int name_counter;
  this->visited = false;
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

void BasicBlock::addEdges(std::list<BasicBlock*>* bss) {
  //this->children.insert(this->children.end(), bs.begin(), bs.end());
  std::list<BasicBlock*> bs = *bss;
  std::cout << "Te lo tengo" << std::endl;
  for (std::list<BasicBlock*>::iterator it = this->children.begin();
       it != this->children.end(); it++) {
    std::cout << "BLABLE: " << (*it)->toString() << std::endl;
    this->children.push_back(*it);
  }
}

void BasicBlock::outputAsDot(std::ofstream& output) {
  if (this->visited == true) return;
  this->visited = true;
  std::string node_start = this->toString();
  for (std::list<BasicBlock*>::iterator it = this->children.begin();
       it != this->children.end(); it++) {
    std::string node_end = (*it)->toString();
    output << "\"" << node_start << "\" -> \"" << node_end << "\""
	   << std::endl;
    (*it)->outputAsDot(output);
  }
}

std::string BasicBlock::toString() {
  std::string result(this->name);
  std::string nl("\\l\\n");

  for (std::list<Instruction*>::iterator it = this->insts.begin();
       it != this->insts.end(); it++) {
    result.append(nl);
    result.append((*it)->toString());
  }
  result.append(std::string("\\l"));

  return result;
}

EntryBlock::EntryBlock() {
  this->name = std::string("ENTRY");
}

ExitBlock::ExitBlock() {
  this->name = std::string("EXIT");
}
