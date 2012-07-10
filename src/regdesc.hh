#ifndef DEVANIX_REGDESC
#define DEVANIX_REGDESC

#include <map>
#include <set>
#include <list>

#include "registers.hh"
#include "symbol.hh"
#include "instruction.hh"

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

  /**
   * Borra de r la variable s, y de s la ubicación r
   */
  void removeLocation(Reg r, SymVar* s);

  /**
   * Vacía los registros actuales y devuelve las instrucciones necesarias
   */
  std::list<Instruction*> emptyRegs();

  std::set<SymVar*>* getSet(Reg r);

  std::list<Instruction*> genStore(Reg r, SymVar* v);

};

#endif
