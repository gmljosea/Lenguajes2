#include <iostream>
#include <string>
#include "expression.hh"
#include "statement.hh"

class Lvalue {};

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
  std::cout << "Instrucción For in" << std::endl;
}

// While
While::While(std::string* label, Expression* cond, Block* block)
  : Iteration(label) {
  this->cond = cond;
  this->block = block;
}

void While::print(int nesting) {
  std::cout << "Instrucción while" << std::endl;
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

void VariableDec::push_back(Asignment* asg) {
  this->asigns.push_back(asg);
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
}

Next::Next(std::string* label) {
  this->label = label;
}

void Next::print(int nesting) {
}

Return::Return(Expression *exp) {
  this->exp = exp;
}

void Return::print(int nesting) {
}
