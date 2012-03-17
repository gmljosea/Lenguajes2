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

bool Type::operator!=(Type& b) {
  return !(*this == b);
}

void Type::print() {
  std::cout << this->toString() << std::endl;
}

std::string Type::toString() {
  return std::string("Algun tipo");
}

// IntType
IntType& IntType::getInstance() {
  static IntType instance;
  return instance;
}

std::string IntType::toString() {
  return std::string("int");
}

// FloatType
FloatType& FloatType::getInstance() {
  static FloatType instance;
  return instance;
}

std::string FloatType::toString() {
  return std::string("float");
}

// BoolType
BoolType& BoolType::getInstance() {
  static BoolType instance;
  return instance;
}

std::string BoolType::toString() {
  return std::string("bool");
}

// CharType
CharType& CharType::getInstance() {
  static CharType instance;
  return instance;
}

std::string CharType::toString() {
  return std::string("char");
}

// VoidType
VoidType& VoidType::getInstance() {
  static VoidType instance;
  return instance;
}

std::string VoidType::toString() {
  return std::string("void");
}

// StringType
bool StringType::operator==(Type& t) {
  return true;
  StringType* ta;
  if (ta = dynamic_cast<StringType*>(&t)) {
    return this->size == ta->getSize();
  } else {
    return false;
  }
}

void StringType::setLength(int length) {
  this->size = length;
}

std::string StringType::toString() {
  return std::string("string");
}

// ErrorType
ErrorType& ErrorType::getInstance() {
  static ErrorType instance;
  return instance;
}

std::string ErrorType::toString() {
  return std::string("error");
}

// ArrayType
bool ArrayType::operator==(Type& t) {
  ArrayType* ta;
  if (ta = dynamic_cast<ArrayType*>(&t)) {
    return this->basetype == ta->getBaseType()
      && this->length == ta->getLength();
  } else {
    return false;
  }
}

std::string ArrayType::toString() {
  return std::string(this->basetype->toString()+" array["
		     +std::to_string(length)+"]");
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
  field->grouped = false;
  field->groupnum = 0;
  this->fixed_fields.push_back(field);
  this->fields_hash[name] = field;
}

void BoxType::addVariantField(Type* type, std::string name, bool grouped) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->offset = 0;
  field->grouped = grouped;
  field->groupnum = this->groupcount;
  this->variant_fields.push_back(field);
  this->fields_hash[name] = field;
}

void BoxType::startGrouping() {
  this->groupcount++;
}

BoxField* BoxType::getField(std::string field) {
  std::unordered_map<std::string, BoxField*>::iterator it
    = this->fields_hash.find(field);
  if (it != this->fields_hash.end()) {
    return it->second;
  }
  return NULL;
}

bool BoxType::isIncomplete() {
  return this->isIncomplete();
}

void BoxType::setIncomplete(bool ic) {
  this->incomplete = ic;
}

int BoxType::getFieldCount() {
  return this->fields_hash.size();
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

std::string BoxType::toString() {
  return std::string("box "+this->name);
}
