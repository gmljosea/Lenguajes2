#include <map>
#include <set>
#include <string>

#include "flowgraph.hh"
#include "mipscode.hh"
#include "symbol.hh"
#include "Quad.hh"

extern MIPSCode mipscode;

std::set<SymVar*> liveTemps;

void FlowGraph::analyzeTemps() {
  // Todos los bloques deben tener IN = vacio
  bool change = true;
  while (change) {

    bool change = entry->recalcIN();
    for (std::list<BasicBlock*>::iterator it = blocks.begin();
	 it != blocks.end(); it++) {
      bool change = change || (*it)->recalcIN();
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
      BasicBlock* target_block = li->getTargetLabel()->getInstruction()->getBlock();
      li->replaceTargetLabel(target_block->getLabel());
      b->setAlternate(target_block);
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

BasicBlock* FlowGraph::getExit() {
  return exit;
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
  if (this->visited == true) return;
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

// FIXME
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
  // Ahora en outs tengo una lista de igual longitud a las instrucciones
  // Para cada instruccion, su correspondiente en la lista outs es el conjunto
  // de temporales vivas después.

  std::list<Instruction*> new_insts;

  for (std::list<Instruction*>::iterator it = insts.begin();
       it != insts.end(); it++) {
    liveTemps = outs.front();
    outs.pop_front();
    std::list<Instruction*> is = (*it)->gen();
    new_insts.splice(new_insts.end(), is);
  }

  // FIXME
  // Vaciar descriptores de registros, generando stores si hace falta

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

// FIXME
bool BasicBlock::recalcIN() {
  /*
    Pedir los IN de los sucesores
    Sacar la union
    Ir de atrás hacia adelante en la lista de instrucciones
      reclaculando IN con la función de transferencia
    Determinar si hubo cambios y devolver.

    new_in = Instruction::recalcIN(out)
   */
  return false;
}

std::set<SymVar*> BasicBlock::getIN() {
  return t_in;
}

std::string BasicBlock::toString() {
  std::string result(this->label->toString());
  std::string nl("\\l\\n");

  for (std::list<Instruction*>::iterator it = this->insts.begin();
       it != this->insts.end(); it++) {
    result.append(nl);
    result.append((*it)->toString());
  }
  result.append(std::string("\\l"));

  return result;
}
