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
  this->init = intCode.newLabel();
  this->exit = next;
  SymVar* lowert = this->lowerb->gen();
  SymVar* uppert = this->upperb->gen();
  SymVar* stept = this->step ? this->step->gen() : NULL;
  std::cout << this->varsym->getId() << " := " << lowert->getId() << std::endl;
  std::cout << "if " << this->varsym->getId() << " >= "
	    << uppert->getId() << " goto l" << next->getId() << std::endl;
  intCode.emitLabel(this->init);
  this->block->gen(init);
  if (this->step) {
    std::cout << this->varsym->getId() << " := " << this->varsym->getId()
	      << " + " << stept->getId() << std::endl;
  } else {
    std::cout << this->varsym->getId() << " := " << this->varsym->getId()
	      << " + 1" << std::endl;
  }
  std::cout << "goto l" << this->init->getId() << std::endl;
}

void While::gen(Label* next) {
  this->init = intCode.newLabel();
  this->exit = next;
  this->cond->jumping(NULL, next);
  this->block->gen(init);
  std::cout << "goto l" << init->getId() << std::endl;
}

void ForEach::gen(Label* next) {
  SymVar* sizet = intCode.newTemp();
  SymVar* counter = intCode.newTemp();

  std::pair<SymVar*,SymVar*> location = this->array->genlvalue();
  if (!location.first->isReference()) {

  }

  // Falta generar el código que inicialice loopvar, sizet y counter
  // Eso depende de la expresión: si se pasa o no por referencia, si es un
  // o no
  std::cout << "Init foreach" << std::endl;

  Label* init = intCode.newLabel();
  intCode.emitLabel(init);
  // if counter = 0 goto next
  std::cout << "if " << counter->getId() << " = 0 goto l"
	    << next->getId() << std::endl;
  this->block->gen(init);
  // i := i + sizet
  std::cout << this->loopvar->getId() << " := " << this->loopvar->getId()
	    << " + " << sizet->getId() << std::endl;
  // counter = counter - 1
  std::cout << counter->getId() << " := " << counter->getId()
	    << " - 1" <<std::endl;
  // goto init
  std::cout << "goto l" << init->getId() << std::endl;
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
    std::pair<SymVar*,SymVar*> lvalue = (*itlvals)->genlvalue();
    VarExp* var;
    if (var = dynamic_cast<VarExp*>(*itlvals)) {
      // Como es una variable, hay que generar a := b, o *a := b
      if ((lvalue.first)->isReference()) {
	std::cout << "*" << (lvalue.first)->getId() << " := "
		  << (*ittemps)->getId() << std::endl;
      } else {
	std::cout << (lvalue.first)->getId() << " := "
		  << (*ittemps)->getId() << std::endl;
      }
    } else {
      // Si no es variable es box o arreglo, entonces hay que generar
      // a[b] := c.

      // Si la base es un apuntador a la base, entonces dereferenciarlo
      SymVar* realval;
      if ((lvalue.first)->isReference()) {
	realval = intCode.newTemp();
	std::cout << realval->getId() << " := *"
		  << (lvalue.first)->getId() << std::endl;
      } else {
	realval = (lvalue.first);
      }
      std::cout << realval->getId() << "[" << (lvalue.second)->getId()
		<< "] := " << (*ittemps)->getId() << std::endl;
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
  std::pair<SymVar*,SymVar*> lvalue = this->lval->genlvalue();
  VarExp* var;
  if (var = dynamic_cast<VarExp*>(this->lval)) {
    if ((lvalue.first)->isReference()) {
      // Generar *a := read type
      std::cout << "*" << (lvalue.first)->getId() << " := read "
		<< this->lval->getType()->toString() << std::endl;
    } else {
      // Generar a := read type
      std::cout << (lvalue.first)->getId() << " := read "
		<< this->lval->getType()->toString() << std::endl;
    }
  } else {
    // Si no es variable es box o arreglo, entonces hay que generar
    // a[b] := c.

    // Si la base es un apuntador a la base, entonces dereferenciarlo
    SymVar* realval;
    if ((lvalue.first)->isReference()) {
      // Genera t1 := *a
      realval = intCode.newTemp();
      std::cout << realval->getId() << " := *"
		<< (lvalue.first)->getId() << std::endl;
    } else {
      realval = (lvalue.first);
    }

    // Genera base[off] := read type
    std::cout << realval->getId() << "[" << (lvalue.second)->getId()
	      << "] := read "
	      << this->lval->getType()->toString() << std::endl;
  }
}
