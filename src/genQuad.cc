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
 **//*
std::list<MIPSinstruction*> AsignmentOpQ::gen(){
  // Ver que registros convienen "getReg" para x, y 
  // Si arg2 no en null ver que registro le conviene

  // Si y(no es una constant) y no pertenece a Assig(Ry) 
    // Buscar el y mas econom en Av(y)
    // lista.add(new Lw(Ry,y)) 
  // Si es una constante lista.add(new Li(Ry,y))

  // Lo mismo para z en caso de no ser null
  // Pero si usaremos addi y es const no hace falta colocarla en reg 
  // lista.add(new Ld(Rz,z))
  /*
  // Dependiendo del operador se gen la instruccion  
  switch(this.op){
  case Operator::sumI:
    // Si el segundo es una constante usar Addi
    //lista.add(new Add(Rx,Ry,Rz));
  case Operator::sumF:
    
  case Operator::substractionI:
    
  case Operator::substractionF:
    
  case Operator::multiplicationI:
    
  case Operator::multiplicationF:
    
  case Operator::divisionI:
  case Operator::divisionF:
  case Operator::remainder:
  case Operator::minusI:
  case Operator::minusF:
  }
}
*/

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

  switch (op) {
  case Operator::greater:
  case Operator::greaterEq:
  case Operator::equal:
  case Operator::notEqual:
  case Operator::less:
  case Operator::lessEq:

    if (arg1Type == ArgType::id and arg2Type == ArgType::id) {

      r = rdesc.get2Reg(arg1.id, arg2.id, false);
      l.splice(l.end(), r.stores);
      if (! arg1.id->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, arg1.id) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, arg1.id);
      }
      if (! arg2.id->isInReg(r.ry) ){
	l.push_back( rdesc.loadVar(r.ry, arg2.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, arg2.id);
      }

    } else if (arg1Type == ArgType::constint) {
      r = rdesc.get1Reg(arg2.id, false);
      r.ry = r.rx;
      r.rx = Reg::a0;
      l.push_back( new Li(Reg::a0, arg1.constint) );
      l.splice(l.end(), r.stores);
      if (! arg2.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, arg2.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, arg2.id);
      }

    } else if (arg2Type == ArgType::constint) {

      r = rdesc.get1Reg(arg1.id, false);
      l.splice(l.end(), r.stores);
      if (! arg1.id->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, arg1.id) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, arg1.id);
      }
      r.ry = Reg::a0;
      l.push_back( new Li(Reg::a0, arg2.constint) );

    } else {
      r.rx = Reg::a0;
      r.ry = Reg::a1;
      l.push_back( new Li(Reg::a0, arg1.constint) );
      l.push_back( new Li(Reg::a1, arg2.constint) );
    }

    break;

  case Operator::greaterF:
  case Operator::greaterEqF:
  case Operator::equalF:
  case Operator::notEqualF:
  case Operator::lessF:
  case Operator::lessEqF:
    
    if (arg1Type == ArgType::id and arg2Type == ArgType::id) {

      r = rdesc.get2Reg(arg1.id, arg2.id, true);
      l.splice(l.end(), r.stores);
      if (! arg1.id->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, arg1.id) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, arg1.id);
      }
      if (! arg2.id->isInReg(r.ry) ){
	l.push_back( rdesc.loadVar(r.ry, arg2.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, arg2.id);
      }

    } else if (arg1Type == ArgType::constfloat) {
      r = rdesc.get1Reg(arg2.id, true);
      r.ry = r.rx;
      r.rx = Reg::f0;
      l.push_back( new LiS(Reg::f0, arg1.constfloat) );
      l.splice(l.end(), r.stores);
      if (! arg2.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, arg2.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, arg2.id);
      }

    } else if (arg2Type == ArgType::constfloat) {

      r = rdesc.get1Reg(arg1.id, true);
      l.splice(l.end(), r.stores);
      if (! arg1.id->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, arg1.id) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, arg1.id);
      }
      r.ry = Reg::f0;
      l.push_back( new LiS(Reg::f0, arg2.constfloat) );

    } else {
      r.rx = Reg::f0;
      r.ry = Reg::f12;
      l.push_back( new LiS(Reg::f0, arg1.constfloat) );
      l.push_back( new LiS(Reg::f12, arg2.constfloat) );
    }

    break;

  }

  switch (op) {

  case less:
    l.push_back( new Blt(r.rx, r.ry, label) );
    break;
  case lessEq:
    l.push_back( new Ble(r.rx, r.ry, label) );
    break;
  case equal:
    l.push_back( new Beq(r.rx, r.ry, label) );
    break;
  case notEqual:
    l.push_back( new Bne(r.rx, r.ry, label) );
    break;
  case greater:
    l.push_back( new Bgt(r.rx, r.ry, label) );
    break;
  case greaterEq:
    l.push_back( new Bge(r.rx, r.ry, label) );
    break;

  case lessF:
    l.push_back( new CompLS(r.rx, r.ry) );
    l.push_back( new Bclt(label) );
    break;
  case lessEqF:
    l.push_back( new CompLTES(r.rx, r.ry) );
    l.push_back( new Bclt(label) );
    break;
  case equalF:
    l.push_back( new CompES(r.rx, r.ry) );
    l.push_back( new Bclt(label) );
    break;
  case notEqualF:
    l.push_back( new CompES(r.rx, r.ry) );
    l.push_back( new Bclf(label) );
    break;
  case greaterF:
    l.push_back( new CompLTES(r.rx, r.ry) );
    l.push_back( new Bclf(label) );
    break;
  case greaterEqF:
    l.push_back( new CompLS(r.rx, r.ry) );
    l.push_back( new Bclf(label) );
    break;

  }


  return l;
}

Operator negateOp(Operator op) {
  switch (op) {
  case greater:
    return lessEq;
  case greaterEq:
    return less;
  case equal:
    return notEqual;
  case notEqual:
    return equal;
  case less:
    return greaterEq;
  case lessEq:
    return greater;

  case greaterF:
    return lessEqF;
  case greaterEqF:
    return lessF;
  case equalF:
    return notEqualF;
  case notEqualF:
    return equalF;
  case lessF:
    return greaterEqF;
  case lessEqF:
    return greaterF;

  }
}

std::list<Instruction*> ConditionalNJumpQ::gen() {
  ConditionalJumpQ j(arg1Type, arg1, negateOp(op), arg2Type, arg2, label);
  return j.gen();
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
