#ifndef DEVANIX_FLOWGRAPH
#define DEVANIX_FLOWGRAPH

#include <iostream>
#include <fstream>
#include <list>

#include "instruction.hh"

class Instruction;

class BasicBlock {
  std::list<Instruction*> insts;
  std::list<BasicBlock*> children;
  std::string name;

  bool visited;

public:
  BasicBlock();
  void addInst(Instruction* i);
  bool isEmpty();
  void addEdge(BasicBlock* b);
  Instruction* getLastInst();
  void addEdges(std::list<BasicBlock*> bs);

  void outputAsDot(std::ofstream& output);

  std::string toString();
};

class EntryBlock : public BasicBlock {
};

class ExitBlock : public BasicBlock {
};

#endif
