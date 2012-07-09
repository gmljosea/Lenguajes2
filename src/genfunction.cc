#include "IntermCode.hh"
#include "mipscode.hh"
#include "Quad.hh"
#include "statement.hh"
#include "symbol.hh"
#include "type.hh"
#include "parser.hh"
#include "program.hh"

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

  // FIXME
  // Escupir el prólogo (incluyendo su label)
  // -- Instanciar una pila de MIPS y hacer emitInst
  /*
    sw $fp, -4($sp)
    sw $ra, -8($sp)
    la $fp, 0($sp)
    la $sp -n($sp)
    --> n offset de la funcion
    (tamaño total de las locales)
   */

  // Escupir el MIPS generado
  fun_graph->emitCode();

  mipscode.emitLabel(fun_graph->getExit()->getLabel());

  // FIXME
  // Escupir el epílogo (incluyendo su label)
  // -- Instanciar una pila de MIPS y hacer emitInst
  /*
    la $sp, 0($fp)
    lw $ra, -8($sp)
    lw $fp, -4($sp)
    jr $ra
   */
}
