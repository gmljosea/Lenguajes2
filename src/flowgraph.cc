#include <map>
#include <set>
#include <string>

#include <fstream>

#include "flowgraph.hh"
#include "mipscode.hh"
#include "symbol.hh"
#include "Quad.hh"
#include "registers.hh"
#include "regdesc.hh"

extern MIPSCode mipscode;
extern RegDesc rdesc;

void FlowGraph::analyzeTemps() {
  // Todos los bloques deben tener IN = vacio
  bool change = true;
  while (change) {

    change = entry->recalcIN();
    for (std::list<BasicBlock*>::iterator it = blocks.begin();
	 it != blocks.end(); it++) {
      change = change || (*it)->recalcIN();
    }

  }
}


Label* genBlockLabel(std::string base, int n) {
  return new Label(base+std::to_string((long long int) n));
}

FlowGraph::FlowGraph(std::list<Instruction*> insts, std::string base) {
  // base es el nombre base para los labels de los bloques
  entry = new BasicBlock(new Label(base+std::string("entry")));
  exit  = new BasicBlock(new Label(base+std::string("exit")));

  int block_counter = 0;

  // Primera pasada: crear bloques
  BasicBlock* current_block = new BasicBlock(genBlockLabel(base, block_counter++));

  for (std::list<Instruction*>::iterator it = insts.begin();
       it != insts.end(); it++) {

    Instruction* q = *it;

    if (q->isJumpTarget() and !current_block->isEmpty()) {
      blocks.push_back(current_block);
      current_block = new BasicBlock(genBlockLabel(base, block_counter++));
    }

    q->setBlock(current_block);
    current_block->addInst(q);

    if (q->isJump()) {
      blocks.push_back(current_block);
      current_block = new BasicBlock(genBlockLabel(base, block_counter++));
    }
  }

  if (!current_block->isEmpty()) {
    blocks.push_back(current_block);
  }

  // Segunda pasada: agregar arcos
  BasicBlock* previous_block = entry;
  Instruction* previous_inst = NULL;

  for (std::list<BasicBlock*>::iterator it = blocks.begin();
       it != blocks.end(); it++) {
    BasicBlock* b = *it;
    Instruction* li = b->getLastInst();

    if (li->isJump() && !li->isReturn()) {
      Instruction* inst = li->getTargetLabel()->getInstruction();
      if (inst) {
	BasicBlock* target_block = li->getTargetLabel()->getInstruction()->getBlock();
	if (target_block) {
	  li->replaceTargetLabel(target_block->getLabel());
	  b->setAlternate(target_block);
	}
      }
    }

    if (previous_inst == NULL or !previous_inst->isHardJump()) {
      previous_block->setNext(b);
    }

    if (li->isReturn()) {
      b->setAlternate(exit);
    }

    previous_block = b;
    previous_inst = li;
  }

  if (previous_inst == NULL or !previous_inst->isHardJump()) {
    previous_block->setNext(exit);
  }

  this->base = base;

}

void FlowGraph::toMIPS() {
  analyzeTemps(); // En cada BasicBlock queda su OUT del dataflow
  for (std::list<BasicBlock*>::iterator it = blocks.begin();
       it != blocks.end(); it++) {
    (*it)->toMIPS();
  }
}

void FlowGraph::emitCode() {
  for (std::list<BasicBlock*>::iterator it = blocks.begin();
       it != blocks.end(); it++) {
    (*it)->setVisited(false);
  }
  entry->setVisited(false);
  exit->setVisited(true);
  entry->emitCode();
}

void FlowGraph::output(std::string prefix) {
  std::ofstream output;
  output.open(prefix+base+std::string(".dot"), std::ios::trunc);
  output << "digraph flowgraph {" << std::endl;
  output << "  node [shape=box, nojustify=true]" << std::endl;

  for (std::list<BasicBlock*>::iterator it = blocks.begin();
       it != blocks.end(); it++){
    (*it)->setVisited(false);
  }
  entry->setVisited(false);
  exit->setVisited(false);

  entry->outputAsDot(output);

  output << "}" << std::endl;
}

BasicBlock* FlowGraph::getExit() {
  return exit;
}

BasicBlock* FlowGraph::getEntry() {
  return entry;
}

void BasicBlock::addInst(Instruction* i) {
  this->insts.push_back(i);
}

bool BasicBlock::isEmpty() {
  return this->insts.empty();
}

Label* BasicBlock::getLabel() {
  return label;
}

Instruction* BasicBlock::getLastInst() {
  // Asumo que no es vacio
  return this->insts.back();
}

