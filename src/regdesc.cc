#include "regdesc.hh"
#include "MIPSinstruction.hh"

#include <list>

RegDesc rdesc;

typedef std::set<SymVar*> Tset;

RegDesc::RegDesc() {
  // Cargar descriptores de registros enteros disponibles
  rints.insert(std::pair<const Reg, Tset*>(Reg::a0, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::a1, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::a2, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::a3, new Tset()));

  rints.insert(std::pair<const Reg, Tset*>(Reg::v1, new Tset()));

  rints.insert(std::pair<const Reg, Tset*>(Reg::t0, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t1, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t2, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t3, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t4, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t5, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t6, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t7, new Tset()));

  rints.insert(std::pair<const Reg, Tset*>(Reg::s0, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s1, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s2, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s3, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s4, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s5, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s6, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::s7, new Tset()));

  rints.insert(std::pair<const Reg, Tset*>(Reg::t8, new Tset()));
  rints.insert(std::pair<const Reg, Tset*>(Reg::t9, new Tset()));

  // Cargar descriptores de registros flotantes disponibles
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f0, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f1, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f2, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f3, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f4, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f5, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f6, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f7, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f8, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f9, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f10, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f11, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f12, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f13, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f14, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f15, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f16, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f17, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f18, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f19, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f20, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f21, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f22, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f23, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f24, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f25, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f26, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f27, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f28, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f29, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f30, new Tset()));
  rfloats.insert(std::pair<const Reg, Tset*>(Reg::f31, new Tset()));
}

void RegDesc::clearReg(Reg r) {
  // FIXME marcar variables borradas como en memoria
  // Si son temporales no vivos o variables que solo estén en este registro
  /*Tset* set = getSet(r);
  for (Tset::iterator it = set->begin();
       it != set->end(); it++) {
    (*it)->removeReg(r);
  }
  set->clear();*/
}

Tset* RegDesc::getSet(Reg r) {
  Tset* set;
  std::map<Reg, Tset*>::iterator it;
  if (isFloatReg(r)) {
    it = rfloats.find(r);
  } else {
    it = rints.find(r);
  }
  set = it->second;
}

void RegDesc::addLocation(Reg r, SymVar* s) {
  /*Tset* set = getSet(r);
  set->insert(s);
  s->addReg(r);*/
}

// FIXME: hacer addExclusiveLocation
void RegDesc::addExclusiveLocation(Reg r, SymVar* s) {
}

void RegDesc::removeLocation(Reg r, SymVar* s) {
  /*  Tset* set = getSet(r);
  set->erase(s);
  s->removeReg(r);*/
}

std::list<Instruction*> RegDesc::emptyRegs() {
  std::list<Instruction*> list;
      /*
  for (std::map<Reg, Tset*>::iterator it = rints.begin();
       it != rints.end(); it++) {
    Reg r = it->first;
    Tset* set = it->second;
    for (Tset::iterator it = set->begin();
	 it != set->end(); it++) {
      SymVar* v = *it;
      v->removeReg(r);
      if (v->isInMem()) continue;
      list.splice(list.end(), this->genStore(r, v));
      v->inMem(true);
    }
  }

  for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
       it != rfloats.end(); it++) {
    Reg r = it->first;
    Tset* set = it->second;
    for (Tset::iterator it = set->begin();
	 it != set->end(); it++) {
      SymVar* v = *it;
      v->removeReg(r);
      if (v->isInMem()) continue;
      list.splice(list.end(), this->genStore(r, v));
      v->inMem(true);
    }
  }
      */
  return list;
}

std::list<Instruction*> RegDesc::genStore(Reg r, SymVar* v) {
  std::list<Instruction*> st;
  /*
  // Es un arreglo o box, o sea, la dirección de uno.
  // Nada que hacer, move on
  if (v->getType()->alwaysByReference()
      || (v->isTemp() && rdesc.liveTemps.count(v) == 0)) {
    return st;
  }

  // Variable global
  if (v->isGlobal()) {
    st.push_back(new Sw(r, v->getLabel()));
    return st;
  }

  int offset = v->spill();
  st.push_back(new Sw(r, -(v->getOffset()+8), Reg::fp));*/
  return st;
}

RegSet RegDesc::get1Reg(SymVar* op, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

RegSet RegDesc::getFreshReg(bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

RegSet RegDesc::get2Reg(SymVar* op1, SymVar* op2, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

RegSet RegDesc::get2RegAs(SymVar* res, SymVar* op, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

RegSet RegDesc::get3Reg(SymVar* op1, SymVar* op2, SymVar* op3, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

RegSet RegDesc::get3RegAs(SymVar* res, SymVar* op1, SymVar* op2, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

// FIXME
Instruction* RegDesc::loadVar(Reg r, SymVar* s) {
  return new J(new Label("HOLA"));
}
