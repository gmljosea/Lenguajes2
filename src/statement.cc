#include <iostream>
#include <string>
#include "expression.hh"
#include "statement.hh"
#include "program.hh"

extern Program program;

bool Lvalue::isBad() {
  return false;
}

Type* Lvalue::getType() {
  return NULL;
  /* Dummy, nunca debería ser llamada porque teóricamente Lvalue es
  abstracta. Pero cuando fue definida por primera vez no lo era, así
  que capaz ahorita hay alguna variable por ahí que es un Lvalue por valor
  y puede explotar si de repente hacemos Lvalue abstracta.
  Luego se arregla */
}

void Lvalue::print(int n) {
}

NormalLvalue::NormalLvalue(SymVar* var) {
  this->variable = var;
}

Type* NormalLvalue::getType() {
  return this->variable->getType();
}

void NormalLvalue::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << variable->getId() << std::endl;
}

bool BadLvalue::isBad() {
  return true;
}

Type* BadLvalue::getType() {
  return &(ErrorType::getInstance());
}

void BadLvalue::print(int n) {
}

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

Iteration::Iteration(std::string* label) {
  this->label = label;
}

std::string* Iteration::getLabel() {
  return this->label;
}

/******** BoundedFor ********/

BoundedFor::BoundedFor(std::string* label, SymVar* varsym,
		       Expression* lowerb, Expression* upperb,
		       Expression* step, Block* block) : Iteration(label) {
  this->varsym = varsym;
  this->lowerb = lowerb;
  this->upperb = upperb;
  this->step = step;
  this->block = block;
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
  : Iteration(label) {
  this->cond = cond;
  this->block = block;
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

/********* ASIGNMENT ***********/

Asignment::Asignment(std::list<Expression*> lvalues, std::list<Expression*> exps) {
  this->lvalues = lvalues;
  this->exps = exps;
}

void Asignment::check(){
  /*  if( lvalues.size() != exps.size() ) {
    program.error("el numero de variables es diferente al numero de "
		  "expresiones del lado derecho",
		  this->first_line,this->first_column);
    return;
  }

  std::list<Expression*>::iterator itExp = this->exps.begin();
  for(std::list<Lvalue*>::iterator itLval= this->lvalues.begin() ;
      itLval != this->lvalues.end() ; itLval++,itExp++){
    if ((*itLval)->isBad() or (*itExp)->isBad()) continue;
    (*itExp)->check();
    if (!( *((*itLval)->getType()) == (*(*itExp)->getType()) )) {
      program.error("los tipos no coinciden",this->first_line,this->first_column);
    }
    }*/
}

void Asignment::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Asignación" << std::endl;
  /*std::cout << padding << " L-values:" << std::endl;
  for (std::list<Lvalue*>::iterator it = lvalues.begin();
       it != lvalues.end(); it++) {
    (*it)->print(nesting+1);
  }
  std::cout << padding << " Expresiones:" << std::endl;
  for (std::list<Expression*>::iterator it = exps.begin();
       it != exps.end(); it++) {
    (*it)->print(nesting+1);
    }*/
}

/********** DECLARATION ********/

VariableDec::VariableDec(Type* type,
			 std::list<std::pair<SymVar*,Expression*>> decls) {
  this->decls = decls;
  this->type = type;
}

void VariableDec::check(){
  // CUIDADO !! en la lista pueden existir NULLs 
  for(std::list<std::pair<SymVar*,Expression*>>::iterator it=this->decls.begin();
      it!= this->decls.end(); it++){
    // Si es una variable tipo string, chequear si se inicializo
    // !!! Super hack horrible
    StringType tb(1);
    StringType& t = tb;
    if( *((*it).first->getType())== t )
      if((*it).second==NULL){
	program.error("variable de tipo 'string' debe ser inicializada al declarar ",((*it).first)->getLine(),((*it).first)->getColumn() );
	continue;
      }

    /* Chequear que los tipos de las variables coincidan con
       los tipos de las expresiones que le corresponden*/  
    if((*it).second != NULL)
      if(!(*((*it).first->getType()) == *((*it).second->getType()))){
	std::string strError= "el tipo de la variable '"+((*it).first)->getId();
	strError += "' no concuerda con el de la expresion asignada"; 
	program.error(strError,((*it).first)->getLine(),((*it).first)->getColumn());
      }

    }
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

Break::Break(std::string* label) {
  this->label = label;
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

Next::Next(std::string* label) {
  this->label = label;
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
