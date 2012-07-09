#include <string>

#include "registers.hh"
#include "MIPSinstruction.hh"

std::string La::toString() {
  std::string result("la ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  if (varLabel) {
    result.append(varLabel->toString());
  } else {
    result.append(std::to_string((long long int) offset));
    result.append("(");
    result.append(regToString(Rx));
    result.append(")");
  }
  return result;
}

std::string Lw::toString() {
  std::string result("lw ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  if (varLabel) {
    result.append(varLabel->toString());
  } else {
    result.append(std::to_string((long long int) offset));
    result.append("(");
    result.append(regToString(Rx));
    result.append(")");
  }
  return result;
}

std::string Sw::toString() {
  std::string result("sw ");
  result.append(regToString(Rs));
  result.append(std::string(" , "));
  if (varLabel) {
    result.append(varLabel->toString());
  } else {
    result.append(std::to_string((long long int) offset));
    result.append("(");
    result.append(regToString(Rd));
    result.append(")");
  }
  return result;
}

std::string Jr::toString() {
  return (std::string("jr ")+regToString(Rx));
}
