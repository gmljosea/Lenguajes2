#include <iostream>

#include "type.hh"

void Type::print() {
}

TypeKind Type::getTypeKind() {
  return this->type;
}

bool Type::operator==(Type& b) {
  return false;
};

VoidType::VoidType() {
  this->type = TypeKind::Void;
}

bool VoidType::operator==(Type& b) {
  return b.getTypeKind() == TypeKind::Void;
}

void VoidType::print() {
  std::cout << "void";
}

StringType::StringType() {
  this->type = TypeKind::String;
}

bool StringType::operator==(Type& b) {
  return b.getTypeKind() == TypeKind::String;
}

void StringType::print() {
  std::cout << "string";
}

ScalarType::ScalarType() {
  this->type = TypeKind::Scalar;
}

ScalarKind ScalarType::getScalarKind() {
  return this->scalartype;
}

bool ScalarType::operator==(Type& b) {
  return b.getTypeKind() == TypeKind::Scalar
    && dynamic_cast<ScalarType*>(&b)->getScalarKind() == this->scalartype;
}

IntType::IntType() {
  this->scalartype = ScalarKind::Integer;
}

void IntType::print() {
  std::cout << "int";
}

FloatType::FloatType() {
  this->scalartype = ScalarKind::Float;
}

void FloatType::print() {
  std::cout << "float";
}

CharType::CharType() {
  this->scalartype = ScalarKind::Char;
}

void CharType::print() {
  std::cout << "char";
}

BoolType::BoolType() {
  this->scalartype = ScalarKind::Bool;
}

void BoolType::print() {
  std::cout << "bool";
}
