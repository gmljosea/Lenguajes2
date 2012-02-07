#include <iostream>
#include <string>
#include "expression.hh"
#include "statement.hh"

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

void Null::print(int nesting) {
  std::string padding(nesting*2,' ');
  std::cout << padding << "Nop" << std::endl;
}

// Constructor de If
If::If(Expression *cond, Block *block_true, Block *block_false) {
  this->cond = cond;
  this->block_true = block_true;
  this->block_false = block_false;
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

// Iteration
Iteration::Iteration(std::string* label) {
  this->label = label;
}

std::string* Iteration::getLabel() {
  return this->label;
}

// BoundedFor
BoundedFor::BoundedFor(std::string* label, std::string* varsym,
		       Expression* lowerb, Expression* upperb,
		       Expression* step, Block* block) : Iteration(label) {
  this->varsym = varsym;
  this->lowerb = lowerb;
  this->upperb = upperb;
  this->step = step;
  this->block = block;
}

void BoundedFor::print(int nesting) {
  std::string padding(nesting*2,' ');
  std::cout << padding << "For in:" << std::endl;
  if (label != NULL) {
    std::cout << padding << " Etiqueta: " << *label << std::endl;
  }
  std::cout << padding << " Variable: " << *varsym << std::endl;
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

// While
While::While(std::string* label, Expression* cond, Block* block)
  : Iteration(label) {
  this->cond = cond;
  this->block = block;
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

// Asignment
Asignment::Asignment() {
}

void Asignment::push_back_lvalue(Lvalue* lvalue) {
  this->lvalues.push_back(lvalue);
}

void Asignment::push_back_exp(Expression* exp) {
  this->exps.push_back(exp);
}

void Asignment::print(int nesting) {
  std::cout << "Instrucción asignación" << std::endl;
}

// Declaration
VariableDec::VariableDec() {};

void VariableDec::push_back(SymVar* sym, Expression* init) {
  std::pair<SymVar*,Expression*> declaration(sym,init);
  this->decs.push_back(declaration);
}

void VariableDec::print(int nesting) {
  std::cout << "Instrucción declaración" << std::endl;
}

// Block
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

void Block::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Bloque (contexto nº " << scope_number << "):"
	    << std::endl;
  for (std::list<Statement*>::iterator it = stmts.begin();
       it != stmts.end(); it++) {
    (*it)->print(nesting+1);
  }
}


Break::Break(std::string* label) {
  this->label = label;
}

void Break::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Break";
  if (label != NULL) {
    std::cout << " (" << *label << ")";
  }
  std::cout << std::endl;
}

Next::Next(std::string* label) {
  this->label = label;
}

void Next::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Next";
  if (label != NULL) {
    std::cout << " (" << *label << ")";
  }
  std::cout << std::endl;
}

Return::Return(Expression *exp) {
  this->exp = exp;
}

void Return::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Return" << std::endl;
  if (exp != NULL) {
    exp->print(nesting+1);
  }
}

FunctionCall::FunctionCall(Expression* exp) {
  this->exp = exp;
}

void FunctionCall::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Llamada a función" << std::endl;
}
