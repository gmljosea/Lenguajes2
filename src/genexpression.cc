#include <utility>
#include "expression.hh"

std::pair<SymVar*,int> Expression::genlvalue() {
  SymVar* t = new SymVar("test_genlvalue",0,0,false,0);
  return std::pair<SymVar*,int>(t,0);
}
