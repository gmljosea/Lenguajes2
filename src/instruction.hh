#ifndef DEVANIX_INSTRUCTION
#define DEVANIX_INSTRUCTION

#include <list>
#include "flowgraph.hh"
#include "symbol.hh"

class BasicBlock;
class SymFunction;

class Instruction {
  BasicBlock* b;
public:
  void setBlock(BasicBlock* b);
  BasicBlock* getBlock();
  virtual bool isJump();
  virtual bool isCall();
  virtual SymFunction* getCallTarget();
  virtual std::list<BasicBlock*> getTargetBlocks();
  virtual bool isHardJump();
  virtual bool isMainReturn();

  virtual std::string toString();
};

#endif
