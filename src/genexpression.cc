#include <iostream>
#include <utility>
#include "expression.hh"
#include "IntermCode.hh"
#include "type.hh"

extern IntermCode intCode;

std::pair<SymVar*,SymVar*> Expression::genlvalue() {
  SymVar* t = new SymVar("lvalue_base",0,0,false,0);
  SymVar* o = new SymVar("lvalue_offset",0,0,false,0);
  return std::pair<SymVar*,SymVar*>(t,o);
}

std::pair<SymVar*,SymVar*> VarExp::genlvalue() {
  return std::pair<SymVar*,SymVar*>(this->symv,(SymVar*)NULL);
}

std::pair<SymVar*,SymVar*> Index::genlvalue() {
  SymVar* offset = this->index->gen();
  // Este cast debe funcionar porque ya debió chequearse el tipo en check()
  ArrayType* arrt = dynamic_cast<ArrayType*>(this->array->getType());
  int elem_size = arrt->getBaseType()->getSize();

  std::pair<SymVar*,SymVar*> location = this->array->genlvalue();
  if (location.second) {
    // offset := offset * tamaño_elemento
    std::cout << offset->getId() << " := "
	      << offset->getId() << " * "
	      << elem_size << std::endl;
    return std::pair<SymVar*,SymVar*>(location.first, offset);
  } else {
    // off := off * tamaño_elemento
    std::cout << offset->getId() << " := "
	      << offset->getId() << " * "
	      << elem_size << std::endl;
    // base := base + offset
    std::cout << (location.second)->getId() << " := "
	      << (location.second)->getId() << " + "
	      << offset->getId() << std::endl;
    return location;
  }
}

std::pair<SymVar*,SymVar*> Dot::genlvalue() {
  SymVar* t = new SymVar("lvalue_base",0,0,false,0);
  SymVar* o = new SymVar("lvalue_offset",0,0,false,0);
  return std::pair<SymVar*,SymVar*>(t,o);
}
