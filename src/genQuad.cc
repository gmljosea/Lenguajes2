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
