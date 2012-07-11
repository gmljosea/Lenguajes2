#include "label.hh"
#include "Quad.hh"

Label::Label(int id) {
  this->id = id;
  this->new_id = std::string(".t");
  this->new_id.append(std::to_string((long long int) id));
  active = false;
}

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
