#include <iostream>
#include <utility>
#include "IntermCode.hh"
#include "statement.hh"

extern IntermCode intCode;

void Statement::gen(Label* next) {
  //  std::cout << " ! No implementado" << std::endl;
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
    // DONE QUAD: goto next
    intCode.addInst(new JumpQ(next));
    //std::cout << "goto l" << next->getId() << std::endl;
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

  Args arg1;
  Args arg2;

  // DONE QUAD: loopvar := lowerbound
  arg1.id = lowert;
  intCode.addInst(new AsignmentQ(ArgType::id, arg1, this->varsym));

  intCode.emitLabel(this->init);

  // DONE QUAD: if loopvar >= upperbound goto next
  arg1.id = this->varsym;
  arg2.id = uppert;
  intCode.addInst(new ConditionalJumpQ(ArgType::id, arg1,
				       Operator::greaterEq,
				       ArgType::id, arg2,
				       next));

  this->block->gen(init);

  if (this->step) {
    // DONE QUAD: loopvar := loopvar + step
    arg1.id = this->varsym;
    arg2.id = stept;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     this->varsym));
  } else {
    // DONE QUAD: loopvar := loopvar + 1
    arg1.id = this->varsym;
    arg2.constint = 1;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::constint, arg2,
				     this->varsym));
  }
  // DONE QUAD: goto init
  intCode.addInst(new JumpQ(init));
}

void While::gen(Label* next) {
  this->init = intCode.newLabel();
  this->exit = next;
  intCode.emitLabel(init);
  this->cond->jumping(NULL, next);
  this->block->gen(init);

  // DONE QUAD: goto init
  intCode.addInst(new JumpQ(init));
}

void ForEach::gen(Label* next) {
  GenLvalue arrayloc = this->array->genlvalue();

  if (arrayloc.doff == NULL) {
    arrayloc.doff = intCode.newTemp();
    // QUAD: doff := 0
    std::cout << (arrayloc.doff)->getId() << " := 0" << std::endl;
  }

  SymVar* counter = intCode.newTemp();

  ArrayType* arrayt = dynamic_cast<ArrayType*>(this->array->getType());
  int length = arrayt->getLength();
  int elemsize = arrayt->getBaseType()->getSize();

  // QUAD: doff := doff + coff
  std::cout << (arrayloc.doff)->getId() << " := "
	    << (arrayloc.doff)->getId() << " + "
	    << arrayloc.coff << std::endl;

  if ( (arrayloc.base)->isReference() ) {
    // QUAD: i := base + doff
    std::cout << this->loopvar->getId() << " := "
	      << (arrayloc.base)->getId() << " + "
	      << (arrayloc.doff)->getId() << std::endl;
    // QUAD: counter := base[4]
    std::cout << counter->getId() << " := "
	      << (arrayloc.base)->getId() << "[4]" << std::endl;
  } else {
    // QUAD: i := &base
    std::cout << this->loopvar->getId() << " := &"
	      << (arrayloc.base)->getId() << std::endl;
    // QUAD: i := i + doff
    std::cout << this->loopvar->getId() << " := "
	      << this->loopvar->getId() << " + "
	      << (arrayloc.doff)->getId() << std::endl;
    // QUAD: counter := <array length>
    std::cout << counter->getId() << " := "
	      << length << std::endl;
  }
  Label* init = intCode.newLabel();
  intCode.emitLabel(init);

  // QUAD: if counter = 0 goto next
  std::cout << "if " << counter->getId() << " = 0 goto l"
	    << next->getId() << std::endl;

  this->block->gen(init);

  // QUAD: i := i + <elemsize>
  std::cout << this->loopvar->getId() << " := "
	    << this->loopvar->getId() << " + "
	    << elemsize << std::endl;
  // QUAD: counter := counter - 1
  std::cout << counter->getId() << " := "
	    << counter->getId() << " - 1" << std::endl;
  // QUAD: goto init
  std::cout << "goto l" << init->getId() << std::endl;
}

void Asignment::gen(Label* next) {
  Args arg1;
  Args arg2;

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

    if ( dynamic_cast<VarExp*>(*itlvals)  ) {
      if ( (lvalue.base)->isReference() ) {
	// DONE QUAD: *base := temp
	arg1.id = *ittemps;
	intCode.addInst(new AsignmentToPointQ(ArgType::id, arg1,
					      lvalue.base));
      } else {
	// DONE QUAD: base := temp
	arg1.id = *ittemps;
	intCode.addInst(new AsignmentQ(ArgType::id, arg1, lvalue.base));
      }
    } else {

      if (lvalue.doff == NULL) {
	lvalue.doff = intCode.newTemp();
	// DONE QUAD: doff := 0
	arg1.constint = 0;
	intCode.addInst(new AsignmentQ(ArgType::constint, arg1, lvalue.doff));
      }

      // DONE QUAD: doff := doff + coff
      arg1.id = lvalue.doff;
      arg2.constint = lvalue.coff;
      intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
					Operator::sumI,
					ArgType::constint, arg2,
					lvalue.doff));

      if ( (lvalue.base)->isReference() ) {
	// DONE QUAD: doff := doff + base
	arg1.id = lvalue.doff;
	arg2.id = lvalue.base;
	intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
					 Operator::sumI,
					 ArgType::id, arg2,
					 lvalue.doff));

	// DONE QUAD: *doff := temp
	arg1.id = *ittemps;
	intCode.addInst(new AsignmentToPointQ(ArgType::id, arg1,
					      lvalue.doff));

      } else {
	// DONE QUAD: base[doff] := temp
	arg1.id = lvalue.doff;
	arg2.id = *ittemps;
	intCode.addInst(new IndexAsigQ(lvalue.base,
				       ArgType::id, arg1,
				       ArgType::id, arg2));
      }
    }

    ittemps++;
    itlvals++;
  }
}

void VariableDec::gen(Label* next) {
  Args arg1;
  Args arg2;

  for (std::list<std::pair<SymVar*,Expression*>>::iterator it =
	 (this->decls).begin(); it != (this->decls).end(); it++) {
    if (!it->second) continue;
    SymVar* addr = ((*it).second)->gen();

    // DONE QUAD: var := temp
    arg1.id = addr;
    intCode.addInst(new AsignmentQ(ArgType::id, arg1,
				   (*it).first));
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
  GenLvalue lvalue = this->lval->genlvalue();

 if (lvalue.doff == NULL) {
    lvalue.doff = intCode.newTemp();
    // QUAD: doff := 0
    std::cout << (lvalue.doff)->getId() << " := 0" << std::endl;
  }

  if ( this->lval->getType() == &(IntType::getInstance()) ) {
    if ( (lvalue.base)->isReference() ) {
      // QUAD: *base := read type
      std::cout << "*" << (lvalue.base)->getId() << " := read <type>"
		<< std::endl;
    } else {
      // QUAD: base := read type
      std::cout << (lvalue.base)->getId() << " := read <type>"
		<< std::endl;
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
      // QUAD: *doff := read type
      std::cout << "*" << (lvalue.doff)->getId() << " := read <type>"
		<< std::endl;

    } else {
      // QUAD: base[doff] := read type
      std::cout << (lvalue.base)->getId() << "["
		<< (lvalue.doff)->getId() << "] := read <type>"
		<< std::endl;
    }
  }
}