void BasicBlock::outputAsDot(std::ofstream& output) {
  if (this->visited) return;
  this->visited = true;
  std::string node_start = this->toString();
  if (next) {
    output << "\"" << node_start << "\" -> \"" << next->toString() << "\""
	   << std::endl;
    next->outputAsDot(output);
  }
  if (alternate) {
    output << "\"" << node_start << "\" -> \"" << alternate->toString() << "\""
	   << std::endl;
    alternate->outputAsDot(output);
  }
}

void BasicBlock::setNext(BasicBlock* next) {
  this->next = next;
}

void BasicBlock::setAlternate(BasicBlock* alt) {
  this->alternate = alt;
}

void BasicBlock::toMIPS() {
  std::list<std::set<SymVar*>> outs;
  std::set<SymVar*> current_out = t_out;
  outs.push_front(t_out);
  for (std::list<Instruction*>::reverse_iterator it = insts.rbegin();
       it != insts.rend(); it++) {
    std::set<SymVar*> t = (*it)->recalcIN(current_out);
    outs.push_front(t);
    current_out = t;
  }
  outs.pop_front();
  // Ahora en outs tengo una lista de igual longitud a las instrucciones
  // Para cada instruccion, su correspondiente en la lista outs es el conjunto
  // de temporales vivas después.

  std::list<Instruction*> new_insts;

  for (std::list<Instruction*>::iterator it = insts.begin();
       it != insts.end(); it++) {
    if ((*it)->isJump()) {
      std::list<Instruction*> stores = rdesc.emptyRegs();
      new_insts.splice(new_insts.end(), stores);
    }
    rdesc.liveTemps = outs.front();
    outs.pop_front();
    std::list<Instruction*> is = (*it)->gen();
    new_insts.splice(new_insts.end(), is);
  }

  // Vaciar descriptores de registros, generando stores si hace falta
  rdesc.liveTemps = this->t_out;
  std::list<Instruction*> stores = rdesc.emptyRegs();
  new_insts.splice(new_insts.end(), stores);

  this->insts = new_insts;
}

void BasicBlock::emitCode() {
  if (visited) return;
  visited = true;

  mipscode.emitLabel(label);
  for (std::list<Instruction*>::iterator it = insts.begin();
       it != insts.end(); it++) {
    mipscode.emitInst(*it);
  }

  if (next) next->emitCode();
  if (alternate) alternate->emitCode();
}

void BasicBlock::setVisited(bool v) {
  this->visited = v;
}

bool BasicBlock::recalcIN() {
  // Yo estoy claro en que estoy siendo brutalmente ineficiente
  // Con la copia intensa de sets de una lado a otro
  // Mientras sirva para esta entrega estoy bien
  std::set<SymVar*> new_out;
  if (next) {
    std::set<SymVar*> next_in = next->getIN();
    for (std::set<SymVar*>::iterator it = next_in.begin();
	 it != next_in.end(); it++) {
      new_out.insert(*it);
    }
  }
  if (alternate) {
    std::set<SymVar*> alt_in = alternate->getIN();
    for (std::set<SymVar*>::iterator it = alt_in.begin();
	 it != alt_in.end(); it++) {
      new_out.insert(*it);
    }
  }

  t_out = new_out;

  for (std::list<Instruction*>::reverse_iterator it = insts.rbegin();
       it != insts.rend(); it++) {
    new_out = (*it)->recalcIN(new_out);
  }

  if (t_in == new_out) return false;

  t_in = new_out;
  return true;
}

std::set<SymVar*> BasicBlock::getIN() {
  return t_in;
}

std::string BasicBlock::toString() {
  std::string result(this->label->toString());
  std::string nl("\\l\\n");

  result.append(nl);
  result.append(std::string("IN { "));
  for (std::set<SymVar*>::iterator it = this->t_in.begin();
       it != this->t_in.end(); it++) {
    result.append((*it)->getId());
    result.append(std::string(" , "));
  }
  result.append(std::string(" } "));

  for (std::list<Instruction*>::iterator it = this->insts.begin();
       it != this->insts.end(); it++) {
    result.append(nl);
    result.append((*it)->toString());
  }

  result.append(nl);
  result.append(std::string(" OUT { "));
  for (std::set<SymVar*>::iterator it = this->t_out.begin();
       it != this->t_out.end(); it++) {
    result.append((*it)->getId());
    result.append(std::string(" , "));
  }

  result.append(std::string(" } "));
  result.append(std::string("\\l"));

  return result;
}
