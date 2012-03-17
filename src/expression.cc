#include <iostream>
#include <string>
#include "expression.hh"
#include "program.hh"

extern Program program;

// Expression
void Expression::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "Expresión" << std::endl;
}

void Expression::setLocation(int fline, int fcol, int lline, int lcol) {
  this->fline = fline;
  this->fcol = fcol;
  this->lline = lline;
  this->lcol = lcol;
}

int Expression::getFirstLine() {
  return fline;
}

int Expression::getFirstCol() {
  return fcol;
}

void Expression::check() {
  // Cada check chequea recursivamente las subexpresiones, si hay
  // y setea el campo type de manera correspondiente
  // Si no se hace check antes de getType, el tipo podría quedar nulo
}

Type* Expression::getType() { return this->type; }
bool Expression::isBad() { return false; }
Expression* Expression::cfold() { return this; }
bool Expression::isConstant() { return false; }
int Expression::getInteger() { return 0; }
double Expression::getFloat() { return 0.0; }
bool Expression::getBool() { return true; }


// BadExp
BadExp::BadExp() {
  this->type = &(ErrorType::getInstance());
}

bool BadExp::isBad() {
  return true;
}


// VarExp
VarExp::VarExp(SymVar* symv) {
  this->symv = symv;
  this->type = symv->getType();
}

void VarExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << symv->getId() << std::endl;
}


// Constant
bool Constant::isConstant() { return true; }


// IntExp
IntExp::IntExp(int value) {
  this->value = value;
  this->type = &(IntType::getInstance());
}

void IntExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

int IntExp::getInteger() {
  return this->value;
}


// FloatExp
FloatExp::FloatExp(float value) {
  this->value = value;
  this->type = &(FloatType::getInstance());
}

void FloatExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << value << std::endl;
}

double FloatExp::getFloat() {
  return this->value;
}


// BoolExp
BoolExp::BoolExp(bool value) {
  this->value = value;
  this->type = &(BoolType::getInstance());
}

void BoolExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  if (value) {
    std::cout << padding << "true" << std::endl;
  } else {
    std::cout << padding << "false" << std::endl;
  }
}

bool BoolExp::getBool() {
  return this->value;
}


// StringExp
StringExp::StringExp(std::string str) {
  this->str = str;
  this->type = new StringType(str.length());
}

void StringExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "\"" << this->str << "\"" << std::endl;
}


// CharExp
CharExp::CharExp(std::string ch) {
  this->ch = ch;
  this->type = &(CharType::getInstance());
}

void CharExp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "'" << this->ch << "'" << std::endl;
}


// BinaryOp
void BinaryOp::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->exp1->print(nesting+1);
  std::cout << padding << this->op << std::endl;
  this->exp2->print(nesting+1);
}

// Arithmetic
void Arithmetic::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  // Caso tipos correctos
  if (*t1 == *t2 and
      (*t1 == IntType::getInstance() or
       *t1 == FloatType::getInstance())) {
    this->type = t1;
    return;
  }
  // Subexpresión errónea, propagar el error silenciosamente
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// Sum
Expression* Sum::cfold() {
  /*this->exp1 = this->exp1->reduce();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->reduce();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()+exp2->getInteger());
  } else {
    result = new FloatExp(exp1->getFloat()+exp2->getFloat());
  }
  //result->setLocation(exp1->fline,exp1->fcol,exp2->lline,exp2->lcol);
  delete exp1;
  delete exp2;
  delete this;
  return result;*/
}

