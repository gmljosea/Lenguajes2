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

/* Load address La Rd,var */
class La : public MIPSinstruction{
private:
  Reg Rd;
  // La verdadera instruccion usa el nombre de la global
  // Y si es local usar indireccion const(reg)
  SymVar *addr;

public:
  Lw(Reg Rd,SymVar* addr): Rd(Rd),addr(addr){};
};

/* Load inmediate */
class Li : MIPSinstruction{
private:
  Reg Rd;
  int inmediate;

public:
  Li(Reg Rd,int inmed): Rd(Rd),inmediate(inmed){};
};

/* Load Word  Lw Rd,addr */
class Lw : public MIPSinstruction{
private:
  Reg Rd;
  // La verdadera instruccion usa el nombre de la global si lo es
  // o el offset de la local y el $fp
  SymVar *addr;

public:
  Lw(Reg Rd,SymVar* addr): Rd(Rd),addr(addr){};
};

// Store word Sw Rx,addr
class Sw : public MIPSinstruction{
private:
  // Si es una local hay que tomar el offset mas el $fp
  SymVar *addr;
  Reg Rx;
public:
  Sw(SymVar* addr,Reg Rx): Rx(Rx),addr(addr){};
};

class Add : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Reg Rz;
public:
  Add(Reg Rx,Reg Ry,Reg Rz): Rx(Rx),Ry(Ry),Rz(Rz){};
};

class Addi : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  int inmediate;
public:
  Add(Reg Rx,Reg Ry,int inmed): Rx(Rx),Ry(Ry),inmediate(inmed){};
};

class AddS : MIPSinstruction{

};

class Sub : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Reg Rz;
public:
  Sub(Reg Rx,Reg Ry,Reg Rz): Rx(Rx),Ry(Ry),Rz(Rz){};
};

class Subi : MIPSinstruction{

};

class SubS : MIPSinstruction{

};

class Mul : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Reg Rz;
public:
  Mul(Reg Rx,Reg Ry,Reg Rz): Rx(Rx),Ry(Ry),Rz(Rz){};
};

class MulS : MIPSinstruction{

};

// Deja el resultado en Lo y Hi
class Div : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
public:
  Div(Reg Rx,Reg Ry): Rx(Rx),Ry(Ry){};
};

class Mfhi : public MIPSinstruction{
private:
  Reg reg;
public:
  Mfhi(Reg reg): reg(reg){};
};

class Mflo : public MIPSinstruction{
private:
  Reg reg;
public:
  Mflo(Reg reg): reg(reg){};
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
class Blt : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Blt(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch less Eq <=
class Ble :public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Ble(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Eq == 
class Beq : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Beq(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Not Eq !=
class Bne : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Bne(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Greather than >
class Bgt : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Bgt(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Greater Eq >=
class Bge : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  std::string label;

public:
  Bge(Reg Rx,Reg Ry,std::string label): Rx(Rx),Ry(Ry),label(label){};
};

// Jump
class J : public MIPSinstruction{
private:
  std::string label;

public:
  Bge(std::string label): label(label){};
};

// Jump and Link
class Jal : public MIPSinstruction{
private:
  std::string label;

public:
  Jal(std::string label): label(label){};
};

// Jr 
class Jr : public MIPSinstruction{
private:
  Reg Rx;

public:
  Jr(Reg Rx): Rx(Rx){};
};

#endif
