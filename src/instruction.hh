#ifndef DEVANIX_INSTRUCTION
#define DEVANIX_INSTRUCTION

#include <list>
#include "flowgraph.hh"
#include "symbol.hh"

class BasicBlock;
class SymFunction;

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

  virtual std::string toString();

  
};

#endif
