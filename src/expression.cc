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
bool Expression::isLvalue() { return false; }
int Expression::getLvalue() { return 0; }
bool Expression::isAssignable() { return false; }

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
  std::cout << padding << this->symv->getType()->toString() 
	    << this->symv->getId() << " (" << this->symv->getLine() 
	    << ":" << this->symv->getColumn()
	    << ") [Bloque: " << this->symv->getnumScope() << "] ["
	    << "Offset: " << this->symv->getOffset() << "]" << std::endl;
}

bool VarExp::isLvalue() { return true; }

bool VarExp::isAssignable() {
  return !this->symv->isReadonly();
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

int StringExp::getLength() {
  // Retorna el tamaño en bytes del string (todos son es ASCII)
  // Le sumo 1 para agregar el caracter NULL del final
  return this->str.length()+1;
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
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()+exp2->getInteger());
  } else if (*this->type == FloatType::getInstance()) {
    result = new FloatExp(exp1->getFloat()+exp2->getFloat());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// Substraction
Expression* Substraction::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()-exp2->getInteger());
  } else if (*this->type == FloatType::getInstance()) {
    result = new FloatExp(exp1->getFloat()-exp2->getFloat());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// Multiplication
Expression* Multiplication::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()*exp2->getInteger());
  } else if (*this->type == FloatType::getInstance()) {
    result = new FloatExp(exp1->getFloat()*exp2->getFloat());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// Division
Expression* Division::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()/exp2->getInteger());
  } else if (*this->type == FloatType::getInstance()) {
    result = new FloatExp(exp1->getFloat()/exp2->getFloat());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

Expression* Remainder::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(exp1->getInteger()%exp2->getInteger());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

Expression* Minus::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;

  Expression* result;
  if (*this->type == IntType::getInstance()) {
    result = new IntExp(-exp1->getInteger());
  } else if (*this->type == FloatType::getInstance()) {
    result = new FloatExp(-exp1->getFloat());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

// And
Expression* And::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == BoolType::getInstance()) {
    result = new BoolExp(exp1->getBool() && exp2->getBool());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// Or
Expression* Or::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;

  Expression* result;
  if (*this->type == BoolType::getInstance()) {
    result = new BoolExp(exp1->getBool() || exp2->getBool());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

Expression* Not::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;

  Expression* result;
  if (*this->type == BoolType::getInstance()) {
    result = new BoolExp(!exp1->getBool());
  } else {
    return this;
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

// Greater
Expression* Greater::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() > exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() > exp2->getFloat());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// GreaterEq
Expression* GreaterEq::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() >= exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() >= exp2->getFloat());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

Expression* Equal::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() == exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() == exp2->getFloat());
  } else if (*exp1->getType() == BoolType::getInstance()) {
    result = new BoolExp(exp1->getBool() == exp2->getBool());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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

Expression* NotEqual::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() != exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() != exp2->getFloat());
  } else if (*exp1->getType() == BoolType::getInstance()) {
    result = new BoolExp(exp1->getBool() != exp2->getBool());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// Less
Expression* Less::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() < exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() < exp2->getFloat());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
}

// LessEq
Expression* LessEq::cfold() {
  this->exp1 = this->exp1->cfold();
  if (!exp1->isConstant()) return this;
  this->exp2 = this->exp2->cfold();
  if (!exp2->isConstant()) return this;
  if (*this->type == ErrorType::getInstance()) return this;

  Expression* result;
  if (*exp1->getType() == IntType::getInstance()) {
    result = new BoolExp(exp1->getInteger() <= exp2->getInteger());
  } else if (*exp1->getType() == FloatType::getInstance()) {
    result = new BoolExp(exp1->getFloat() <= exp2->getFloat());
  }
  result->setLocation(exp1->getFirstLine(),exp1->getFirstCol(),0,0);
  delete exp1;
  delete exp2;
  delete this;
  return result;
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
    if (this->index->isConstant() and
	cast_tarr->getLength()>0) {
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

bool Index::isLvalue() { return true; }

bool Index::isAssignable() {
  return this->array->isAssignable();
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
    this->type = &(ErrorType::getInstance());
    return;
  }
  BoxField* field = bt->getField(this->field);
  if (!field) {
    program.error("No existe el campo '"+this->field+"' en '"+t->toString()+"'",
		  this->fline, this->fcol);
    this->type = &(ErrorType::getInstance());
    return;
  } else {
    this->type = field->type;
  }
}

void Dot::print(int nesting) {
  std::string padding(nesting*2, ' ');
  this->box->print(nesting+1);
  std::cout << padding << "." << std::endl
	    << padding << "  " << field << std::endl;
}

bool Dot::isLvalue() { return true; }

bool Dot::isAssignable() {
  return this->box->isAssignable();
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
    this->checkedFunction=true;
    SymFunction* symfun = program.symtable.lookup_function(name);
    this->symf= symfun;
  }
  if(symf==NULL) return &(ErrorType::getInstance());
  return this->symf->getType(); 
}

void FunCallExp::check() {
  if (!checkedFunction){
    SymFunction* symfun = program.symtable.lookup_function(name);
    this->symf= symfun;
    this->checkedFunction=true;
  } else {
    name = this->symf->getId();
  }
    
  if (symf == NULL) {
    program.error("llamada a función no declarada '"+name+"'",
		  this->fline, this->fcol);
    return;
  } 
  
  // Chequear que el numero de parametros y arg coincidan
  if(this->args.size()!= this->symf->getArgumentCount()){
    program.error("el numero de argumentos de la llamada a la funcion '"+name+"'"
                  +" es incorrecto", this->fline, this->fcol);
    return;
  }
  // Chequear los tipos de los parametros 
  bool error=false;
  std::list<Expression*>::iterator arg= this->args.begin();
  for(ArgList::iterator param= this->symf->getArguments()->begin();
      param!=this->symf->getArguments()->end(); param++,arg++){

    (*arg)->check();
    (*arg) = (*arg)->cfold();

    if (*(*arg)->getType() == ErrorType::getInstance()) {
      continue;
    }

    if( (*(*arg)->getType()) != (*(*param)->getType()) ){
      program.error("en la llamada a la funcion '"+name+"'"
		    +" el tipo del argumento '"+(*param)->getId()+
		    "' es de tipo '"+(*param)->getType()->toString()+
		    "' pero se encontró '"+(*arg)->getType()->toString()+"'",
		    (*arg)->getFirstLine(), (*arg)->getFirstCol());
      continue;
    }
    // Chequear que los argumentos readonly sean pasados como tal
    SymVar *vart= dynamic_cast<SymVar*>(*arg);
    if (vart and vart->isReadonly() and (*param)->isReference())
      // cambie a isReference porque es aceptable pasar una variable readonly
      // por valor, sea o no readonly el argumento
      program.error("en la llamada a la funcion '"+name+"'"
                    +" el argumento '"+ vart->getId()+"' es de solo lectura",
		    (*arg)->getFirstLine(), (*arg)->getFirstCol());

  }
}
