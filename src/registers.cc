#include <map>
#include <set>

#include "registers.hh"
#include "symbol.hh"

std::string regToString(Reg r) {
  switch (r) {
  case Reg::zero:
    return std::string("$zero");

  case Reg::a0:
    return std::string("$a0");
  case Reg::a1:
    return std::string("$a1");
  case Reg::a2:
    return std::string("$a2");
  case Reg::a3:
    return std::string("$a3");

  case Reg::v0:
    return std::string("$v0");
  case Reg::v1:
    return std::string("$v1");

  case Reg::t0:
    return std::string("$t0");
  case Reg::t1:
    return std::string("$t1");
  case Reg::t2:
    return std::string("$t2");
  case Reg::t3:
    return std::string("$t3");
  case Reg::t4:
    return std::string("$t4");
  case Reg::t5:
    return std::string("$t5");
  case Reg::t6:
    return std::string("$t6");
  case Reg::t7:
    return std::string("$t7");


  case Reg::s0:
    return std::string("$s0");
  case Reg::s1:
    return std::string("$s1");
  case Reg::s2:
    return std::string("$s2");
  case Reg::s3:
    return std::string("$s3");
  case Reg::s4:
    return std::string("$s4");
  case Reg::s5:
    return std::string("$s5");
  case Reg::s6:
    return std::string("$s6");
  case Reg::s7:
    return std::string("$s7");

  case Reg::t8:
    return std::string("$t8");
  case Reg::t9:
    return std::string("$t9");

  case Reg::gp:
    return std::string("$gp");
  case Reg::sp:
    return std::string("$sp");
  case Reg::fp:
    return std::string("$fp");
  case Reg::ra:
    return std::string("$ra");

  case Reg::f0:
    return std::string("$f0");
  case Reg::f1:
    return std::string("$f1");
  case Reg::f2:
    return std::string("$f2");
  case Reg::f3:
    return std::string("$f3");
  case Reg::f4:
    return std::string("$f4");
  case Reg::f5:
    return std::string("$f5");
  case Reg::f6:
    return std::string("$f6");
  case Reg::f7:
    return std::string("$f7");
  case Reg::f8:
    return std::string("$f8");
  case Reg::f9:
    return std::string("$f9");
  case Reg::f10:
    return std::string("$f10");
  case Reg::f11:
    return std::string("$f11");
  case Reg::f12:
    return std::string("$f12");
  case Reg::f13:
    return std::string("$f13");
  case Reg::f14:
    return std::string("$f14");
  case Reg::f15:
    return std::string("$f15");
  case Reg::f16:
    return std::string("$f16");
  case Reg::f17:
    return std::string("$f17");
  case Reg::f18:
    return std::string("$f18");
  case Reg::f19:
    return std::string("$f19");
  case Reg::f20:
    return std::string("$f20");
  case Reg::f21:
    return std::string("$f21");
  case Reg::f22:
    return std::string("$f22");
  case Reg::f23:
    return std::string("$f23");
  case Reg::f24:
    return std::string("$f24");
  case Reg::f25:
    return std::string("$f25");
  case Reg::f26:
    return std::string("$f26");
  case Reg::f27:
    return std::string("$f27");
  case Reg::f28:
    return std::string("$f28");
  case Reg::f29:
    return std::string("$f29");
  case Reg::f30:
    return std::string("$f30");
  case Reg::f31:
    return std::string("$f31");

  }

}

bool isFloatReg(Reg r) {
  switch (r) {
  case Reg::zero:
  case Reg::a0:
  case Reg::a1:
  case Reg::a2:
  case Reg::a3:
  case Reg::v0:
  case Reg::v1:
  case Reg::t0:
  case Reg::t1:
  case Reg::t2:
  case Reg::t3:
  case Reg::t4:
  case Reg::t5:
  case Reg::t6:
  case Reg::t7:
  case Reg::s0:
  case Reg::s1:
  case Reg::s2:
  case Reg::s3:
  case Reg::s4:
  case Reg::s5:
  case Reg::s6:
  case Reg::s7:
  case Reg::t8:
  case Reg::t9:
  case Reg::gp:
  case Reg::sp:
  case Reg::fp:
  case Reg::ra:
    return false;

  case Reg::f0:
  case Reg::f1:
  case Reg::f2:
  case Reg::f3:
  case Reg::f4:
  case Reg::f5:
  case Reg::f6:
  case Reg::f7:
  case Reg::f8:
  case Reg::f9:
  case Reg::f10:
  case Reg::f11:
  case Reg::f12:
  case Reg::f13:
  case Reg::f14:
  case Reg::f15:
  case Reg::f16:
  case Reg::f17:
  case Reg::f18:
  case Reg::f19:
  case Reg::f20:
  case Reg::f21:
  case Reg::f22:
  case Reg::f23:
  case Reg::f24:
  case Reg::f25:
  case Reg::f26:
  case Reg::f27:
  case Reg::f28:
  case Reg::f29:
  case Reg::f30:
  case Reg::f31:
    return true;

  }

}
