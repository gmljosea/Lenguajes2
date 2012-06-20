#ifndef DEVANIX_MIPS
#define DEVANIX_MIPS

#include "instruction.hh"

class MIPSinstruction : public Instruction{

};

// Registros que seran usados por el generador de codigo
enum Reg{

  t0,
  t1,
  t2,
  t3,
  t4,
  t5,
  t6,
  t7,

  s0,
  s1,
  s2,
  s3,
  s4,
  s5,
  s6,
  s7,

  t8,
  t9,

};

class Li : MIPSinstruction{

};

class Lw : MIPSinstruction{

};

class Sw : MIPSinstruction{

};

class Add : MIPSinstruction{

};

class Addi : MIPSinstruction{

};

class AddS : MIPSinstruction{

};

class Sub : MIPSinstruction{

};

class Subi : MIPSinstruction{

};

class SubS : MIPSinstruction{

};

class Mul : MIPSinstruction{

};

class MulS : MIPSinstruction{

};

// Deja el resultado en Lo y Hi
class Div : MIPSinstruction{

};

class DivS : MIPSinstruction{

};


class Negu : MIPSinstruction{

};

class And : MIPSinstruction{

};

class Andi : MIPSinstruction{

};

class Or : MIPSinstruction{

};

class Ori : MIPSinstruction{

};

class Not : MIPSinstruction{

};
  
// Branch less than  <
class Blt : MIPSinstruction{

};

// Branch less Eq <=
class Ble : MIPSinstruction{

};

// Branch Eq == 
class Beq : MIPSinstruction{

};

// Branch Not Eq !=
class Bne : MIPSinstruction{

};

// Branch Greather than >
class Bgt : MIPSinstruction{

};

// Branch Greater Eq >=
class Bgt : MIPSinstruction{

};

#endif
