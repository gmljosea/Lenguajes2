#include <iostream>
#include <string>
#include "expression.hh"
#include "statement.hh"
#include "program.hh"

extern Program program;

Statement::Statement() {
  this->enclosing = NULL;
}

void Statement::setEnclosing(Statement* stmt) {
  this->enclosing = stmt;
}

void Statement::setLocation(int first_line, int first_column, int last_line,
			    int last_column) {
  this->first_line = first_line;
  this->first_column = first_column;
  this->last_line = last_line;
  this->last_column = last_column;
}

int Statement::getFirstLine() {
  return this->first_line;
}

int Statement::getFirstCol() {
  return this->first_column;
}

/***** Block *******/

Block::Block(int scope_number, Statement *stmt) {
  this->scope_number = scope_number;
  this->push_back(stmt);
}

void Block::push_back(Statement *stmt) {
  this->stmts.push_back(stmt);
}

void Block::push_back(std::list<Statement*> stmts) {
  this->stmts.splice(this->stmts.end(), stmts);
}

void Block::check(){
  for(std::list<Statement*>::iterator it = this->stmts.begin();
      it != this->stmts.end(); it++){
    (*it)->check();
  }
}

void Block::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Bloque (contexto nº " << scope_number << "):"
	    << std::endl;
  for (std::list<Statement*>::iterator it = stmts.begin();
       it != stmts.end(); it++) {
    (*it)->print(nesting+1);
  }
}

/****** NULL *****/

void Null::check(){}

void Null::print(int nesting) {
  std::string padding(nesting*2,' ');
  std::cout << padding << "Nop" << std::endl;
}

/******** If ******/

If::If(Expression *cond, Block *block_true, Block *block_false) {
  this->cond = cond;
  this->block_true = block_true;
  this->block_false = block_false;
}

void If::check(){
  this->cond->check();
  this->cond = this->cond->cfold();
  Type* t = this->cond->getType();
  if (*t != BoolType::getInstance() and *t != ErrorType::getInstance()) {
    program.error("condicion debe ser de tipo 'bool' pero se encontró '"+
		  t->toString()+"'", this->cond->getFirstLine(),
		  this->cond->getFirstCol());
  }
  this->block_true->check();
  if (this->block_false != NULL) this->block_false->check();
}

void If::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "If" << std::endl;
  std::cout << padding << " Condición:" << std::endl;
  cond->print(nesting+1);
  std::cout << padding << " Caso verdadero:" << std::endl;
  block_true->print(nesting+1);
  if (block_false != NULL) {
    std::cout << padding << " Caso falso:" << std::endl;
    block_false->print(nesting+1);
  }
}

/******* Iteration ********/

Iteration::Iteration(std::string* label, Block* block) {
  this->label = label;
  this->block = block;
}

void Iteration::setBlock(Block* block) {
  this->block = block;
}

std::string* Iteration::getLabel() {
  return this->label;
}

/******** BoundedFor ********/

BoundedFor::BoundedFor(std::string* label, SymVar* varsym,
		       Expression* lowerb, Expression* upperb,
		       Expression* step, Block* block)
  : Iteration(label,block) {
  this->varsym = varsym;
  this->lowerb = lowerb;
  this->upperb = upperb;
  this->step = step;
}

void BoundedFor::check(){
  // Chequear límite inferior
  this->lowerb->check();
  this->lowerb = this->lowerb->cfold();
  Type* tlowb = this->lowerb->getType();
  if (*tlowb != IntType::getInstance() and *tlowb != ErrorType::getInstance()) {
    program.error("limite inferior debe ser de tipo 'int' pero se encontró '"
		  +tlowb->toString()+"'", this->lowerb->getFirstLine()
		  , this->lowerb->getFirstCol());
  }
  // Chequear límite superior
  this->upperb->check();
  this->upperb = this->upperb->cfold();
  Type* tuppb = this->upperb->getType();
  if (*tuppb != IntType::getInstance() and *tuppb != ErrorType::getInstance()) {
    program.error("limite superior debe ser de tipo 'int' pero se encontró '"
		  +tuppb->toString()+"'", this->upperb->getFirstLine()
		  , this->upperb->getFirstCol());
  }

  // Chequear paso, si existe
  if (this->step!=NULL) {
    this->step->check();
    this->step = this->step->cfold();
    Type* tstep = this->step->getType();
    if (*tstep != IntType::getInstance() and *tstep != ErrorType::getInstance()) {
      program.error("el paso debe ser de tipo 'int' pero se encontró '"
		    +tstep->toString()+"'", this->step->getFirstLine()
		    , this->step->getFirstCol());
    }
  }

  this->block->check();
}

void BoundedFor::print(int nesting) {
  std::string padding(nesting*2,' ');
  std::cout << padding << "For in:" << std::endl;
  if (label != NULL) {
    std::cout << padding << " Etiqueta: " << *label << std::endl;
  }
  //  std::cout << padding << " Variable: " << *varsym << std::endl;
  std::cout << padding << " Cota inferior:" << std::endl;
  lowerb->print(nesting+1);
  std::cout << padding << " Cota superior:" << std::endl;
  upperb->print(nesting+1);
  if (step != NULL) {
    std::cout << padding << " Paso:" << std::endl;
    step->print(nesting+1);
  }
  std::cout << padding << " Cuerpo:" << std::endl;
  block->print(nesting+1);
}

