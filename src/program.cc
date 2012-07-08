#include <iostream>

#include "IntermCode.hh"
#include "program.hh"
#include "symbol.hh"

extern IntermCode intCode;

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
    (*funIt)->check();
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

void Program::gen(){
  // Escupir runtime en la salida
  // Ridiculo, escupir un string a la salida y ya

  // Generar codigo para las variables gobales
  for(std::list<VariableDec*>::iterator varIt=this->globalinits.begin();
      varIt != this->globalinits.end(); varIt++){
    (*varIt)->gen(NULL);
    // Ir recogiendo una lista de variables globales para meterlas en
    // .data
  }

  // Agregar la instrucción call main
  intCode.addInst(new CallQ(main, 0));
  std::list<Quad*> global_tac = intCode.getInstructions();
  intCode.clear();

  // Escupir en la salida los .data de variables

  // Pasar el TAC de inicialización a MIPS y escupirlo a la salida
  // Pasarlo a grafo, pasarlo a mips, optimizar

  // Escupir los .data de strings y borrar MIPSCode

  this->main->gen();
  for (std::list<SymFunction*>::iterator funIt= this->functions.begin();
       funIt != this->functions.end(); funIt++){
    (*funIt)->gen();
  }

}
