// Metodos gen de los Quads 
#include <list>
#include "Quad.hh"
#include "MIPSinstruction.hh"
#include "mipscode.hh"
#include "regdesc.hh"
#include "registers.hh"

extern MIPSCode mipscode;
extern RegDesc rdesc;


/** 
 * Asignacion de 3 o 2 direcciones
 * x:= y op z  =>  Rd:= Rx op Ry  
 * x:= op y    =>  Rd:= op Rx
 **/
std::list<Instruction*> AsignmentOpQ::gen(){
  std::list<Instruction*> l;
 
  bool anyConstint= (this->arg1Type== constint) || this->arg2Type== constint;
  bool anyConstFloat= (this->arg1Type== constfloat) || this->arg2Type==constfloat;
  bool anyConst= anyConstint || anyConstFloat;
  bool arg2Null= this->arg2Type==null;
  bool isFloat= this->result->getType()== &(FloatType::getInstance());
  RegSet regs;
  Reg Rd,Rx,Ry;

  /* Pedir registros a getReg dependiendo del numero de operandos
   * si hay operandos constantes (inmediatos) no se toman de getReg, en vez 
   * de eso usamos registros reservados para el uso del compilador Devanix*/
  if (anyConst && !arg2Null){
    Args argId= (arg1Type==id) ? this->arg1 : this->arg2;
    regs= rdesc.get2RegAs(result,argId.id,anyConstFloat);
    // Agregar los stores devueltos por getReg()
    l.splice(l.end(),regs.stores);
    // Cargar segundo operando solo cuando el operador 
    // no sea suma o resta de enteros (para aprovechar addi subi)
    if( this->op!= sumI && this->op!=substractionI){
      Ry = (this->arg2Type==id)?regs.ry: Reg::a1;
      l.push_back(rdesc.loadVar(this->arg2,this->arg2Type,Ry));
      // Actualizar descriptores si no fue $a1
      if(Rx!=Reg::a1){
	rdesc.clearReg(Ry);
	rdesc.addLocation(Ry,this->arg1.id);
      }
    } else if((this->op== sumI || this->op==substractionI)&& arg2Type==id){
      Ry = regs.ry;
      l.push_back(rdesc.loadVar(Ry,this->arg2.id));
      // Actualizar descriptores
      rdesc.clearReg(Ry);
      rdesc.addLocation(Ry,this->arg1.id);
    }

  }else if( anyConst && arg2Null){
    regs= rdesc.get1Reg(result,anyConstFloat);
    l.splice(l.end(),regs.stores);
  }else if( !anyConst && !arg2Null){
    regs= rdesc.get3RegAs(this->result,this->arg1.id,this->arg2.id,isFloat);
    l.splice(l.end(),regs.stores);
    Ry = regs.rz;
    l.push_back(rdesc.loadVar(Ry,this->arg2.id));
    // Actualizar descriptores
    rdesc.clearReg(Ry);
    rdesc.addLocation(Ry,this->arg2.id);
  }else if(!anyConst && arg2Null){
    regs= rdesc.get2RegAs(this->result,this->arg1.id,isFloat);
    l.splice(l.end(),regs.stores);
  }

  // Cargar primer argumento en Rx
  Rx = (this->arg1Type==id)?regs.ry: Reg::a0;
  l.push_back(rdesc.loadVar(this->arg1,this->arg1Type,Rx));
  // Actualizar descriptores si no fue $a0
  if(Rx!=Reg::a0){
    rdesc.clearReg(Rx);
    rdesc.addLocation(Rx,this->arg1.id);
  }

  Rd= regs.rx;
  // En cualquier caso result solo estara en Rd
  rdesc.addExclusiveLocation(Rd,result);
 
  Instruction* instructionOp;

  switch(this->op){
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
     instructionOp= new Negu(Rd,Rx);
  case Operator::minusF:
     instructionOp= new NegS(Rd,Rx);
  }
  l.push_back(instructionOp);
  return l;
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

std::list<Instruction*> AsignmentPointQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  // Chequear si el type del resultado no es null. Pudiera serlo si
  // es un temporal, en cuyo caso asumiré que es entero.
  if ( result->getType() and isFloat(result) ) {
    // Dereferenciar apuntador a flotante
    r = rdesc.get1Reg(arg1, false);
    l.splice(l.end(), r.stores);
    if (! arg1->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, arg1) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, arg1);
    }

    RegSet rf = rdesc.getFreshReg(true);
    l.splice(l.end(), rf.stores);
    rdesc.clearReg(rf.rx);
    l.push_back( new LwS(rf.rx, 0, r.rx) );
    rdesc.addExclusiveLocation(rf.rx, result);

  } else {
    // Dereferenciar apuntador a entero
    r = rdesc.get2RegAs(result, arg1, false);
    l.splice(l.end(), r.stores);
    if (! arg1->isInReg(r.ry) ) {
      l.push_back( rdesc.loadVar(r.ry, arg1) );
      rdesc.clearReg(r.ry);
      rdesc.addLocation(r.ry, arg1);
    }
    rdesc.clearReg(r.rx);
    l.push_back( new Lw(r.rx, 0, r.ry) );
    rdesc.addExclusiveLocation(r.rx, result);

  }

  return l;
}

