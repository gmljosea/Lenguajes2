// Metodos gen de los Quads 
#include <list>
#include "Quad.hh"
#include "MIPSinstruction.hh"
#include "mipscode.hh"
#include "regdesc.hh"

extern MIPSCode mipscode;
extern RegDesc rdesc;


/** 
 * Asignacion de 3 o 2 direcciones
 * x:= y op z  =>  Rd:= Rx op Ry  
 * x:= op y    =>  Rd:= op Rx
 **/
std::list<Instruction*> AsignmentOpQ::gen(){
  
  bool anyConstint= (this.arg1Type== constint) ||  this.arg2Type== constint;
  bool anyConstFloat= (this.arg1Type== constfloat) || this.arg2.Type==constFloat;
  bool anyConst= anyConstint || anyContfloat;
  bool arg2Null= this.arg2Type==null;
  bool isFloat= this->result->getType()== &(FloatType::getInstance());
  RegSet regs;
  Reg Rd,Rx,Ry;

  /* Pedir registros a getReg dependiendo del numero de operandos
   * si hay operandos constantes (inmediatos) no se toman de getReg, en vez 
   * de eso usamos registros reservados para el uso del compilador Devanix*/
  if (anyConst && !arg2Null){
    Args argId= (arg1Type==id) ? this->arg1 : this->arg2;
    regs= get2RegAs(result,argId.id,anyConstFloat);
 
    // Cargar segundo operando solo cuando el operador 
    // no sea suma o resta de enteros (para aprovechar addi subi)
    if( this.op!= sumI && this.op!=substractionI){
      Ry = (this->arg2Type==id)?regs.Ry: $a1;
      loadVar(this->arg2,this->arg2Type,Ry);
      // Actualizar descriptores si no fue $a1
      if(Rx!=$a1){
	clearReg(Ry);
	addLocation(Ry,this.arg1.id);
      }
    } else if((this.op== sumI || this.op==substractionI)&& arg2Type==id){
      Ry = regs.Ry;
      loadVar(this->arg2.id,Ry);
      // Actualizar descriptores
      clearReg(Ry);
      addLocation(Ry,this.arg1.id);
    }

  }else if( anyConst && arg2Null){
    regs= get1Reg(result,anyConstFloat);
  }else if( !anyConst && !arg2Null){
    regs= get3RegAs(this->result,this->arg1.id,this->arg2.id,isFloat);
    Ry = regs.Rz;
    loadVar(this->arg2.id,Ry);
    // Actualizar descriptores
    clearReg(Ry);
    addLocation(Ry,this.arg2.id);
  }else if(!anyConst && arg2Null){
    regs= get2RegAs(this->result,this->arg1.id,isFloat);
  }

  // Cargar primer argumento en Rx
  Rx = (this.arg1Type==id)?regs.Ry: $a0;
  loadVar(this.arg1,this->arg1Type,Rx);
  // Actualizar descriptores si no fue $a0
  if(Rx!=$a0){
    clearReg(Rx);
    addLocation(Rx,this.arg1.id);
  }

  Rd= regs.Rx;
  // En cualquier caso result solo estara en Rd
  addExclusiveLocation(Rd,result);
 
  Instruction* instructionOp;

  switch(this.op){
  case Operator::sumI:
    // Si el segundo es una constante usar Addi
    if(this->arg2Type==constint){
      instructionOp= new Addi(Rd,Rx,this->arg2.constint);
    }else{
      instructionOp= new Add(Rd,Rx,Ry);
    }
  case Operator::sumF:
    instructionOp= new AddS(Rd,Rx,Ry);
  case Operator::substractionI:
    if(this->arg2Type==constint){
      instructionOp= new Subi(Rd,Rx,this->arg2.constint);
    }else{
      instructionOp= new Sub(Rd,Rx,Ry);
    }    
  case Operator::substractionF:
     instructionOp= new SubS(Rd,Rx,Ry);
  case Operator::multiplicationI:
     instructionOp= new Mul(Rd,Rx,Ry);
  case Operator::multiplicationF:
     instructionOp= new MulS(Rd,Rx,Ry);
  case Operator::divisionI:
     instructionOp= new Div(Rd,Rx,Ry);
  case Operator::divisionF:
     instructionOp= new DivS(Rd,Rx,Ry);
  case Operator::remainder:
     instructionOp= new Rem(Rd,Rx,Ry);
  case Operator::minusI:
     instructionOp= new Neg(Rd,Rx);
  case Operator::minusF:
     instructionOp= new NegS(Rd,Rx,Ry);
  }

  return reg.stores++instruction;
}


bool isFloat(SymVar* s) {
  return s->getType() == &(FloatType::getInstance());
}

