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

class Syscall : public MIPSinstruction {
public:
  virtual std::string toString();
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

// l.s Rfd, off(R
// l.s Rfd, addr
class LwS : public MIPSinstruction{
private:
  Mode mode;
  Reg Rd;
  // En caso de ser direct (Con etiqueta)
  Label *varLabel;
  // En caso de ser indirect
  int offset;
  Reg Rx;

public:
  LwS(Reg Rd,Label* varLabel): Rd(Rd),varLabel(varLabel),mode(direct){};
  LwS(Reg Rd,int offs,Reg Rx): Rd(Rd),offset(offs),Rx(Rx),mode(indirect),varLabel(NULL) {};
  virtual std::string toString();
};

/* Load inmediate Li Rd, #a */
class Li : public MIPSinstruction{
private:
  Reg Rd;
  int inmediate;

public:
  Li(Reg Rd,int inmed): Rd(Rd),inmediate(inmed){};
  virtual std::string toString();
};

/* Load inmediate flotante simple Li.s Rd, #a */
class LiS : public MIPSinstruction{
private:
  Reg Rd;
  float inmediate;

public:
  LiS(Reg Rd,float inmed): Rd(Rd),inmediate(inmed){};
  virtual std::string toString();
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
  virtual std::string toString();
};

/*Addi Rd, Rx, #a */
class Addi : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  int inmediate;
public:
  Addi(Reg Rd,Reg Rx,int inmed): Rd(Rd),Rx(Rx),inmediate(inmed){};
  virtual std::string toString();
};

/* Add.s Rfd, Rfx, Rfy*/
class AddS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  AddS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
  virtual std::string toString();
};

/*Sub Rd, Rx,Ry*/
class Sub : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Sub(Reg Rd,Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
  virtual std::string toString();
};

/*Subi Rd, Rx, #a*/
class Subi : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  int inmediate;
public:
  Subi(Reg Rd,Reg Rx,int inmed): Rd(Rd),Rx(Rx),inmediate(inmed){};
  virtual std::string toString();
};

/* Sub.s Rfd, Rfx, Rfy*/
class SubS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  SubS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
  virtual std::string toString();
};

/*Mul Rd, Rx,Ry*/
class Mul : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Mul(Reg Rd,Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
  virtual std::string toString();
};

/* Mul.s Rfd, Rfx, Rfy*/
class MulS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  MulS(Reg Rfd,Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
  virtual std::string toString();
};

/* Div Rd, Rx, Ry */
class Div : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Div(Reg Rd, Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
  virtual std::string toString();
};

/* Div.s Rfd, Rfx, Rfy */
class DivS : public MIPSinstruction{
private:
  Reg Rfd;
  Reg Rfx;
  Reg Rfy;
public:
  DivS(Reg Rfd, Reg Rfx,Reg Rfy): Rfd(Rfd),Rfx(Rfx),Rfy(Rfy){};
  virtual std::string toString();
};

/* Rem Rd, Rx, Ry */
class Rem : public MIPSinstruction{
private:
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Rem(Reg Rd, Reg Rx,Reg Ry): Rd(Rd),Rx(Rx),Ry(Ry){};
  virtual std::string toString();
};

/*Move From Hi*/
class Mfhi : public MIPSinstruction{
private:
  Reg reg;
public:
  Mfhi(Reg reg): reg(reg){};
  virtual std::string toString();
};

/*Move From Lo*/
class Mflo : public MIPSinstruction{
private:
  Reg reg;
public:
  Mflo(Reg reg): reg(reg){};
  virtual std::string toString();
};

// move Rd, Rx
class Move : public MIPSinstruction {
  Reg Rd;
  Reg Rx;
public:
  Move (Reg Rd, Reg Rx) : Rd(Rd), Rx(Rx) {};
  virtual std::string toString();
};

// movz Rd, Rx, Ry
class Movz : public MIPSinstruction {
  Reg Rd;
  Reg Rx;
  Reg Ry;
public:
  Movz (Reg Rd, Reg Rx, Reg Ry) : Rd(Rd), Rx(Rx), Ry(Ry) {};
  virtual std::string toString();
};

/* mtc1 Rfd, Rx */
class Mtc1 : public MIPSinstruction {
  Reg Rfd;
  Reg Rx;
public:
  Mtc1 (Reg Rfd, Reg Rx) : Rfd(Rfd), Rx(Rx) {};
  virtual std::string toString();
};

/* mfc1 Rd, Rfx */
class Mfc1 : public MIPSinstruction {
  Reg Rd;
  Reg Rfx;
public:
  Mfc1 (Reg Rd, Reg Rfx) : Rd(Rd), Rfx(Rfx) {};
  virtual std::string toString();
};

// cvt.s.w Rfd, Rfx
// Convierte el entero en Rfx a un flotante precision simple
class CvtIF : public MIPSinstruction {
  Reg Rfd;
  Reg Rx;
public:
  CvtIF (Reg Rfd, Reg Rx) : Rfd(Rfd), Rx(Rx) {};
  virtual std::string toString();
};

// cvt.w.s Rfd, Rfx
// Convierte el flotante en Rfx a entero
class CvtFI : public MIPSinstruction {
  Reg Rfd;
  Reg Rx;
public:
  CvtFI (Reg Rfd, Reg Rx) : Rfd(Rfd), Rx(Rx) {};
  virtual std::string toString();
};

/* Negu Rd, Rx */ 
class Negu : MIPSinstruction{
  Reg Rd;
  Reg Rx;
public:
  Negu (Reg Rd, Reg Rx) : Rd(Rd), Rx(Rx) {};
  virtual std::string toString();
};

/* Neg.s Rd, Rx */ 
class NegS : MIPSinstruction{
  Reg Rd;
  Reg Rx;
public:
  NegS (Reg Rd, Reg Rx) : Rd(Rd), Rx(Rx) {};
  virtual std::string toString();
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

// sll Rd, Rx, shamt
class ShiftL : MIPSinstruction {
  Reg Rd;
  Reg Rx;
  int shamt;
public:
  ShiftL (Reg Rd, Reg Rx, int shamt) : Rd(Rd), Rx(Rx), shamt(shamt) {};
  virtual std::string toString();
};

// srl Rd, Rx, shamt
class ShiftR : MIPSinstruction {
  Reg Rd;
  Reg Rx;
  int shamt;
public:
  ShiftR (Reg Rd, Reg Rx, int shamt) : Rd(Rd), Rx(Rx), shamt(shamt) {};
  virtual std::string toString();
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
  virtual std::string toString();
};

// Branch less Eq <=
class Ble :public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Ble(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
  virtual std::string toString();
};

// Branch Eq == 
class Beq : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Beq(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
  virtual std::string toString();
};

// Branch Not Eq !=
class Bne : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bne(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
  virtual std::string toString();
};

// Branch Greater than >
class Bgt : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bgt(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
  virtual std::string toString();
};

// Branch Greater Eq >=
class Bge : public MIPSinstruction{
private:
  Reg Rx;
  Reg Ry;
  Label* label;

public:
  Bge(Reg Rx,Reg Ry,Label* label): Rx(Rx),Ry(Ry),label(label){};
  virtual std::string toString();
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

/*
class NopM : public MIPSinstruction {
public:
  virtual std::string toString();
  }*/

#endif
