#include <utility>
#include "expression.hh"

std::pair<SymVar*,int> Expression::genlvalue() {
  return std::pair<SymVar*,int>(NULL,0);
}
