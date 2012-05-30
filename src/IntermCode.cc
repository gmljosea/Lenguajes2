#include <iostream>
#include <fstream>
#include <list>

#include "flowgraph.hh"
#include "IntermCode.hh"

Label* IntermCode::newLabel(){
  Label* newLabel= new Label(this->nextlabel++);
  //std::cout << "emiti etiqueta" << newLabel->getId(); 
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
 } 
 this->unSet.clear();
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

Quad* Label::getInstruction() {
  return this->instruction;
}

int Label::getId(){ return this->id;}

BasicBlock* IntermCode::splitBlocks() {
  // Primera pasada: crear bloques

  std::list<BasicBlock*> block_list;
  BasicBlock* current_block;

  for (std::list<Instruction*>::iterator it = (this->inst).begin();
       it != (this->inst).end(); it++) {
    Instruction* q = *it;
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

  entry_block->addEdge(block_list.front());
  BasicBlock* previous_block = entry_block;

  for (std::list<BasicBlock*>::iterator it = block_list.begin();
       it != block_list.end(); it++)  {
    BasicBlock* b = *it;
    Instruction* li = b->getLastInst();

    std::list<BasicBlock*> succ = li->getTargetBlocks();
    b->addEdges(succ);

    if (!li->isHardJump()) {
      if (b != block_list.back()) {
	previous_block->addEdge(b);
      } else {
	b->addEdge(exit_block);
      }
    }

    if (li->isMainReturn()) {
      b->addEdge(exit_block);
    }

    previous_block = b;

  }

  /*  ofstream output;
  output.open("flowgraph.dot", ios::out | ios::trunc);
  output << "digraph flow { " << std::endl;
	 << "  node [shape=box]" << std::endl;
  entry_block->outputAsDot(output);
  output << "}" << std::endl;
  output.close();*/

  return entry_block;
}