std::list<Instruction*> AsignmentQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  switch (arg1Type) {

  case ArgType::id:
    r = rdesc.get1Reg(arg1.id, isFloat(arg1.id));
    l.splice(l.end(), r.stores);
    if (! arg1.id->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, arg1.id) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, arg1.id);
    }
    rdesc.addExclusiveLocation(r.rx, result);
    break;

  case ArgType::constint:
    r = rdesc.getFreshReg(false);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back(new Li(r.rx, arg1.constint));
    rdesc.addExclusiveLocation(r.rx, result);
    break;

  case ArgType::constfloat:
    r = rdesc.getFreshReg(true);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back(new LiS(r.rx, arg1.constfloat));
    rdesc.addExclusiveLocation(r.rx, result);
    break;

  case ArgType::constbool:
    r = rdesc.getFreshReg(false);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back(new Li(r.rx, (int) arg1.constbool));
    rdesc.addExclusiveLocation(r.rx, result);
    break;

  case ArgType::conststring:
    r = rdesc.getFreshReg(false);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back(new La(r.rx, mipscode.emitString(*arg1.conststring)));
    rdesc.addExclusiveLocation(r.rx, result);
    break;

  case ArgType::constchar:
    r = rdesc.getFreshReg(false);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back(new Li(r.rx, (int) arg1.constchar));
    rdesc.addExclusiveLocation(r.rx, result);
    break;
  }

  return l;
}

std::list<Instruction*> ConditionalJumpQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  bool dofloat = false;

  // Fuck my life

  return l;
}

std::list<Instruction*> AsignmentPointQ::gen() {
  std::list<Instruction*> l;

  // WARN: no sé si puedo asegurar que arg1 no sea temporal
  // Si lo fuera, isFloat() dará falso y podrían ocurrir cosas inesperadas

  if ( isFloat(arg1) ) {

    RegSet rop = rdesc.get1Reg(arg1, false);
    l.splice(l.end(), rop.stores);
    if (! arg1->isInReg(rop.rx) ) {
      l.push_back( rdesc.loadVar(rop.rx, arg1) );
      rdesc.clearReg(rop.rx);
      rdesc.addLocation(rop.rx, arg1);
    }

    RegSet rr = rdesc.getFreshReg( true );
    l.splice(l.end(), rr.stores);
    rdesc.clearReg(rr.rx);
    l.push_back(new LwS(rr.rx, 0, rop.rx));
    rdesc.addExclusiveLocation(rr.rx, result);

  } else {

    RegSet r = rdesc.getFreshReg(false);
    l.splice(l.end(), r.stores);
    rdesc.clearReg(r.rx);
    l.push_back( rdesc.loadVar(r.rx, arg1) );
    l.push_back( new Lw(r.rx, 0, r.rx) );
    rdesc.addExclusiveLocation(r.rx, result);

  }

  return l;
}

std::list<Instruction*> JumpQ::gen() {
  std::list<Instruction*> res;
  res.push_back(new J(this->label));
  return res;
}

// FIXME
std::list<Instruction*> CallQ::gen() {
  std::list<Instruction*> res;
  res.push_back(new Jal(func->getLabel()));
  // Hacer que se devuelva el $sp tantos bytes como argumentos
  // de la función
  return res;
}

std::list<Instruction*> WriteQ::gen() {
  std::list<Instruction*> l;

  switch (argt) {
  case ArgType::id:
    if ( type == &(FloatType::getInstance()) ) {
      // Print float
      l.push_back( rdesc.loadVar(Reg::f12, arg.id) );
      l.push_back( new Li(Reg::v0, 2) );

    } else if (type == &(IntType::getInstance())) {
      // Print int
      l.push_back( rdesc.loadVar(Reg::a0, arg.id) );
      l.push_back( new Li(Reg::v0, 1) );

    } else if (type == &(BoolType::getInstance())) {
      // Print bool
      l.push_back( rdesc.loadVar(Reg::v0, arg.id) );
      l.push_back( new La(Reg::a0, new Label("true")) );
      l.push_back( new La(Reg::a1, new Label("false")) );
      l.push_back( new Movz(Reg::v0, Reg::a1, Reg::a0) );
      l.push_back( new Li(Reg::v0, 4) );

    } else {
      // Print string
      l.push_back( rdesc.loadVar(Reg::a0, arg.id) );
      l.push_back( new Li(Reg::v0, 4) );

    }

    l.push_back( new Syscall() );

    break;

  case ArgType::constint:
    l.push_back( new Li(Reg::a0, arg.constint) );
    l.push_back( new Li(Reg::v0, 1) );
    l.push_back( new Syscall() );
    break;

  case ArgType::constfloat:
    l.push_back( new LiS(Reg::f12, arg.constfloat) );
    l.push_back( new Li(Reg::v0, 2) );
    l.push_back( new Syscall() );
    break;

  case ArgType::constbool:
    l.push_back( new Li(Reg::v0, (int) arg.constbool) );
    l.push_back( new La(Reg::a0, new Label("true")) );
    l.push_back( new La(Reg::a1, new Label("false")) );
    l.push_back( new Movz(Reg::v0, Reg::a1, Reg::a0) );
    l.push_back( new Li(Reg::v0, 4) );
    l.push_back( new Syscall() );
    break;

  case ArgType::conststring:
    l.push_back( new La(Reg::a0, mipscode.emitString(*arg.conststring)) );
    l.push_back( new Li(Reg::v0, 4) );
    l.push_back( new Syscall() );
    break;

  }

  if (isLn) {
    l.push_back( new La(Reg::a0, new Label("newline")) );
    l.push_back( new Li(Reg::v0, 4) );
    l.push_back( new Syscall() );
  }

  return l;
}
