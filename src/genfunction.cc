#include "IntermCode.hh"
#include "mipscode.hh"
#include "Quad.hh"
#include "statement.hh"
#include "symbol.hh"
#include "type.hh"
#include "parser.hh"
#include "program.hh"
#include "registers.hh"
#include "MIPSinstruction.hh"

#include <iostream>

extern IntermCode intCode;
extern MIPSCode mipscode;
extern SymFunction* currentfun;
extern Program program;

void SymFunction::gen(){
  if (generated) return;

  this->generated = true;
  currentfun = this;

  // Generar TAC, como siempre
  this->block->gen(NULL);

  // Extraer TAC
  std::list<Instruction*> fun_tac = intCode.getInstructions();
  intCode.clear();
  FlowGraph* fun_graph = new FlowGraph(fun_tac, std::string("f_")+id+std::string("_"));

  this->epilogue = fun_graph->getExit()->getLabel();

  mipscode.emitComment(std::string("Función ")+this->id);
  mipscode.emitComment(std::string("Espacio en pila: ")+
		       std::to_string((long long int)this->local_space)
		       +std::string(" bytes"));

  mipscode.emitData();

  fun_graph->output(std::string("tac_"));

  //  fun_graph->optimize()
  fun_graph->toMIPS();

  fun_graph->output(std::string("mips_"));

  mipscode.emitText();

  // Emitir label donde comienza esta función
  mipscode.emitLabel(this->start);

  // Prólogo
  mipscode.emitInst(new Sw(Reg::fp, -4, Reg::sp));
  mipscode.emitInst(new Sw(Reg::ra, -8, Reg::sp));
  mipscode.emitInst(new La(Reg::fp, 0, Reg::sp));
  mipscode.emitInst(new La(Reg::sp, -local_space-8, Reg::sp));

  /*
    sw $fp, -4($sp)
    sw $ra, -8($sp)
    la $fp, 0($sp)
    la $sp -local_space($sp)
   */

  // Escupir el MIPS generado
  fun_graph->emitCode();

  mipscode.emitLabel(fun_graph->getExit()->getLabel());

  // Epílogo
  mipscode.emitInst(new La(Reg::sp, 0, Reg::fp));
  mipscode.emitInst(new Lw(Reg::ra, -8, Reg::sp));
  mipscode.emitInst(new Lw(Reg::fp, -4, Reg::sp));
  mipscode.emitInst(new Jr(Reg::ra));

  /*
    la $sp, 0($fp)
    lw $ra, -8($sp)
    lw $fp, -4($sp)
    jr $ra
  */
}
