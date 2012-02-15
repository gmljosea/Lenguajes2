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

NormalLvalue::NormalLvalue(SymVar* var) {
  this->variable = var;
}

Type* NormalLvalue::getType() {
  return NULL;
  // !!! return this->variable->getType();
}

bool BadLvalue::isBad() {
  return true;
}

Type* BadLvalue::getType() {
  return new VoidType();
  /* Si que horrible. Es un memory leak y aparte devuelve tipo void, lo cual
     tiene poco sentido, pero no se me ocurre algo mejor esta noche.
   */
}

/**** Statement ****/

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
  BoolType i;
  if(!(*(this->cond->getType()) == i ))
    program.error("condicion debe ser de tipo 'bool'",this->first_line,this->first_column);
  this->block_true->check();
  this->block_false->check();
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
  IntType i;
  if(!(*(this->lowerb->getType())==i))
    program.error("limite inferior debe ser de tipo 'int'",this->first_line,this->first_column);

  if(!(*(this->upperb->getType())==i))
    program.error("limite inferior debe ser de tipo 'int'",this->first_line,this->first_column);
    
  if(this->step!=NULL){
    if(!(*(this->upperb->getType())==i))
      program.error("limite inferior debe ser de tipo 'int'",this->first_line,this->first_column);
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
  BoolType t;
  if(!(*(this->cond->getType())==t))
    program.error("la condición del While debe ser de tipo 'int'",this->first_line,this->first_column);
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

Asignment::Asignment(std::list<Lvalue*> lvalues, std::list<Expression*> exps) {
  this->lvalues = lvalues;
  this->exps = exps;
}

void Asignment::check(){
  /*  lvalues 
  exps
tamaño 
tipos cuadren por pares
readonly
isBad ignorar*/

  if(this->lvalues.size()!=this->exps.size()){
    program.error("el numero de variables es diferente al numero de expresiones del lado derecho",this->first_line,this->first_column);
  }else{
    std::list<Expression*>::iterator itExp = this->exps.begin();

    for(std::list<Lvalue*>::iterator itLval= this->lvalues.begin() ;
	itLval != this->lvalues.end() ; itLval++,itExp++){

      //Falta hacer el chequeo de readonly
      
      if ((*itLval)->isBad() or (*itExp)->isBad()) continue;
	
      if (!( *((*itLval)->getType()) == (*(*itExp)->getType()) ))
	program.error("los tipos no coinciden",this->first_line,this->first_column);
    }
  }

}

void Asignment::print(int nesting) {
  std::cout << "Instrucción asignación" << std::endl;
}

/********** DECLARATION ********/

VariableDec::VariableDec(Type type,
			 std::list<std::pair<SymVar*,Expression*>> decls) {
  this->decls = decls;
  this->type = type;
}

void VariableDec::check(){}

void VariableDec::print(int nesting) {
  std::cout << "Instrucción declaración" << std::endl;
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

void Return::check(){}

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

void FunctionCall::check(){}

void FunctionCall::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Llamada a función" << std::endl;
}

/************ WRITE ************/

Write::Write(std::list<Expression*> exps, bool isLn) {
  this->exps = exps;
  this->isLn = isLn;
}

void Write::check(){}

void Write::print(int nesting) {
  std::cout << "Write" << std::endl;
}

/********** READ ************/

Read::Read(Lvalue* lval) {
  this->lval = lval;
}

void Read::check(){}

void Read::print(int nesting) {
  std::cout << "Read" << std::endl;
}
