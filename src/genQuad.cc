// Metodos gen de los Quads 
#include <list>
#include "Quad.hh"
#include "MIPSinstruction.hh"


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
