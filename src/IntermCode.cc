#include <iostream>
#include "IntermCode.hh"

Label* IntermCode::newLabel(){
  Label* newLabel= new Label(this->nextlabel++);
  return newLabel;   
}

void IntermCode::emitLabel(Label* label){
  this->unSet.push_back(label);
  std::cout << "l" << label->getId() << ": ";
}

void IntermCode::emitLabel2(Quad* instr){
  // Asocia las etiquetas de unSet a la instruccion
 for (std::list<Label*>::iterator it = this->unSet.begin();
       it != this->unSet.end(); it++) {
   this->labelset.insert(labels::value_type((*it)->getId(),(*it)));
   (*it)->setInstruction(instr);
   // Falta sacarlos de la lista, no quiero destructor
 }
 
}

bool IntermCode::areUnSet(){
  return this->unSet.size()!=0;
}

void IntermCode::addInst(Quad* quad){
  if(areUnSet()) emitLabel2(quad);
  this->inst.push_back(quad);
  quad->printQuad();
}

SymVar* IntermCode::newTemp(){
  return new SymVar(this->nextTemp++);
}

void Label::setInstruction(Quad* instruction){
  this->instruction= instruction;
}

int Label::getId(){ return this->id;}

