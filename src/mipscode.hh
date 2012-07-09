#ifndef DEVANIX_MIPSCODE
#define DEVANIX_MIPSCODE

#include <string>

#include "label.hh"
#include "instruction.hh"

class MIPSCode {
  int str_count;

public:
  MIPSCode() : str_count(0) {};
  Label* emitString(std::string str);
  Label* emitVar(std::string name, int size, int alignment);

  void emitData();
  void emitText();
  void emitComment(std::string comm);

  void emitLabel(Label* l);
  void emitInst(Instruction* mi);
};

#endif
