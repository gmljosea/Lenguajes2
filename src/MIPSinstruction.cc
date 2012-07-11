#include <string>

#include "registers.hh"
#include "MIPSinstruction.hh"

std::string comma(" , ");

std::string Syscall::toString() {
  return std::string("syscall");
}

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

std::string LwS::toString() {
  std::string result("l.s ");
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

std::string Li::toString() {
  std::string result("li ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(std::to_string((long long int) inmediate));
  return result;
}

std::string LiS::toString() {
  std::string result("li.s ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(std::to_string((long double) inmediate));
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

std::string Add::toString() {
  std::string result("add ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string Addi::toString() {
  std::string result("addi ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(std::to_string((long long int) inmediate));
  return result;
}

std::string AddS::toString() {
  std::string result("add.s ");
  result.append(regToString(Rfd));
  result.append(comma);
  result.append(regToString(Rfx));
  result.append(comma);
  result.append(regToString(Rfy));
  return result;
}

std::string Sub::toString() {
  std::string result("sub ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string Subi::toString() {
  std::string result("subi ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(std::to_string((long long int) inmediate));
  return result;
}

std::string SubS::toString() {
  std::string result("sub.s ");
  result.append(regToString(Rfd));
  result.append(comma);
  result.append(regToString(Rfx));
  result.append(comma);
  result.append(regToString(Rfy));
  return result;
}

std::string Mul::toString() {
  std::string result("mul ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string MulS::toString() {
  std::string result("mul.s ");
  result.append(regToString(Rfd));
  result.append(comma);
  result.append(regToString(Rfx));
  result.append(comma);
  result.append(regToString(Rfy));
  return result;
}

std::string Div::toString() {
  std::string result("div ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string DivS::toString() {
  std::string result("div.s ");
  result.append(regToString(Rfd));
  result.append(comma);
  result.append(regToString(Rfx));
  result.append(comma);
  result.append(regToString(Rfy));
  return result;
}

std::string Rem::toString() {
  std::string result("rem ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string Mfhi::toString() {
  std::string result("mfhi ");
  result.append(regToString(reg));
  return result;
}

std::string Mflo::toString() {
  std::string result("mflo ");
  result.append(regToString(reg));
  return result;
}

std::string Move::toString() {
  std::string result("move ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  return result;
}


std::string CompES::toString() {
  std::string result("c.eq.s ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rfx));
  return result;
}

std::string CompLTES::toString() {
  std::string result("c.le.s ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rfx));
  return result;
}

std::string CompLS::toString() {
  std::string result("c.lt.s ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rfx));
  return result;
}

std::string Movz::toString() {
  std::string result("movz ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(regToString(Ry));
  return result;
}

std::string Mtc1::toString() {
  std::string result("mtc1 ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  return result;
}

std::string Mfc1::toString() {
  std::string result("mfc1 ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rfx));
  return result;
}

std::string CvtIF::toString() {
  std::string result("cvt.s.w ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  return result;
}

std::string CvtFI::toString() {
  std::string result("cvt.w.s ");
  result.append(regToString(Rfd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  return result;
}

std::string Negu::toString() {
  std::string result("neg ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  return result;
}

std::string ShiftL::toString() {
  std::string result("sll ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(std::to_string((long long int) shamt));
  return result;
}

std::string ShiftR::toString() {
  std::string result("srl ");
  result.append(regToString(Rd));
  result.append(std::string(" , "));
  result.append(regToString(Rx));
  result.append(std::string(" , "));
  result.append(std::to_string((long long int) shamt));
  return result;
}

std::string Blt::toString() {
  std::string result("blt ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string Ble::toString() {
  std::string result("ble ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string Beq::toString() {
  std::string result("beq ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string Bne::toString() {
  std::string result("bne ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string Bgt::toString() {
  std::string result("bgt ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string Bge::toString() {
  std::string result("bge ");
  result.append(regToString(Rx));
  result.append(comma);
  result.append(regToString(Ry));
  result.append(comma);
  result.append(label->toString());
  return result;
}

std::string J::toString() {
  return (std::string("j ")+label->toString());
}

std::string Jal::toString() {
  return (std::string("jal ")+label->toString());
}

std::string Jr::toString() {
  return (std::string("jr ")+regToString(Rx));
}

std::string Bclf::toString() {
  return (std::string("bclf ")+label->toString());
}

std::string Bclt::toString() {
  return (std::string("bclt ")+label->toString());
}

/*
std::string NopM::toString() {
  return (std::string("nop"));
  }*/
