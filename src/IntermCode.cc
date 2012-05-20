#include "IntermCode.hh"

Label* IntermCode::newLabel(){
  Label* newLabel= new Label(this->nextLabel++);
  return newLabel;   
}

void emitLabel(Label* label){
  this->unSet.insert(label);
}

void emitLabel2(Quad* instr){
  // Asocia las etiquetas de unSet a la instruccion
 for (std::list<Label*>::iterator it = this->unSet.begin();
       it != this->unSet.end(); it++) {
   this->labelset.insert(::value_type((*it)->getId(),instr));
 }
}

bool IntermCode::areUnSet(){
  return this->unSet.size()!=0;
}

void IntermCode::addInst(Quad* quad){
  if(areUnSet()) emitLabel2(quad);
  this->inst.push_back(quad);
}

void Label::setInstruction(Quad* instruction){
  this->instruction= instruction;
}
