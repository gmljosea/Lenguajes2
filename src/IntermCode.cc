#include <iostream>
#include <fstream>
#include <list>

#include "flowgraph.hh"
#include "IntermCode.hh"
#include "parser.hh"
#include "symbol.hh"

extern SymFunction* currentfun;

Label* IntermCode::newLabel(){
  Label* newLabel= new Label(this->nextlabel++);
  //std::cout << "emiti etiqueta" << newLabel->getId(); 
  return newLabel;   
}

void IntermCode::emitLabel(Label* label){
  this->unSet.push_back(label);
  //  std::cout << "l" << label->getId() << ": ";
}

void IntermCode::emitLabel2(Instruction* instr){
  // Asocia las etiquetas de unSet a la instruccion
  instr->setLabels(this->unSet);
 for (std::list<Label*>::iterator it = this->unSet.begin();
       it != this->unSet.end(); it++) {
   this->labelset.insert(labels::value_type((*it)->getId(),(*it)));
   (*it)->setInstruction(instr);
   instr->setJumpTarget(true);
 } 
 this->unSet.clear();
}

bool IntermCode::areUnSet(){
  return this->unSet.size()!=0;
}

void IntermCode::addInst(Instruction* quad){
  if(areUnSet()) emitLabel2(quad);
  this->inst.push_back(quad);
  //  quad->printQuad();
}

SymVar* IntermCode::newTemp(){
  return new SymVar(this->nextTemp++);
}


BasicBlock* IntermCode::splitBlocks() {
  // Primera pasada: crear bloques

  /*  std::list<BasicBlock*> block_list;
  BasicBlock* current_block = new BasicBlock();

  for (std::list<Instruction*>::iterator it = (this->inst).begin();
       it != (this->inst).end(); it++) {
    Instruction* q = *it;

    if (q->isJumpTarget() and !current_block->isEmpty()) {
      block_list.push_back(current_block);
      current_block = new BasicBlock();
    }

    q->setBlock(current_block);
    current_block->addInst(q);

    if (q->isJump()) {
      block_list.push_back(current_block);
      current_block = new BasicBlock();
      if (q->isCall()) {
	q->getCallTarget()->addReturnTarget(current_block);
      }
    }
  }

  if (!current_block->isEmpty()) {
    block_list.push_back(current_block);
  }

  // Segunda pasada: conectar bloques
  BasicBlock* entry_block = new EntryBlock();
  BasicBlock* exit_block = new ExitBlock();

  //entry_block->addEdge(block_list.front());
  BasicBlock* previous_block = entry_block;
  Instruction* previous_inst = NULL;

  for (std::list<BasicBlock*>::iterator it = block_list.begin();
       it != block_list.end(); it++)  {
    BasicBlock* b = *it;
    Instruction* li = b->getLastInst();

    std::list<BasicBlock*> succ = li->getTargetBlocks();

    //    b->addEdges(&succ);

    //    std::cout << "Voy de " << b->toString() << std::endl;
    for (std::list<BasicBlock*>::iterator it = succ.begin();
	 it != succ.end(); it++) {
      //std::cout << "   suc: " << (*it)->toString() << std::endl;
      b->addEdge(*it);
    }

    if (previous_inst == NULL or !previous_inst->isHardJump()) {
	previous_block->addEdge(b);
    }

    if (li->isMainReturn()) {
      b->addEdge(exit_block);
    }

    previous_block = b;
    previous_inst = li;

  }

  return entry_block;*/
}

std::list<Instruction*> IntermCode::getInstructions() {
  return inst;
}

void IntermCode::clear() {
  unSet.clear();
  inst.clear();
  labelset.clear();
}
