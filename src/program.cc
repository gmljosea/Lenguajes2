#include <iostream>

#include "IntermCode.hh"
#include "mipscode.hh"
#include "program.hh"
#include "symbol.hh"
#include "registers.hh"
#include "regdesc.hh"

extern IntermCode intCode;
extern MIPSCode mipscode;

extern std::map<Reg, std::set<SymVar*>> rints;
extern std::map<Reg, std::set<SymVar*>> rfloats;

Program::Program() {
  // Inicializar un Programa, inicialmente vacío y sin errores
  errorCount = 0;

  // Agregar los casts como funciones en la tabla de símbolos
  // para beneficiarse del chequeo de tipos.
  SymVar *argInt   = new SymVar("a",0,0,true,0);
  SymVar *argFloat = new SymVar("b",0,0,true,0);
  SymVar *argChar  = new SymVar("c",0,0,true,0);

  argInt->setType(&(IntType::getInstance()));
  argInt->setReference(false);
  argFloat->setType(&(FloatType::getInstance()));
  argFloat->setReference(false);
  argChar->setType(&(CharType::getInstance()));
  argChar->setReference(false);

  ArgList *listargInt = new ArgList();
  ArgList *listargFloat = new ArgList();
  ArgList *listargChar = new ArgList();

  listargInt->push_back(argInt);
  listargFloat->push_back(argFloat);
  listargChar->push_back(argChar);

  SymFunction *inttofloat
    = new SymFunction("inttofloat",listargInt, &(FloatType::getInstance()),0,0);
  SymFunction *floattoint
    = new SymFunction("floattoint",listargFloat, &(IntType::getInstance()),0,0);
  SymFunction *chartoint
    = new SymFunction("chartoint",listargChar, &(IntType::getInstance()), 0,0);
  SymFunction *inttochar
    = new SymFunction("inttochar", listargInt, &(CharType::getInstance()),0,0);

  symtable.insert(inttofloat);
  symtable.insert(floattoint);
  symtable.insert(chartoint);
  symtable.insert(inttochar);
}

void Program::error(std::string error, int line, int col) {
  this->errorCount++;
  std::cout << line << ":" << col << ": error: " << error << std::endl;
}

void Program::check(){

  // Chequear que existe int main()
  main = symtable.lookup_function("main");
  if (main == NULL) {
    this->errorCount++;
    std::cerr << "Error: No se ha definido la función main." << std::endl;
  } else {
    int line = main->getLine();
    int col  = main->getColumn();
    // Si existe, verificar que no tenga agumentos y que sea tipo int
    if (main->getArgumentCount() != 0) {
      error("la funcion main no debe tener argumentos",line,col);
    }
    IntType& i = IntType::getInstance();
    if(!(*(main->getType()) == i)){
      error("La funcion main debe ser de tipo 'int'",line,col);
    }
  }

  /* Chequear que todos los boxes usados han sido correctamente definidos
   * y que no existen ciclos en sus declarariones */
  for (std::list<BoxType*>::iterator BoxIt= this->boxes.begin();
       BoxIt != this->boxes.end(); BoxIt++){
    (*BoxIt)->check();
  }

  if (this->errorCount==0){
    // Calcular offsets de los box
    this->calcOffsets();
  }

  this->offsetVarDec=0;
  // Chequear las variables gobales
  for(std::list<VariableDec*>::iterator varIt=this->globalinits.begin();
      varIt != this->globalinits.end(); varIt++){
    (*varIt)->check();
  }

  // Chequeo de las funciones
  for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
       funIt != this->functions.end(); funIt++){
    this->offsetVarDec=0;
    this->maxoffset = 0;
    (*funIt)->check();
    (*funIt)->setLocalSpace(maxoffset);
  }

}

/* Calcular offsets de los box
 * La precondicion es que todos los boxes esten completamente definidos
 * y que no existan ciclos en la definicion de los mismos.
 */
void Program::calcOffsets(){
  // Recorrer los box para calcular los offsets
  for (std::list<BoxType*>::iterator BoxIt= this->boxes.begin();
       BoxIt != this->boxes.end(); BoxIt++){
    if(!(*BoxIt)->areOffsetsDone()){
      (*BoxIt)->calcOffsets();
    }
  }

}

extern RegDesc rdesc;

typedef std::set<SymVar*> Tset;

void initRegDesc() {
  // Cargar descriptores de registros enteros disponibles
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::a0, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::a1, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::a2, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::a3, new Tset()));

  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::v1, new Tset()));

  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t0, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t1, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t2, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t3, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t4, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t5, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t6, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t7, new Tset()));

  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s0, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s1, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s2, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s3, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s4, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s5, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s6, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::s7, new Tset()));

  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t8, new Tset()));
  rdesc.rints.insert(std::pair<const Reg, Tset*>(Reg::t9, new Tset()));

  // Cargar descriptores de registros flotantes disponibles
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f0, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f1, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f2, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f3, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f4, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f5, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f6, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f7, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f8, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f9, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f10, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f11, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f12, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f13, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f14, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f15, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f16, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f17, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f18, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f19, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f20, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f21, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f22, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f23, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f24, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f25, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f26, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f27, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f28, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f29, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f30, new Tset()));
  rdesc.rfloats.insert(std::pair<const Reg, Tset*>(Reg::f31, new Tset()));
}

void Program::gen(){
  initRegDesc();

  // FIXME
  // Escupir runtime en la salida
  // Ridiculo, escupir un string a la salida y ya

  // Generar codigo para las variables gobales
  mipscode.emitData();

  for(std::list<VariableDec*>::iterator varIt = this->globalinits.begin();
      varIt != this->globalinits.end(); varIt++){
    (*varIt)->gen(NULL);
    std::list<SymVar*> t = (*varIt)->getVars();

    // Generar los .data para las variables globales
    for (std::list<SymVar*>::iterator it = t.begin();
	 it != t.end(); it++) {
      SymVar* var = *it;
      Label* l = mipscode.emitVar(var->getId(), var->getType()->getSize(),
				  var->getType()->getAlignment());
      // FIXME
      // Pegarle al SymVar el label generado
    }
  }

  // Agregar la instrucción call main
  intCode.addInst(new CallQ(main, 0));
  std::list<Instruction*> global_tac = intCode.getInstructions();
  intCode.clear();

  // Generar grafo y pasarlo a MIPS
  FlowGraph* global_graph = new FlowGraph(global_tac, std::string("init_"));
  // global_graph.optimize();
  global_graph->toMIPS(); // Como efecto de borde se emiten los .data de los
                         // strings que se consigan en el TAC

  // Colapsar el grafo a código en la salida estándar
  // Se emiten .text para indicar que viene código
  // y la etiqueta main: para indicar donde comienza el programa
  mipscode.emitText();
  mipscode.emitLabel(new Label(std::string("main")));
  global_graph->emitCode();

  // FIXME
  // Emitir código MIPS para terminar el programa
  // Esencialmente hacer syscall 10

  // Generar primero el main, y luego el resto de las funciones
  this->main->gen();
  for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
       funIt != this->functions.end(); funIt++){
    (*funIt)->gen();
  }

}
