#ifndef DEVANIX_MIPS
#define DEVANIX_MIPS

#include "registers.hh"
#include "instruction.hh"

class MIPSinstruction : public Instruction{

};

enum Mode{
  direct,
  indirect
};

/* Load address La Rd,var  direct
 *              La Rd,offset(Rx) indirect
 */
class La : public MIPSinstruction{
private:
  Mode mode;
  Reg Rd;
  // En caso de ser direct (Con etiqueta) 
  Label *varLabel;
  // En caso de ser indirect
  int offset;
  Reg Rx;

public:
  La(Reg Rd,Label* varLabel): Rd(Rd),varLabel(varLabel),mode(direct){};
  La(Reg Rd,int offs,Reg Rx): Rd(Rd),offset(offs),Rx(Rx),mode(indirect),varLabel(NULL) {};
  virtual std::string toString();
};

/* Load inmediate Li Rd, #a */
class Li : MIPSinstruction{
private:
  Reg Rd;
  int inmediate;

public:
  Li(Reg Rd,int inmed): Rd(Rd),inmediate(inmed){};
};

/* Load inmediate flotante simple Li.s Rd, #a */
class LiS : MIPSinstruction{
private:
  Reg Rd;
  float inmediate;

public:
  LiS(Reg Rd,float inmed): Rd(Rd),inmediate(inmed){};
};

/* Load Word  Lw Rd,var
 *            Lw Rd,offset(Rx)
 */
class Lw : public MIPSinstruction{
private:
  Mode mode;
  Reg Rd;
  // Direct, label of var
  Label *varLabel;
  // Indirect
  int offset;
  Reg Rx;

public:
  Lw(Reg Rd,Label* varLabel): Rd(Rd),varLabel(varLabel),mode(direct){};
  Lw(Reg Rd,int offs,Reg Rx): Rd(Rd),offset(offs),Rx(Rx),mode(indirect),varLabel(NULL){};
  virtual std::string toString();
};

/* Store Word Sw Rs,var
 *            Sw Rs,offset(Rd)
 */
class Sw : public MIPSinstruction{
private:
  Mode mode;
  Reg Rs;
  // Direct, label of var
  Label *varLabel;
  // Indirect
  int offset;
  Reg Rd;

public:
  Sw(Reg Rs,Label* varLabel): Rs(Rs),varLabel(varLabel),mode(direct){};
  Sw(Reg Rs,int offs,Reg Rd): Rs(Rs),offset(offs),Rd(Rd),mode(indirect),varLabel(NULL){};
  virtual std::string toString();
};

/*Add Rd, Rx, Ry*/
class Add : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Add(Reg Rd,Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
};

/*Addi Rd, Rx, #a */
class Addi : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  int inmediate;
public:
  Addi(Reg Rd,Reg Rx,int inmed): Rd(Rd),Rx(Rx),inmediate(inmed){};
};

/* Add.s Rfd, Rfx, Rfy*/
class AddS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  AddS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
};

/*Sub Rd, Rx,Ry*/
class Sub : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Sub(Reg Rd,Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
};

/*Subi Rd, Rx, #a*/
class Subi : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  int inmediate;
public:
  Subi(Reg Rd,Reg Rx,int inmed): Rd(Rd),Rx(Rx),inmediate(inmed){};
};

/* Sub.s Rfd, Rfx, Rfy*/
class SubS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  SubS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
};

/*Mul Rd, Rx,Ry*/
class Mul : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Mul(Reg Rd,Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
};

/* Mul.s Rfd, Rfx, Rfy*/
class MulS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  MulS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
};

// Deja el resultado en Lo y Hi
class Div : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
public:
  Div(Reg Rx,Reg Ry): Rx(Rx),Ry(Ry){};
};

/*Move From Hi*/
class Mfhi : public MIPSinstruction{
private:
  Reg reg;
public:
  Mfhi(Reg reg): reg(reg){};
};

/*Move From Lo*/
class Mflo : public MIPSinstruction{
private:
  Reg reg;
public:
  Mflo(Reg reg): reg(reg){};
};

/*Div.s Rfd, Rfx, Rfy*/
class DivS : MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  DivS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
};


class Negu : MIPSinstruction{

};

class AndM : MIPSinstruction{

};

class Andi : MIPSinstruction{

};

class OrM : MIPSinstruction{

};

class Ori : MIPSinstruction{

};

class NotM : MIPSinstruction{

};

/*    Jumps and Branches    */
  
// Branch less than  <
class Blt : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Blt(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch less Eq <=
class Ble :public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Ble(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Eq == 
class Beq : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Beq(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Not Eq !=
class Bne : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bne(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Greather than >
class Bgt : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bgt(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Branch Greater Eq >=
class Bge : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bge(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
};

// Jump
class J : public MIPSinstruction{
private:
  Label* label;

public:
  J(Label* label): label(label){};
  virtual std::string toString();
};

// Jump and Link
class Jal : public MIPSinstruction{
private:
  Label *label;

public:
  Jal(Label* label): label(label){};
  virtual std::string toString();
};

// Jump Register 
class Jr : public MIPSinstruction{
private:
  Reg Rx;

public:
  Jr(Reg Rx): Rx(Rx){};
  virtual std::string toString();
};

#endif