std::list<Instruction*> AsignmentToPointQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  switch (arg1Type) {
  case ArgType::id:

    if ( result->getType() and isFloat(result) ) {
      r = rdesc.get1Reg(result, false);
      l.splice(l.end(), r.stores);
      if (! result->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, result) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, result);
      }

      RegSet rf = rdesc.getFreshReg(true);
      l.splice(l.end(), rf.stores);
      if (! arg1.id->isInReg(rf.rx) ) {
	l.push_back( rdesc.loadVar(rf.rx, arg1.id) );
	rdesc.clearReg(rf.rx);
	rdesc.addLocation(rf.rx, arg1.id);
      }

      l.push_back( new SwS(rf.rx, 0, r.rx) );

    } else {
      r = rdesc.get2Reg(result, arg1.id, false);
      l.splice(l.end(), r.stores);
      if (! result->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, result) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, result);
      }
      if (! arg1.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, arg1.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, arg1.id);
      }

      l.push_back( new Sw(r.rx, 0, r.ry) );

    }

    break;

  case ArgType::constint:

    r = rdesc.get1Reg(result, false);
    l.splice(l.end(), r.stores);
    if (! result->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, result) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, result);
    }
    l.push_back( new Li(Reg::a0, arg1.constint) );
    l.push_back( new Sw(Reg::a0, 0, r.rx) );

    break;

  case ArgType::constfloat:

    r = rdesc.get1Reg(result, false);
    l.splice(l.end(), r.stores);
    if (! result->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, result) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, result);
    }
    l.push_back( new LiS(Reg::f0, arg1.constfloat) );
    l.push_back( new SwS(Reg::f0, 0, r.rx) );

    break;

  case ArgType::constchar:

    r = rdesc.get1Reg(result, false);
    l.splice(l.end(), r.stores);
    if (! result->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, result) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, result);
    }
    l.push_back( new Li(Reg::a0, (int) arg1.constchar) );
    l.push_back( new Sw(Reg::a0, 0, r.rx) );

    break;

  case ArgType::constbool:

    r = rdesc.get1Reg(result, false);
    l.splice(l.end(), r.stores);
    if (! result->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, result) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, result);
    }
    l.push_back( new Li(Reg::a0, (int) arg1.constbool) );
    l.push_back( new Sw(Reg::a0, 0, r.rx) );

    break;


  }

  return l;
}

