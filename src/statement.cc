#include <iostream>
#include "expression.hh"
#include "statement.hh"

class Lvalue {};

Statement::Statement() {
  this->enclosing = NULL;
}

void Null::print(int nesting) {
  std::cout << "Instrucción nula" << std::endl;
}

// Constructor de If
If::If(Expression *cond, Block *block_true, Block *block_false) {
  this->cond = cond;
  this->block_true = block_true;
  this->block_false = block_false;
}

void If::print(int nesting) {
  std::cout << "Instrucción If" << std::endl;
  block_true->print(nesting+1);
  if (block_false != NULL) {
    block_false->print(nesting+1);
  }
}

// BoundedFor
BoundedFor::BoundedFor(std::string* varsym, Expression* lowerb,
		       Expression* upperb, Expression* step,
		       Block* block) {
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
While::While(Expression* cond, Block* block) {
  this->cond = cond;
  this->block = block;
}

void While::print(int nesting) {
  std::cout << "Instrucción while" << std::endl;
}

// Asignment
Asignment::Asignment(Lvalue* lvalue, Expression* exp) {
  this->push_back(lvalue, exp);
}

void Asignment::push_back(Lvalue* lvalue, Expression* exp) {
  this->lvalue.push_back(lvalue);
  this->exp.push_back(exp);
}

void Asignment::print(int nesting) {
  std::cout << "Instrucción asignación" << std::endl;
}

// Declaration
Declaration::Declaration() {};

void Declaration::push_back(Asignment* asg) {
  this->asigns.push_back(asg);
}

void Declaration::print(int nesting) {
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
  std::cout << "Bloque (contexto nº " << scope_number << "):" << std::endl;
  for (std::list<Statement*>::iterator it = stmts.begin();
       it != stmts.end(); it++) {
    (*it)->print(nesting+1);
  }
}
