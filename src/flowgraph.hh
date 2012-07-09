#ifndef DEVANIX_FLOWGRAPH
#define DEVANIX_FLOWGRAPH

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <set>

#include "instruction.hh"
#include "label.hh"
#include "symbol.hh"

class Instruction;
class BasicBlock;
class Label;

class FlowGraph {
  std::list<BasicBlock*> blocks;
  BasicBlock* entry;
  BasicBlock* exit;

  void analyzeTemps();

  std::string base;

public:
  FlowGraph(std::list<Instruction*> insts, std::string base);
  void toMIPS();
  void emitCode();
  BasicBlock* getExit();
  BasicBlock* getEntry();
  void output();
};

class BasicBlock {
  std::list<Instruction*> insts;

  BasicBlock* next;
  BasicBlock* alternate;

  bool visited;
  Label* label;

  std::set<SymVar*> t_in;
  std::set<SymVar*> t_out;

public:
  BasicBlock(Label* label) : visited(false), label(label), next(NULL),
			     alternate(NULL) {};
  void addInst(Instruction* i);
  bool isEmpty();
  Instruction* getLastInst();

  void outputAsDot(std::ofstream& output);

  void setNext(BasicBlock* next);
  void setAlternate(BasicBlock* alt);

  void toMIPS();
  void emitCode();

  void setVisited(bool v);
  Label* getLabel();

  bool recalcIN(); // Actualiza el IN y devuelve si hubo algun cambio
  std::set<SymVar*> getIN();

  virtual std::string toString();
};

#endif