std::list<Instruction*> IndexQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  ArrayType* at = dynamic_cast<ArrayType*>(array->getType());

  if (at->getBaseType() == &(FloatType::getInstance())) {
    if (indexType == ArgType::id) {
      r = rdesc.get2Reg(array, index.id, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }
      if (! index.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, index.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, index.id);
      }
      RegSet rf = rdesc.getFreshReg(true);
      l.splice(l.end(), rf.stores);
      rdesc.clearReg(rf.rx);

      l.push_back( new Add(Reg::a0, r.rx, r.ry) );
      l.push_back( new LwS(rf.rx, 0, Reg::a0) );

      rdesc.addExclusiveLocation(rf.rx, result);

    } else {

      r = rdesc.get1Reg(array, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }

      RegSet rf = rdesc.getFreshReg(true);
      l.splice(l.end(), rf.stores);
      rdesc.clearReg(rf.rx);

      l.push_back( new LwS(rf.rx, index.constint, r.rx) );

      rdesc.addExclusiveLocation(rf.rx, result);

    }
  } else {
    if (indexType == ArgType::id) {
      r = rdesc.get3RegAs(result, array, index.id, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, array) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, array);
      }
      if (! index.id->isInReg(r.rz) ) {
	l.push_back( rdesc.loadVar(r.rz, index.id) );
	rdesc.clearReg(r.rz);
	rdesc.addLocation(r.rz, index.id);
      }

      l.push_back( new Add(Reg::a0, r.ry, r.rz) );
      l.push_back( new Lw(r.rx, 0, Reg::a0) );

      rdesc.addExclusiveLocation(r.rx, result);

    } else {
      r = rdesc.get2RegAs(result, array, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, array) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, array);
      }

      l.push_back( new Lw(r.rx, index.constint, r.ry) );

      rdesc.addExclusiveLocation(r.rx, result);

    }
  }

  return l;
}

std::list<Instruction*> IndexAsigQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  ArrayType* at = dynamic_cast<ArrayType*>(array->getType());
  bool isf = at->getBaseType() == &(FloatType::getInstance());

  if (indexType == ArgType::id) {
    // Indice en variable, hay que cargarlo y sumarlo a la base
    switch (argType) {
    case ArgType::id:
      if (isf) {
	// Indice variable, valor en variable y flotante
	r = rdesc.get2Reg(array, index.id, false);
	l.splice(l.end(), r.stores);
	if (! array->isInReg(r.rx) ) {
	  l.push_back( rdesc.loadVar(r.rx, array) );
	  rdesc.clearReg(r.rx);
	  rdesc.addLocation(r.rx, array);
	}
	if (! index.id->isInReg(r.ry) ) {
	  l.push_back( rdesc.loadVar(r.ry, index.id) );
	  rdesc.clearReg(r.ry);
	  rdesc.addLocation(r.ry, index.id);
	}

	RegSet rf = rdesc.get1Reg(arg.id, true);
	l.splice(l.end(), rf.stores);
	if (! arg.id->isInReg(rf.rx) ) {
	  l.push_back( rdesc.loadVar(rf.rx, arg.id) );
	  rdesc.clearReg(rf.rx);
	  rdesc.addLocation(rf.rx, arg.id);
	}
	l.push_back( new Add(Reg::a0, r.rx, r.ry) );
	l.push_back( new SwS(rf.rx, 0, Reg::a0) );

      } else {

	r = rdesc.get3Reg(array, index.id, arg.id, false);
	l.splice(l.end(), r.stores);
	if (! array->isInReg(r.rx) ) {
	  l.push_back( rdesc.loadVar(r.rx, array) );
	  rdesc.clearReg(r.rx);
	  rdesc.addLocation(r.rx, array);
	}
	if (! index.id->isInReg(r.ry) ) {
	  l.push_back( rdesc.loadVar(r.ry, index.id) );
	  rdesc.clearReg(r.ry);
	  rdesc.addLocation(r.ry, index.id);
	}
	if (! arg.id->isInReg(r.rz) ) {
	  l.push_back( rdesc.loadVar(r.rz, arg.id) );
	  rdesc.clearReg(r.rz);
	  rdesc.addLocation(r.rz, arg.id);
	}

	l.push_back( new Add(Reg::a0, r.rx, r.ry) );
	l.push_back( new Sw(r.rz, 0, Reg::a0) );

      }

      break;

    case ArgType::constint:
    case ArgType::constchar:
    case ArgType::constbool:

      r = rdesc.get2Reg(array, index.id, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }
      if (! index.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, index.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, index.id);
      }

      // creo que puedo hacer esto boletamente igual para bool y char
      l.push_back( new Add(Reg::a0, r.rx, r.ry) );
      l.push_back( new Li(Reg::a1, arg.constint) );
      l.push_back( new Sw(Reg::a1, 0, Reg::a0) );

      break;

    case ArgType::constfloat:

      r = rdesc.get2Reg(array, index.id, false);
      l.splice(l.end(), r.stores);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }
      if (! index.id->isInReg(r.ry) ) {
	l.push_back( rdesc.loadVar(r.ry, index.id) );
	rdesc.clearReg(r.ry);
	rdesc.addLocation(r.ry, index.id);
      }

      l.push_back( new Add(Reg::a0, r.rx, r.ry) );
      l.push_back( new LiS(Reg::f0, arg.constfloat) );
      l.push_back( new SwS(Reg::f0, 0, Reg::a0) );

      break;

    }

  } else {
    // Indice constante, se puede usar directo en el load
    switch (argType) {
    case ArgType::id:
      if (isf) {
	r = rdesc.get1Reg(array, false);
	l.splice(l.end(), r.stores);
	if (! array->isInReg(r.rx) ) {
	  l.push_back( rdesc.loadVar(r.rx, array) );
	  rdesc.clearReg(r.rx);
	  rdesc.addLocation(r.rx, array);
	}
	RegSet rf = rdesc.get1Reg(arg.id, true);
	l.splice(l.end(), r.stores);
	if (! arg.id->isInReg(rf.rx) ) {
	  l.push_back( rdesc.loadVar(rf.rx, arg.id) );
	  rdesc.clearReg(rf.rx);
	  rdesc.addLocation(rf.rx, arg.id);
	}

	l.push_back( new SwS(rf.rx, index.constint, r.rx) );

      } else {
	r = rdesc.get2Reg(array, arg.id, false);
	l.splice(l.end(), r.stores);
	if (! array->isInReg(r.rx) ) {
	  l.push_back( rdesc.loadVar(r.rx, array) );
	  rdesc.clearReg(r.rx);
	  rdesc.addLocation(r.rx, array);
	}
	if (! arg.id->isInReg(r.ry) ) {
	  l.push_back( rdesc.loadVar(r.ry, arg.id) );
	  rdesc.clearReg(r.ry);
	  rdesc.addLocation(r.ry, arg.id);
	}

	l.push_back( new Sw(r.ry, index.constint, r.rx) );
      }

      break;

    case ArgType::constint:
    case ArgType::constchar:
    case ArgType::constbool:

      r = rdesc.get1Reg(array, false);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }

      l.push_back( new Li(Reg::a0, arg.constint) );
      l.push_back( new Sw(Reg::a0, index.constint, r.rx) );

      break;

    case ArgType::constfloat:

      r = rdesc.get1Reg(array, false);
      if (! array->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, array) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, array);
      }

      l.push_back( new LiS(Reg::f0, arg.constfloat) );
      l.push_back( new SwS(Reg::f0, index.constint, r.rx) );

      break;

    }
  }

  return l;
}

