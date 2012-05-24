#include <iostream>
#include "IntermCode.hh"
#include "statement.hh"

extern IntermCode intCode;

void Statement::gen(Label* next) {
  std::cout << " ! No implementado" << std::endl;
}

void Block::gen(Label* next) {
  Statement* last = (this->stmts).back();
  (this->stmts).pop_back();
  for (std::list<Statement*>::iterator it = (this->stmts).begin();
       it != (this->stmts).end(); it++) {
    Label* newlab = intCode.newLabel();
    (**it).gen(newlab);
    intCode.emitLabel(newlab);
  }
  if (next == NULL) {
    next = intCode.newLabel();
    last->gen(next);
    intCode.emitLabel(next);
  } else {
    last->gen(next);
  }
  (this->stmts).push_back(last);
}

void Null::gen(Label* next) {
  std::cout << "nop" << std::endl;
}

void If::gen(Label* next) {
  // Estoy asumiendo que next no va a ser NULL porque
  // este If debe estar en al menos un Block que debió crear un label
  // next si hacía falta
  Label* cfalse = this->block_false ? intCode.newLabel() : next;
  this->cond->jumping(NULL, cfalse);
  this->block_true->gen(next);
  if (this->block_false) {
    std::cout << "goto l" << next->getId() << std::endl;
    intCode.emitLabel(cfalse);
    this->block_false->gen(next);
  }
}

/*
 * Recordar meter los labels en la clase para los Break y Next
 */
void BoundedFor::gen(Label* next) {
  std::cout << "bounded for" << std::endl;
}

void While::gen(Label* next) {
  this->init = intCode.newLabel();
  this->cond->jumping(NULL, next);
  this->block->gen(init);
  std::cout << "goto l" << init->getId() << std::endl;
}

void ForEach::gen(Label* next) {
  std::cout << "Foreach" << std::endl;
}

void Asignment::gen(Label* next) {
  std::cout << "Asignment" << std::endl;
}

void VariableDec::gen(Label* next) {
  for (std::list<std::pair<SymVar*,Expression*>>::iterator it =
	 (this->decls).begin(); it != (this->decls).end(); it++) {
    if (!it->second) continue;
    SymVar* addr = ((*it).second)->gen();
    std::cout << ((*it).first)->getId() << " := " << addr->getId() << std::endl;
  }
}

void Break::gen(Label* next) {
  std::cout << "goto l" << this->loop->exit->getId() << std::endl;
}

void Next::gen(Label* next) {
  std::cout << "goto l" << this->loop->init->getId() << std::endl;
}

void Return::gen(Label* next) {
  SymVar* addr = this->exp->gen();
  std::cout << "return " << addr->getId() << std::endl;
}

void FunctionCall::gen(Label* next) {
  this->exp->gen();
}

void Write::gen(Label* next) {
  for (std::list<Expression*>::iterator it = (this->exps).begin();
       it != (this->exps).end(); it++) {
    SymVar* addr = (*it)->gen();
    // WARNING, falta que escupa writeln cuando debe
    std::cout << "write " << (*it)->getType()->toString()
	      << " " << addr->getId() << std::endl;
  }
}

void Read::gen(Label* next) {
  std::cout << "read" << std::endl;
}
