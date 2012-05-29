#include <iostream>
#include <utility>
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
  //  std::cout << "nop" << std::endl;
}

void If::gen(Label* next) {
  // Estoy asumiendo que next no va a ser NULL porque
  // este If debe estar en al menos un Block que debió crear un label
  // next si hacía falta
  Label* cfalse = this->block_false ? intCode.newLabel() : next;
  this->cond->jumping(NULL, cfalse);
  this->block_true->gen(next);
  if (this->block_false) {
    // QUAD: goto next
    std::cout << "goto l" << next->getId() << std::endl;
    intCode.emitLabel(cfalse);
    this->block_false->gen(next);
  }
}

/*
 * Recordar meter los labels en la clase para los Break y Next
 */
void BoundedFor::gen(Label* next) {
  this->init = intCode.newLabel();
  this->exit = next;
  SymVar* lowert = this->lowerb->gen();
  SymVar* uppert = this->upperb->gen();
  SymVar* stept = this->step ? this->step->gen() : NULL;

  // QUAD: loopvar := lowerbound
  std::cout << this->varsym->getId() << " := "
	    << lowert->getId() << std::endl;

  intCode.emitLabel(this->init);

  // QUAD: if loopvar >= upperbound goto next
  std::cout << "if " << this->varsym->getId() << " >= "
	    << uppert->getId()
	    << " goto l" << next->getId() << std::endl;

  this->block->gen(init);

  if (this->step) {
    // QUAD: loopvar := loopvar + step
    std::cout << this->varsym->getId() << " := " << this->varsym->getId()
	      << " + " << stept->getId() << std::endl;
  } else {
    // QUAD: loopvar := loopvar + 1
    std::cout << this->varsym->getId() << " := " << this->varsym->getId()
	      << " + 1" << std::endl;
  }
  // QUAD: goto init
  std::cout << "goto l" << this->init->getId() << std::endl;
}

void While::gen(Label* next) {
  this->init = intCode.newLabel();
  this->exit = next;
  this->cond->jumping(NULL, next);
  this->block->gen(init);

  // QUAD: goto init
  std::cout << "goto l" << init->getId() << std::endl;
}

void ForEach::gen(Label* next) {
}

void Asignment::gen(Label* next) {
  std::list<SymVar*> temps;
  for (std::list<Expression*>::iterator it = (this->exps).begin();
       it != (this->exps).end(); it++) {
    SymVar* addr = (*it)->gen();
    temps.push_back(addr);
  }
  std::list<SymVar*>::iterator ittemps = temps.begin();
  std::list<Expression*>::iterator itlvals = (this->lvalues).begin();
  while (ittemps != temps.end()) {

    GenLvalue lvalue = (*itlvals)->genlvalue();
    if ( (*itlvals)->getType() == &(IntType::getInstance()) ) {
      if ( (lvalue.base)->isReference() ) {
	// QUAD: *base := temp
	std::cout << "*" << (lvalue.base)->getId() << " := "
		  << (*ittemps)->getId() << std::endl;
      } else {
	// QUAD: base := temp
	std::cout << (lvalue.base)->getId() << " := "
		  << (*ittemps)->getId() << std::endl;
      }
    } else {
      // QUAD: doff := doff + coff
      std::cout << (lvalue.doff)->getId() << " := "
		<< (lvalue.doff)->getId() << " + "
		<< lvalue.coff << std::endl;

      if ( (lvalue.base)->isReference() ) {
	// QUAD: doff := doff + base
	std::cout << (lvalue.doff)->getId() << " := "
		  << (lvalue.doff)->getId() << " + "
		  << (lvalue.base)->getId() << std::endl;
	// QUAD: *doff := temp
	std::cout << "*" << (lvalue.doff)->getId() << " := "
		  << (*ittemps)->getId() << std::endl;

      } else {
	// QUAD: base[doff] := temp
	std::cout << (lvalue.base)->getId() << "["
		  << (lvalue.doff)->getId() << "] := "
		  << (*ittemps)->getId() << std::endl;
      }
    }

    ittemps++;
    itlvals++;
  }
}

void VariableDec::gen(Label* next) {
  for (std::list<std::pair<SymVar*,Expression*>>::iterator it =
	 (this->decls).begin(); it != (this->decls).end(); it++) {
    if (!it->second) continue;
    SymVar* addr = ((*it).second)->gen();

    // QUAD: var := temp
    std::cout << ((*it).first)->getId() << " := " << addr->getId() << std::endl;
  }
}

void Break::gen(Label* next) {
  // QUAD: goto next
  std::cout << "goto l" << this->loop->exit->getId() << std::endl;
}

void Next::gen(Label* next) {
  // QUAD: goto init
  std::cout << "goto l" << this->loop->init->getId() << std::endl;
}

void Return::gen(Label* next) {
  if (this->exp) {
    SymVar* addr = this->exp->gen();
    // QUAD: return temp
    std::cout << "return " << addr->getId() << std::endl;
  } else{
    // QUAD: return
    std::cout << "return" << std::endl;
  }
}

void FunctionCall::gen(Label* next) {
  this->exp->gen();
}

void Write::gen(Label* next) {
  for (std::list<Expression*>::iterator it = (this->exps).begin();
       it != (this->exps).end(); it++) {
    SymVar* addr = (*it)->gen();

    // QUAD: write/writeln type temp
    if (this->isLn) {
      std::cout << "writeln";
    } else {
      std::cout << "write";
    }
    std::cout << (*it)->getType()->toString()
	      << " " << addr->getId() << std::endl;
  }
}

void Read::gen(Label* next) {
}
