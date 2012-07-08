#ifndef DEVANIX_FLOWGRAPH
#define DEVANIX_FLOWGRAPH

#include <iostream>
#include <fstream>
#include <list>

#include "instruction.hh"

class Instruction;
class BasicBlock;

class FlowGraph {
  std::list<BasicBlock*> blocks;
  BasicBlock* entry;
  BasicBlock* exit;

  void analyzeTemps();

public:
  FlowGraph(std::list<Instruction*> insts);
  void toMIPS();
  void emitCode();
};

class BasicBlock {
  std::list<Instruction*> insts;
  std::list<BasicBlock*> children;

  bool visited;

protected:
  std::string name;

public:
  BasicBlock();
  void addInst(Instruction* i);
  bool isEmpty();
  void addEdge(BasicBlock* b);
  Instruction* getLastInst();
  void addEdges(std::list<BasicBlock*>* bs);

  void outputAsDot(std::ofstream& output);

  virtual std::string toString();
};

class EntryBlock : public BasicBlock {
public:
  EntryBlock();
};

class ExitBlock : public BasicBlock {
public:
  ExitBlock();
};

#endif
