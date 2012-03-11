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

// !!! Implementar esto como debe ser
void Type::print() {
  std::cout << "Algun tipo" << std::endl;
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
void BoxType::addFixedField(Type* type, std::string name) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->offset = 0;
  field->braced = false;
  this->fixed_fields[name] = field;
}

void BoxType::addVariantField(Type* type, std::string name, bool braced) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->offset = 0;
  field->braced = braced;
  this->variant_fields[name] = field;
}

BoxField* BoxType::getField(std::string field) {
  std::map<std::string, BoxField*>::iterator it = this->fixed_fields.find(field);
  if (it != this->fixed_fields.end()) {
    return it->second;
  }

  std::map<std::string, BoxField*>::iterator it2 = this->variant_fields.find(field);
  if (it2 != this->variant_fields.end()) {
    return it2->second;
  }

  return NULL;
}

bool BoxType::isIncomplete() {
  return this->isIncomplete();
}

void BoxType::setIncomplete(bool ic) {
  this->incomplete = ic;
}

std::string BoxType::getName() {
  return this->name;
}

// !!!!!
void BoxType::check() {
}

// !!!!!
bool BoxType::reaches(BoxType& box) {
}
