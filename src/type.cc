#include <iostream>

#include "type.hh"

void Type::print() {
  std::cout << "algún tipo" << std::endl;
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

StringType::StringType() {
  this->type = TypeKind::String;
}

bool StringType::operator==(Type& b) {
  return b.getTypeKind() == TypeKind::String;
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

FloatType::FloatType() {
  this->scalartype = ScalarKind::Float;
}

CharType::CharType() {
  this->scalartype = ScalarKind::Char;
}

BoolType::BoolType() {
  this->scalartype = ScalarKind::Bool;
}