// Remainder
void Remainder::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  // Caso tipos correctos
  if (*t1 == *t2 and *t1 == IntType::getInstance()) {
    this->type = t1;
    return;
  }
  // Subexpresión errónea, propagar el error silenciosamente
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// Minus
void Minus::check() {
  this->exp1->check();
  Type* t = this->exp1->getType();
  if (*t == IntType::getInstance() or *t == FloatType::getInstance() or
      *t == ErrorType::getInstance()) {
    this->type = t;
    return;
  }
  program.error("No se puede aplicar operador '-' al tipo '"+t->toString(),
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

void Minus::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "-" << std::endl;
  this->exp1->print(nesting+1);
}

// Logical
void Logical::check() {
  this->exp1->check();
  this->exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  if (*t1 == *t2 && *t1 == BoolType::getInstance()) {
    this->type = t1;
    return;
  }
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// Not
void Not::check() {
  this->exp1->check();
  Type* t = this->exp1->getType();
  if (*t == BoolType::getInstance() or *t == ErrorType::getInstance()) {
    this->type = t;
    return;
  }
  program.error("No se puede aplicar operador 'not' al tipo '"+t->toString(),
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

void Not::print(int nesting) {
  std::string padding(nesting*2, ' ');
  std::cout << padding << "not" << std::endl;
  this->exp1->print(nesting+1);
}

// Relational
void Relational::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  // Caso tipos correctos
  if (*t1 == *t2 and
      (*t1 == IntType::getInstance() or
       *t1 == FloatType::getInstance())) {
    this->type = &(BoolType::getInstance());
    return;
  }
  // Subexpresión errónea, propagar el error silenciosamente
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// Equal
// Sobrescribo check() de Relational porque Equal permite comparar booleanos
void Equal::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  // Caso tipos correctos
  if (*t1 == *t2 and
      (*t1 == IntType::getInstance() or
       *t1 == FloatType::getInstance() or
       *t1 == BoolType::getInstance())) {
    this->type = &(BoolType::getInstance());
    return;
  }
  // Subexpresión errónea, propagar el error silenciosamente
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// NotEqual
// misma razón que Equal para sobrescribir check()
void NotEqual::check() {
  exp1->check();
  exp2->check();
  Type* t1 = this->exp1->getType();
  Type* t2 = this->exp2->getType();
  // Caso tipos correctos
  if (*t1 == *t2 and
      (*t1 == IntType::getInstance() or
       *t1 == FloatType::getInstance() or
       *t1 == BoolType::getInstance())) {
    this->type = &(BoolType::getInstance());
    return;
  }
  // Subexpresión errónea, propagar el error silenciosamente
  if (*t1 == ErrorType::getInstance() or *t2 == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '"+op+"' entre los tipos '"
		+t1->toString()+"' y '"+t2->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

// Index (operador [], acceso a arreglo)
void Index::check() {
  this->array->check();
  this->index->check();
  Type* tarr = this->array->getType();
  Type* tind = this->index->getType();
  ArrayType* cast_tarr = dynamic_cast<ArrayType*>(tarr);
  // Propagar error si existe
  if (*tarr == ErrorType::getInstance() or
      *tind == ErrorType::getInstance()) {
    this->type = &(ErrorType::getInstance());
    return;
  }
  // Tipos correctos
  if (cast_tarr and *tind == IntType::getInstance()) {
    this->type = cast_tarr->getBaseType();
    // Si el índice es constante, de una vez ver si es válido
    if (this->index->isConstant()) {
      int value = this->index->getInteger();
      if (value >= cast_tarr->getLength()) {
	program.error("El índice excede el tamaño del arreglo",
		      this->fline, this->fcol);
      }
    }
    return;
  }
  // Subexpresiones correctas, pero tipos no cuadran
  program.error("No se puede aplicar operador '[]' entre los tipos '"
		+tarr->toString()+"' y '"+tind->toString()+"'",
		this->fline, this->fcol);
  this->type = &(ErrorType::getInstance());
}

void Index::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->array->print(nesting+1);
  std::cout << padding << "[" << std::endl;
  this->index->print(nesting+1);
  std::cout << padding << "]" << std::endl;
}

// Dot (operador ., acceso a un campo de un box)
void Dot::check() {
  this->box->check();
  Type* t = this->box->getType();
  if (*t == ErrorType::getInstance()) {
    this->type = t;
    return;
  }
  BoxType* bt = dynamic_cast<BoxType*>(t);
  if (!bt) {
    program.error("No se puede aplicar operador '.' a '"+t->toString()+"'",
		  this->fline, this->fcol);
    return;
  }
  BoxField* field = bt->getField(this->field);
  if (!field) {
    program.error("No existe el campo '"+this->field+"' en '"+t->toString()+"'",
		  this->fline, this->fcol);
    return;
  }
}

void Dot::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->box->print(nesting+1);
  std::cout << padding << "." << std::endl
	    << padding << "  " << field << std::endl;
}

// FunCall
FunCallExp::FunCallExp(SymFunction* symf, std::list<Expression*> args) {
  this->symf = symf;
  this->args = args;
  this->checkedFunction = true;
}

FunCallExp::FunCallExp(std::string name, std::list<Expression*> args) {
  this->symf = NULL;
  this->args = args;
  this->checkedFunction = false;
  this->name = name;
}

// Esta vaina hay que hacerla bien
Type* FunCallExp::getType() {
  if (!this->checkedFunction) {
    // Buscar en la tabla de símbolos esta función
    // Si no existe, devuelve VoidType
  }
  // !!! return this->symf->getType();
  return &(VoidType::getInstance());
}

void FunCallExp::check() {
  if (checkedFunction) return;
  SymFunction* symf = program.symtable.lookup_function(name);
  if (symf == NULL) {
    program.error("llamada a función no declarada '"+name+"'", 0, 0);
  } else {
    checkedFunction = true;
  }
}