/********** WHILE *********/
While::While(std::string* label, Expression* cond, Block* block)
  : Iteration(label,block) {
  this->cond = cond;
}

void While::check(){
  this->cond->check();
  this->cond = this->cond->cfold();
  Type* t = this->cond->getType();
  if(*t != BoolType::getInstance() and *t != ErrorType::getInstance()) {
    program.error("la condición debe ser de tipo 'bool' pero se encontró '"
		  +t->toString()+"'", this->cond->getFirstLine(),
		  this->cond->getFirstCol());
  }
  this->block->check();
}

void While::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "While:" << std::endl;
  if (label != NULL) {
    std::cout << padding << " Etiqueta: " << *label << std::endl;
  }
  std::cout << padding << " Condición:" << std::endl;
  cond->print(nesting+1);
  std::cout << padding << " Cuerpo:" << std::endl;
  block->print(nesting+1);
}

/*** ForEach **/
void ForEach::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "For:" << std::endl;
  if (label != NULL) {
    std::cout << padding << " Etiqueta:" << *label << std::endl;
  }
  std::cout << padding << " Arreglo: " << std::endl;
  this->array->print(nesting+1);
  std::cout << padding << " Cuerpo:" << std::endl;
  block->print(nesting+1);
}

void ForEach::check() {
  array->check();
  ArrayType* t = dynamic_cast<ArrayType*>(array->getType());
  if (!t) {
    program.error("expresión no es de tipo 'array', se encontró '"+
		  array->getType()->toString()+"'", array->getFirstLine(),
		  array->getFirstCol());
    loopvar->setType(&(ErrorType::getInstance()));
  } else {
    loopvar->setType(t->getBaseType());
  }
  block->check();
}

/********* ASIGNMENT ***********/

Asignment::Asignment(std::list<Expression*> lvalues, std::list<Expression*> exps) {
  this->lvalues = lvalues;
  this->exps = exps;
}

void Asignment::check(){
  if( lvalues.size() != exps.size() ) {
    program.error("el numero de variables es diferente al numero de "
		  "expresiones del lado derecho",
		  this->first_line,this->first_column);
    return;
  }

  std::list<Expression*>::iterator itExp = this->exps.begin();
  for(std::list<Expression*>::iterator itLval = this->lvalues.begin();
      itLval != this->lvalues.end() ; itLval++, itExp++ ) {

    // Chequear y reducir lvalues y expresiones
    (*itLval)->check();
    (*itExp)->check();
    (*itLval) = (*itLval)->cfold();
    (*itExp) = (*itExp)->cfold();

    if (!(*itLval)->isLvalue()) {
      program.error("no es una expresión asignable", (*itLval)->getFirstLine(),
		    (*itLval)->getFirstCol());
      continue;
    }

    Type* tlval = (*itLval)->getType();
    Type* texp = (*itExp)->getType();

    // Si algun lado tiene error, seguir silenciosamente
    if (*tlval == ErrorType::getInstance() or
	*texp == ErrorType::getInstance()) {
      continue;
    }

    // Se prohibe asignar strings, arrays y boxes
    if (dynamic_cast<StringType*>(tlval) or
	dynamic_cast<ArrayType*>(tlval) or
	dynamic_cast<BoxType*>(tlval)) {
      program.error("no se puede asignar a variables de tipo '"+
		    tlval->toString()+"'", (*itLval)->getFirstLine(),
		    (*itExp)->getFirstCol());
      continue;
    }

    // Clásico error de asignar an un tipo uan expresión de otro tipo
    if (*tlval != *texp) {
      program.error("no coinciden los tipos en la asignación, se esperaba '"+
		    tlval->toString()+"' y se encontró '"+
		    texp->toString()+"' en "+
		    std::to_string((*itExp)->getFirstLine())+":"+
		    std::to_string((*itExp)->getFirstCol()),
		    (*itLval)->getFirstLine(), (*itLval)->getFirstCol());
    }
  }
}

void Asignment::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Asignación" << std::endl;
  std::cout << padding << " L-values:" << std::endl;
  for (std::list<Expression*>::iterator it = lvalues.begin();
       it != lvalues.end(); it++) {
    (*it)->print(nesting+1);
  }
  std::cout << padding << " Expresiones:" << std::endl;
  for (std::list<Expression*>::iterator it = exps.begin();
       it != exps.end(); it++) {
    (*it)->print(nesting+1);
    }
}

/********** DECLARATION ********/

VariableDec::VariableDec(Type* type,
			 std::list<std::pair<SymVar*,Expression*>> decls) {
  this->decls = decls;
  this->type = type;
}