std::list<Instruction*> AsignmentAddQ::gen() {
  std::list<Instruction*> l;

  // Las referencias y direcciones de memoria siempre están en los
  // registros normales para enteros
  RegSet r = rdesc.getFreshReg(false);
  l.splice(l.end(), r.stores);
  rdesc.clearReg(r.rx);

  if ( arg1->isGlobal() ) {
    l.push_back( new La(r.rx, arg1->getLabel()) );
  } else {
    l.push_back( new La(r.rx, arg1->getOffset(), Reg::fp) );
  }

  rdesc.addExclusiveLocation(r.rx, result);

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

std::list<Instruction*> ParamValQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  switch (paramType) {
  case ArgType::id:
    r = rdesc.get1Reg(param.id, isFloat(param.id));
    l.splice(l.end(), r.stores);
    if (! param.id->isInReg(r.rx) ) {
      l.push_back( rdesc.loadVar(r.rx, param.id) );
      rdesc.clearReg(r.rx);
      rdesc.addLocation(r.rx, param.id);
    }
    l.push_back( new Sw(r.rx, -4, Reg::sp) );
    l.push_back( new La(Reg::sp, -4, Reg::sp) );
    break;

  case ArgType::constint:
  case ArgType::constchar:
  case ArgType::constbool:
    l.push_back( new Li(Reg::a0, param.constint) );
    l.push_back( new Sw(Reg::a0, -4, Reg::sp) );
    l.push_back( new La(Reg::sp, -4, Reg::sp) );
    break;

  case ArgType::constfloat:
    l.push_back( new LiS(Reg::f0, param.constfloat) );
    l.push_back( new SwS(Reg::f0, -4, Reg::sp) );
    l.push_back( new La(Reg::sp, -4, Reg::sp) );
    break;
  }

  return l;
}

