#include "expression.hh"
#include "statement.hh"

Statement::Statement() {
  this->enclosing = NULL;
}

void Null::print(int nesting) {
  //bla
}

// Constructor de If
If::If(Expression *cond, Block *block_true, Block *block_false) {
  this->cond = cond;
  this->block_true = block_true;
  this->block_false = block_false;
}

void If::print(int nesting) {
  //bla
}

Block::Block(int scope_number, Statement *stmt) {
  this->scope_number = scope_number;
  this->push_back(stmt);
}

void Block::push_back(Statement *stmt) {
  this->stmts.push_back(stmt);
}

void Block::print(int nesting) {
  //bla
}
