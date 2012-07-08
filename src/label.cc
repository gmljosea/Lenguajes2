#include "label.hh"
#include "Quad.hh"

void Label::setInstruction(Instruction* instruction){
  this->instruction= instruction;
}

void Label::setActive(bool a) {
  this->active = a;
}

bool Label::isActive() {
  return this->active;
}

Instruction* Label::getInstruction() {
  return this->instruction;
}

int Label::getId(){ return this->id;}

std::string Label::toString() {
  return this->new_id;
}