std::list<Instruction*> ParamRefQ::gen() {
  std::list<Instruction*> l;
  RegSet r;

  ArrayType* at = dynamic_cast<ArrayType*>(param->getType());
  if (at) {
    if (param->isReference()) {
      // Si el parámetro ya era una referencia a arreglo
      // Copiamos en la pila su valor, pues ya es la dirección
      // Si abajo copiamos el tamaño que se pasó en nuestro 'dope vector'
      r = rdesc.get1Reg(param, false);
      l.splice(l.end(), r.stores);
      if (! param->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, param) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, param);
      }
      l.push_back( new Sw(r.rx, -4, Reg::sp) );
      l.push_back( new Lw(Reg::a0, param->getOffset()-4, Reg::fp) );
      l.push_back( new Sw(Reg::a0, -8, Reg::sp) );
      l.push_back( new La(Reg::sp, -8, Reg::sp) );
    } else {
      // Si el parámetro no es referencia
      // Se le calcula su dirección y se empila
      // Su tamaño se puede sacar ya mismo a tiempo de compilación
      if (param->isGlobal()) {
	l.push_back( new La(Reg::a0, param->getLabel()) );
      } else {
	l.push_back( new La(Reg::a0, param->getOffset(), Reg::fp) );
      }
      l.push_back( new Sw(Reg::a0, -4, Reg::sp) );
      l.push_back( new Li(Reg::a0, at->getLength()) );
      l.push_back( new Sw(Reg::a0, -8, Reg::sp) );
      l.push_back( new La(Reg::sp, -8, Reg::sp) );
    }

  } else {
    if (param->isReference()) {
      r = rdesc.get1Reg(param, false);
      l.splice(l.end(), r.stores);
      if (! param->isInReg(r.rx) ) {
	l.push_back( rdesc.loadVar(r.rx, param) );
	rdesc.clearReg(r.rx);
	rdesc.addLocation(r.rx, param);
      }
      l.push_back( new Sw(r.rx, -4, Reg::sp) );
      l.push_back( new La(Reg::sp, -4, Reg::sp) );
    } else {
      if (param->isTemp() and !param->isInMem()) {
	l.push_back( rdesc.storeVar(param->getLocation(), param) );
	param->inMem(true);
      }
      if (param->isGlobal()) {
	l.push_back( new La(Reg::a0, param->getLabel()) );
      } else {
	l.push_back( new La(Reg::a0, param->getOffset(), Reg::fp) );
      }

      l.push_back( new Sw(Reg::a0, -4, Reg::sp) );
      l.push_back( new La(Reg::sp, -4, Reg::sp) );

    }

  }

  return l;
}

std::list<Instruction*> RetrieveQ::gen() {
  std::list<Instruction*> l;
  RegSet r = rdesc.getFreshReg(isFloat(var));
  l.splice(l.end(), r.stores);
  rdesc.clearReg(r.rx);

  if (isFloat(var)) {
    l.push_back( new MoveS(r.rx, Reg::f1) );
  } else {
    l.push_back( new Move(r.rx, Reg::v0) );
  }

  rdesc.addExclusiveLocation(r.rx, var);

  return l;
}

std::list<Instruction*> ReturnQ::gen() {
  std::list<Instruction*> l;

  switch (argt) {
  case ArgType::id:
    if (isFloat(arg.id)) {
      l.push_back( rdesc.loadVar(Reg::f1, arg.id) );
    } else {
      l.push_back( rdesc.loadVar(Reg::v0, arg.id) );
    }
    break;
  case ArgType::constint:
  case ArgType::constbool:
  case ArgType::constchar:
    l.push_back( new Li(Reg::v0, arg.constint) );
    break;
  case ArgType::constfloat:
    l.push_back( new LiS(Reg::f0, arg.constfloat) );
    break;
  case ArgType::null:
    break;
  }

  l.push_back( new J(symf->getEpilogueLabel()) );
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
  res.push_back(new La(Reg::sp, func->getArgsSpace(), Reg::sp));
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
