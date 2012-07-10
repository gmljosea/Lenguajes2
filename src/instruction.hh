#ifndef DEVANIX_INSTRUCTION
#define DEVANIX_INSTRUCTION

#include <list>
#include <set>
#include "flowgraph.hh"
#include "symbol.hh"

class BasicBlock;
class SymFunction;
class SymVar;
class Label;

class Instruction {
  bool isTarget;

  std::list<Label*> labels;
  BasicBlock* b;
public:
  Instruction();
  void setBlock(BasicBlock* b);
  BasicBlock* getBlock();
  virtual bool isJump();
  virtual bool isCall();
  virtual SymFunction* getCallTarget();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();
  virtual bool isMainReturn();

  virtual void setLabels(std::list<Label*> labels);
  virtual bool setJumpTarget(bool ist);
  virtual bool isJumpTarget();

  virtual bool isReturn();
  virtual Label* getTargetLabel();
  virtual void replaceTargetLabel(Label* l);

  virtual std::string toString();

  // Devuelve la traducción en MIPS, modificando la tabla de descriptores etc
  virtual std::list<Instruction*> gen();
  virtual std::set<SymVar*> recalcIN(std::set<SymVar*> set);
};

#endif
