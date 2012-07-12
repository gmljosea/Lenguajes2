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

  //  intCode.emitLabel(this->init);
  Label* cond = intCode.newLabel();
  intCode.emitLabel(cond);

  // DONE QUAD: if loopvar >= upperbound goto next
  arg1.id = this->varsym;
  arg2.id = uppert;
  intCode.addInst(new ConditionalJumpQ(ArgType::id, arg1,
				       Operator::greaterEq,
				       ArgType::id, arg2,
				       next));

  this->block->gen(this->init);

  // El 'init' de este loop lo genero no al principio donde se evalúa
  // la condición, sino al final donde se incremente el contador
  intCode.emitLabel(this->init);

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
  // DONE QUAD: goto cond
  intCode.addInst(new JumpQ(cond));
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
  Args arg1;
  Args arg2;

  this->init = intCode.newLabel();
  this->exit = next;

  GenLvalue arrayloc = this->array->genlvalue();

  if (arrayloc.doff == NULL) {
    arrayloc.doff = intCode.newTemp();
    arrayloc.doff->setType(&(IntType::getInstance()));
    // DONE QUAD: doff := 0
    arg1.constint = 0;
    intCode.addInst(new AsignmentQ(ArgType::constint, arg1, arrayloc.doff));
  }

  SymVar* counter = intCode.newTemp();
  counter->setType(&(IntType::getInstance()));

  ArrayType* arrayt = dynamic_cast<ArrayType*>(this->array->getType());
  int length = arrayt->getLength();
  int elemsize = arrayt->getBaseType()->getSize();

  // DONE QUAD: doff := doff + coff
  arg1.id = arrayloc.doff;
  arg2.constint = arrayloc.coff;
  intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				   Operator::sumI,
				   ArgType::constint, arg2,
				   arrayloc.doff));

  if ( (arrayloc.base)->isReference() ) {
    // DONE QUAD: i := base + doff
    arg1.id = arrayloc.base;
    arg2.id = arrayloc.doff;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     this->loopvar));

    // DONE QUAD: counter := base[4]
    arg1.constint = 4;
    intCode.addInst(new IndexQ(arrayloc.base, ArgType::constint, arg1,
			       counter));
  } else {
    // DONE QUAD: i := &base
    intCode.addInst(new AsignmentAddQ(arrayloc.base, this->loopvar));

    // DONE QUAD: i := i + doff
    arg1.id = this->loopvar;
    arg2.id = arrayloc.doff;
    intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				     Operator::sumI,
				     ArgType::id, arg2,
				     this->loopvar));

    // DONE QUAD: counter := <array length>
    arg1.constint = length;
    intCode.addInst(new AsignmentQ(ArgType::constint, arg1, counter));
  }

  Label* cond = intCode.newLabel();
  intCode.emitLabel(cond);

  // DONE QUAD: if counter = 0 goto next
  arg1.id = counter;
  arg2.constint = 0;
  intCode.addInst(new ConditionalJumpQ(ArgType::id, arg1,
				       Operator::equal,
				       ArgType::constint, arg2,
				       next));

  this->block->gen(this->init);

  intCode.emitLabel(this->init);

  // DONE QUAD: i := i + <elemsize>
  arg1.id = this->loopvar;
  arg2.constint = elemsize;
  intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				   Operator::sumI,
				   ArgType::constint, arg2,
				   this->loopvar));

  // DONE QUAD: counter := counter - 1
  arg1.id = counter;
  arg2.constint = 1;
  intCode.addInst(new AsignmentOpQ(ArgType::id, arg1,
				   Operator::substractionI,
				   ArgType::constint, arg2,
				   counter));

  // DONE QUAD: goto cond
  intCode.addInst(new JumpQ(cond));
  //  std::cout << "goto l" << init->getId() << std::endl;
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
	lvalue.doff->setType(&(IntType::getInstance()));
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
  // DONE QUAD: goto next
  intCode.addInst(new JumpQ(this->loop->exit));
}

void Next::gen(Label* next) {
  // DONE QUAD: goto init
  intCode.addInst(new JumpQ(this->loop->init));
}

void Return::gen(Label* next) {
  Args arg1;
  if (this->exp) {
    SymVar* addr = this->exp->gen();
    // DONE QUAD: return temp
    arg1.id = addr;
    intCode.addInst(new ReturnQ(ArgType::id, arg1, this->symf));
  } else{
    // DONE QUAD: return
    intCode.addInst(new ReturnQ(this->symf));
  }
}

void FunctionCall::gen(Label* next) {
  // FIXME: cuando la función es void, igual se genera un retrieve al temporal
  // generado, que igual no se usa.
  this->exp->gen();
}

void Write::gen(Label* next) {
  Args arg1;
  for (std::list<Expression*>::iterator it = (this->exps).begin();
       it != (this->exps).end(); it++) {
    SymVar* addr = (*it)->gen();

    // DONE QUAD: write/ln type temp
    arg1.id = addr;
    intCode.addInst(new WriteQ(ArgType::id, arg1,
			       (*it)->getType(), this->isLn));
  }
}

void Read::gen(Label* next) {
  Args arg1;
  Args arg2;
  GenLvalue lvalue = this->lval->genlvalue();

  if (lvalue.doff == NULL) {
    lvalue.doff = intCode.newTemp();
    lvalue.doff->setType(&(IntType::getInstance()));
    // DONE QUAD: doff := 0
    arg1.constint = 0;
    intCode.addInst(new AsignmentQ(ArgType::constint, arg1, lvalue.doff));
  }

  if ( dynamic_cast<VarExp*>(this->lval)  ) {
    if ( (lvalue.base)->isReference() ) {
      // DONE QUAD: *base := read type
      intCode.addInst(new ReadQ(lvalue.base, this->lval->getType(), true));
    } else {
      // DONE QUAD: base := read type
      intCode.addInst(new ReadQ(lvalue.base, this->lval->getType(), false));
    }
  } else {
    // DONE QUAD: doff := doff + coff
    arg1.id = lvalue.doff;
    arg1.constint = lvalue.coff;
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

      // DONE QUAD: *doff := read type
      intCode.addInst(new ReadQ(lvalue.doff, this->lval->getType(), true));

    } else {
      // DONE QUAD: base[doff] := read type
      arg1.id = lvalue.doff;
      intCode.addInst(new ReadIndexQ(lvalue.base,
				     ArgType::id, arg1,
				     this->lval->getType()));
    }
  }
}
