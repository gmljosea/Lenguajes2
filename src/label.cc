#include "label.hh"
#include "Quad.hh"

void Label::setInstruction(Quad* instruction){
  this->instruction= instruction;
}

void Label::setActive(bool a) {
  this->active = a;
}

bool Label::isActive() {
  return this->active;
}

Quad* Label::getInstruction() {
  return this->instruction;
}

int Label::getId(){ return this->id;}

std::string Label::toString() {
  return this->new_id;
}
