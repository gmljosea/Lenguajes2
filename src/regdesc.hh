#ifndef DEVANIX_REGDESC
#define DEVANIX_REGDESC

#include <map>
#include <set>
#include <list>

#include "Quad.hh"
#include "registers.hh"
#include "symbol.hh"
#include "instruction.hh"

struct RegSet {
  Reg rx;
  Reg ry;
  Reg rz;
  std::list<Instruction*> stores;
};

struct RegDesc {
  std::map<Reg, std::set<SymVar*>*> rints;
  std::map<Reg, std::set<SymVar*>*> rfloats;
  std::set<SymVar*> liveTemps;

  RegDesc();

  /**
   * Deja el registro totalmente vacío, y actualiza las variables
   * que tenía para que ya no posean a r como ubicación.
   * El borrado se hace ciegamente. Se asume que se hizo spill
   * de lo que había allí.
   */
  void clearReg(Reg r);

  /**
   * Agrega a r la variable s, y a s la ubicación r
   */
  void addLocation(Reg r, SymVar* s);
  void addExclusiveLocation(Reg r, SymVar* s);

  /**
   * Borra de r la variable s, y de s la ubicación r
   */
  void removeLocation(Reg r, SymVar* s);

  /**
   * Vacía los registros actuales y devuelve las instrucciones necesarias
   */
  std::list<Instruction*> emptyRegs();

  std::set<SymVar*>* getSet(Reg r);

  // FIXME: tomar en cuenta cuando es una variable argumento
  std::list<Instruction*> genStore(Reg r, SymVar* v);

  // Funciones para obtener registros

  // Cada función revisa los registros y devuelve un conjunto de registros
  // más las instrucciones necesarias para salvar los valores que estaban
  // allí.
  // Sin embargo ninguna función manipula los descriptores. Eso es trabajo
  // de los gen() de cada Quad.

  // Devuelve 1 registro para meter la variable op
  // Sirve para implantar los return
  RegSet get1Reg(SymVar* op, bool f);

  // Devuelve 1 registro totalmente vacío nuevo
  // Sirve para obtener registros donde guardar constantes
  RegSet getFreshReg(bool f);

  // Devuelve 2 registros para meter los operandos
  // Siempre devuelve dos registros distintos, a menos que ambos operandos
  // se encuentren ya en el mismo registro.
  // Sirve para implantar los if
  RegSet get2Reg(SymVar* op1, SymVar* op2, bool f);

  // Devuelve 2 registros, uno para cargar el operando y otro para
  // guardar el resultado de una operación sobre el operando.
  // Considera la vida del operando para determinar si asignar el mismo
  // registro a res.
  // Sirve para implantar res := - op
  RegSet get2RegAs(SymVar* res, SymVar* op, bool f);

  // Devuelve 3 registros independientes para los operandos
  // A menos que algún subconjunto de los mismos ya compartan un
  // registro
  // Sirve para implantar op1[op2] = op3
  RegSet get3Reg(SymVar* op1, SymVar* op2, SymVar* op3, bool f);

  // Devuelve 3 registros, dos para cargar los operandos
  // y otro para guardar el resultado.
  // Dependiendo de los datos de vida el registro para res podría
  // ser el mismo que el de un operando
  // Sirve para implantar res := op1 . op2
  RegSet get3RegAs(SymVar* res, SymVar* op1, SymVar* op2, bool f);

  Instruction* loadVar(Args arg, ArgType type, Reg r);

  Instruction* loadVar(Reg r, SymVar* s);


};

#endif