void VariableDec::check(){
  // bueno, esto se viene en grande
  // super chequeo ultra ++

  if (*(this->type) == VoidType::getInstance()) {
    program.error("no se pueden declarar variables tipo 'void'",
		  this->first_line, this->first_column);
    return;
  }

  StringType* strt = dynamic_cast<StringType*>(this->type);
  BoxType* boxt = dynamic_cast<BoxType*>(this->type);
  ArrayType* arrt = dynamic_cast<ArrayType*>(this->type);

  // Si es box y está incompleto, error
  if (boxt and boxt->isIncomplete()) {
    program.error("tipo '"+boxt->toString()+"' desconocido",
		  this->first_line, this->first_column);
    return;
  }

  // CUIDADO !! en la lista pueden existir NULLs
  for(std::list<std::pair<SymVar*,Expression*>>::iterator it = this->decls.begin();
      it!= this->decls.end(); it++){
    // Si es una variable tipo string, chequear si se inicializo
    // !!! Super hack horrible

    // Si es string y no se inicializó, dar error
    if (strt and !it->second) {
      program.error("variable de tipo 'string' debe ser inicializada al declarar",
		    it->first->getLine(), it->first->getColumn());
      continue;
    }

    // Si es box o array y se inicializó, con lo que sea, error
    if ((boxt or arrt) and it->second) {
      program.error("no se puede asignar a variables de tipo '"+
		    this->type->toString()+"'", it->first->getLine(),
		    it->first->getColumn());
      continue;
    }

    /* Chequear que los tipos de las variables coincidan con
       los tipos de las expresiones que le corresponden*/
    if (it->second) {
      it->second->check();
      it->second = it->second->cfold();
      if(*(this->type) != *(it->second->getType())) {
	std::string strError = "el tipo de la variable '"+((*it).first)->getId()
	  +"' no concuerda con el de la expresion asignada, se esperaba '"+
	  this->type->toString()+"' y se encontró '"+
	  it->second->getType()->toString()+"'";
	program.error(strError,it->first->getLine(), it->first->getColumn());
      }
      if (this->isGlobal and !it->second->isConstant()) {
	program.error("expresión no constante en inicialización de variable "
		      "global", it->second->getFirstLine(),
		      it->second->getFirstCol());
      }
    }
  }
}

void VariableDec::setGlobal(bool global) {
  this->isGlobal = global;
}

void VariableDec::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Declaración de variables" << std::endl;
  std::cout << padding << " Tipo: ";
  type->print();
  std::cout << std::endl;
  for (std::list<std::pair<SymVar*,Expression*>>::iterator it = decls.begin();
       it != decls.end(); it++) {
    std::cout << padding << " " << (*it).first->getId();
    if ((*it).second != NULL) {
      std::cout << "=" << std::endl;
      (*it).second->print(nesting+1);
    } else {
      std::cout << std::endl;
    }
  }
}

/*********** BREAK ************/

Break::Break(std::string* label, Iteration* loop) {
  this->label = label;
  this->loop = loop;
}

void Break::check(){}

void Break::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Break";
  if (label != NULL) {
    std::cout << " (" << *label << ")";
  }
  std::cout << std::endl;
}

/********** NEXT ************/

Next::Next(std::string* label, Iteration* loop) {
  this->label = label;
  this->loop = loop;
}

void Next::check(){}

void Next::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Next";
  if (label != NULL) {
    std::cout << " (" << *label << ")";
  }
  std::cout << std::endl;
}

/********** RETURN **********/

Return::Return(SymFunction* symf, Expression *exp) {
  this->symf = symf;
  this->exp = exp;
}

void Return::check(){
  if(this->exp!= NULL){
    if(!(*(this->exp->getType())== *(this->symf->getType())))
      program.error("return devuelve tipo incompatible",this->first_line,this->first_column);
  }else{
    VoidType& t = VoidType::getInstance();
    if(!(*(this->symf->getType()) == t))
      program.error("return esperaba 'void'", this->first_line, this->first_column);} 
}

void Return::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Return" << std::endl;
  if (exp != NULL) {
    exp->print(nesting+1);
  }
}

/******** FUNCTION CALL ********/

FunctionCall::FunctionCall(Expression* exp) {
  this->exp = exp;
}

void FunctionCall::check() {
  FunCallExp* fce = dynamic_cast<FunCallExp*>(exp);
  fce->check();
}

void FunctionCall::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Llamada a función" << std::endl;
  exp->print(nesting+1);
}

/************ WRITE ************/

Write::Write(std::list<Expression*> exps, bool isLn) {
  this->exps = exps;
  this->isLn = isLn;
}

void Write::check(){
  for (std::list<Expression*>::iterator it = exps.begin();
       it != exps.end(); it++) {
    (*it)->check();
    *it = (*it)->cfold();
  }
}

void Write::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding;
  if (isLn) {
    std::cout << "Writeln" << std::endl;
  } else {
    std::cout << "Write" << std::endl;
  }
  for (std::list<Expression*>::iterator it = exps.begin();
       it != exps.end(); it++) {
    (*it)->print(nesting+1);
  }
}

/********** READ ************/

Read::Read(Expression* lval) {
  this->lval = lval;
}

void Read::check(){}

void Read::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Read" << std::endl;
  lval->print(nesting+1);
}
