#include <iostream>

#include "type.hh"

// Type
int Type::getSize() {
  return this->size;
}

int Type::getAlignment() {
  return this->alignment;
}

bool Type::operator==(Type& b) {
  return this == &b;
}

// IntType
IntType& IntType::getInstance() {
  static IntType instance;
  return instance;
}

// FloatType
FloatType& FloatType::getInstance() {
  static FloatType instance;
  return instance;
}

// BoolType
BoolType& BoolType::getInstance() {
  static BoolType instance;
  return instance;
}

// CharType
CharType& CharType::getInstance() {
  static CharType instance;
  return instance;
}

// VoidType
VoidType& VoidType::getInstance() {
  static VoidType instance;
  return instance;
}

// StringType

// ErrorType
ErrorType& ErrorType::getInstance() {
  static ErrorType instance;
  return instance;
}

// ArrayType
bool ArrayType::operator==(Type& t) {
  ArrayType* ta;
  if (dynamic_cast<ArrayType*>(&t)) {
    return this->basetype == ta->getBaseType()
      && this->length == ta->getLength();
  } else {
    return false;
  }
}

/**
 * Como el tipo base de un array podría ser un box incompleto al momento
 * de instanciar el array, entonces no podemos calcular el tamaño y
 * alineación del arreglo en el constructor, entonces lo hacemos en las
 * propias llamadas a getSize y getAlignment.
 * La precondición es que se llamen cuando se esté seguro que el tipo base
 * no es un box incompleto.
 */
int ArrayType::getSize() {
  return this->basetype->getSize() * this->length;
}

int ArrayType::getAlignment() {
  return this->basetype->getAlignment();
}

Type* ArrayType::getBaseType() {
  return this->basetype;
}

int ArrayType::getLength() {
  return this->length;
}

int ArrayType::getOffset(int pos) {
  return this->basetype->getSize()*pos;
}

// BoxType

int main() {
  IntType& it = IntType::getInstance();
  FloatType& it2 = FloatType::getInstance();
  std::cout << it.getSize() << " " << it.getAlignment() <<
    " " << (it == it2) << std::endl;
}
