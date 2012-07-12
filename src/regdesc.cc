#include "regdesc.hh"
#include "MIPSinstruction.hh"

#include <list>

RegDesc rdesc;

typedef std::set<SymVar*> Tset;

RegDesc::RegDesc() {
  // Cargar descriptores de registros enteros disponibles
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
  Tset* set = getSet(r);
  for (Tset::iterator it = set->begin();
       it != set->end(); it++) {
    (*it)->removeReg(r);
    (*it)->inMem(true);
  }
  set->clear();
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
  Tset* set = getSet(r);
  set->insert(s);
  s->addReg(r);
}

// FIXME: hacer addExclusiveLocation
void RegDesc::addExclusiveLocation(Reg r, SymVar* s) {
  s->inMem(false);
  std::set<Reg> locs;
  for (std::set<Reg>::iterator it = locs.begin();
       it != locs.end(); it++) {
    removeLocation(*it, s);
  }
  addLocation(r,s);
}

void RegDesc::removeLocation(Reg r, SymVar* s) {
  Tset* set = getSet(r);
  set->erase(s);
  s->removeReg(r);
}

std::list<Instruction*> RegDesc::emptyRegs() {
  std::list<Instruction*> list;

  for (std::map<Reg, Tset*>::iterator it = rints.begin();
       it != rints.end(); it++) {
    Reg r = it->first;
    list.splice(list.end(), dumpReg(r));
    clearReg(r);
  }

  for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
       it != rfloats.end(); it++) {
    Reg r = it->first;
    list.splice(list.end(), dumpReg(r));
    clearReg(r);
  }

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

int RegDesc::spillCost(Reg r) {
  Tset* set = getSet(r);
  int count = 0;
  for (std::set<SymVar*>::iterator it = set->begin();
       it != set->end(); it++) {
    if ((*it)->availableOther(r)) continue;
    if ((*it)->isTemp() and liveTemps.count(*it) == 0) continue;
    count++;
  }
  return count;
}

std::list<Instruction*> RegDesc::dumpReg(Reg r) {
  std::list<Instruction*> l;
  Tset* set = getSet(r);
  for (std::set<SymVar*>::iterator it = set->begin();
       it != set->end(); it++) {
    if ((*it)->availableOther(r)) continue;
    if ((*it)->isTemp() and liveTemps.count(*it) == 0) continue;
    l.push_back(storeVar(r, *it));
  }
  return l;
}

RegSet RegDesc::get1Reg(SymVar* op, bool f) {
  RegSet r;

  Reg candidate = op->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.rx = candidate;
    return r;
  }

  return getFreshReg(f);
}

RegSet RegDesc::getFreshReg(bool f) {
  RegSet r;

  if (!f) {
    r.rx = Reg::t0;
    int acc = spillCost(Reg::t0);

    for (std::map<Reg, Tset*>::iterator it = rints.begin();
	 it != rints.end(); it++) {
      int s = spillCost(it->first);
      if (s >= acc) continue;
      r.rx = it->first;
      acc = s;
    }
  } else {
    r.rx = Reg::f2;
    int acc = spillCost(Reg::f2);

    for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	 it != rfloats.end(); it++) {
      int s = spillCost(it->first);
      if (s >= acc) continue;
      r.rx = it->first;
      acc = s;
    }
  }

  r.stores = dumpReg(r.rx);

  return r;
}

RegSet RegDesc::get2Reg(SymVar* op1, SymVar* op2, bool f) {
  RegSet r;
  r.rx = Reg::zero;
  r.ry = Reg::zero;
  r.rz = Reg::zero;

  Reg candidate = op1->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.rx = candidate;
  }

  candidate = op2->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.ry = candidate;
  }

  if (r.rx == Reg::zero) {
    if (!f) {
      r.rx = Reg::t0;
      int acc = spillCost(Reg::t0);

      for (std::map<Reg, Tset*>::iterator it = rints.begin();
	   it != rints.end(); it++) {
	if (it->first == r.ry) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rx = it->first;
	acc = s;
      }
    } else {
      r.rx = Reg::f2;
      int acc = spillCost(Reg::f2);

      for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	   it != rfloats.end(); it++) {
	if (it->first == r.ry) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rx = it->first;
	acc = s;
      }
    }
  }

  if (r.ry == Reg::zero) {
    if (!f) {
      r.ry = Reg::t1;
      int acc = spillCost(Reg::t1);

      for (std::map<Reg, Tset*>::iterator it = rints.begin();
	   it != rints.end(); it++) {
	if (it->first == r.rx) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.ry = it->first;
	acc = s;
      }
    } else {
      r.ry = Reg::f3;
      int acc = spillCost(Reg::f3);

      for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	   it != rfloats.end(); it++) {
	if (it->first == r.rx) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.ry = it->first;
	acc = s;
      }
    }
  }

  if (! op1->isInReg(r.rx)) r.stores.splice(r.stores.end(), dumpReg(r.rx));
  if (! op2->isInReg(r.ry)) r.stores.splice(r.stores.end(), dumpReg(r.ry));

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
  r.rx = Reg::zero;
  r.ry = Reg::zero;
  r.rz = Reg::zero;

  Reg candidate = op1->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.rx = candidate;
  }

  candidate = op2->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.ry = candidate;
  }

  candidate = op3->getLocation();
  if (candidate != Reg::zero and f == isFloatReg(candidate)) {
    r.rz = candidate;
  }

  if (r.rx == Reg::zero) {
    if (!f) {
      r.rx = Reg::t0;
      int acc = spillCost(Reg::t0);

      for (std::map<Reg, Tset*>::iterator it = rints.begin();
	   it != rints.end(); it++) {
	if (it->first == r.ry or it->first == r.rz) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rx = it->first;
	acc = s;
      }
    } else {
      r.rx = Reg::f2;
      int acc = spillCost(Reg::f2);

      for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	   it != rfloats.end(); it++) {
	if (it->first == r.ry or it->first == r.rz) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rx = it->first;
	acc = s;
      }
    }
  }

  if (r.ry == Reg::zero) {
    if (!f) {
      r.ry = Reg::t1;
      int acc = spillCost(Reg::t1);

      for (std::map<Reg, Tset*>::iterator it = rints.begin();
	   it != rints.end(); it++) {
	if (it->first == r.rx or it->first == r.rz) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.ry = it->first;
	acc = s;
      }
    } else {
      r.ry = Reg::f3;
      int acc = spillCost(Reg::f3);

      for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	   it != rfloats.end(); it++) {
	if (it->first == r.rx or it->first == r.rz) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.ry = it->first;
	acc = s;
      }
    }
  }

  if (r.rz == Reg::zero) {
    if (!f) {
      r.rz = Reg::t2;
      int acc = spillCost(Reg::t2);

      for (std::map<Reg, Tset*>::iterator it = rints.begin();
	   it != rints.end(); it++) {
	if (it->first == r.rx or it->first == r.ry) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rz = it->first;
	acc = s;
      }
    } else {
      r.rz = Reg::f4;
      int acc = spillCost(Reg::f4);

      for (std::map<Reg, Tset*>::iterator it = rfloats.begin();
	   it != rfloats.end(); it++) {
	if (it->first == r.rx or it->first == r.ry) continue;
	int s = spillCost(it->first);
	if (s >= acc) continue;
	r.rz = it->first;
	acc = s;
      }
    }
  }

  if (! op1->isInReg(r.rx)) r.stores.splice(r.stores.end(), dumpReg(r.rx));
  if (! op2->isInReg(r.ry)) r.stores.splice(r.stores.end(), dumpReg(r.ry));
  if (! op3->isInReg(r.rz)) r.stores.splice(r.stores.end(), dumpReg(r.rz));

  return r;

}

RegSet RegDesc::get3RegAs(SymVar* res, SymVar* op1, SymVar* op2, bool f) {
  RegSet r;
  r.rx = Reg::t0;
  r.ry = Reg::t1;
  r.rz = Reg::t2;
  return r;
}

Instruction* RegDesc::loadVar(Reg r, SymVar* s) {
  if (isFloatReg(r)) {
    if (s->availableReg()) {
      Reg o = s->getLocation();
      if (isFloatReg(o)) {
	// Generar move dentro del coprocesador
	return new MoveS(r, o);
      } else {
	// Generar move del procesador al coprocesador
	return new Mtc1(r, o);
      }
    } else {
      if (s->isGlobal()) {
	// Load flotante desde un label
	return new LwS(r, s->getLabel());
      } else {
	// Load flotante con offset del fp
	return new LwS(r, s->getOffset(), Reg::fp);
      }
    }

  } else {
    if (s->availableReg()) {
      Reg o = s->getLocation();
      if (isFloatReg(o)) {
	// Generar move entre coprocesadores
	return new Mfc1(r, o);
      } else {
	// Generar move dentro del procesdor normal
	return new Move(r, o);
      }
    } else {
      if (s->isGlobal()) {
	// Load normal desde un label
	return new Lw(r, s->getLabel());
      } else {
	// Load normal con offset del fp
	return new Lw(r, s->getOffset(), Reg::fp);
      }
    }
  }
}

Instruction* RegDesc::storeVar(Reg r, SymVar* s) {
  if (isFloatReg(r)) {
    if (s->isGlobal()) {
      return new SwS(r, s->getLabel());
    } else {
      return new SwS(r, s->getOffset(), Reg::fp);
    }
  } else {
    if (s->isGlobal()) {
      return new Sw(r, s->getLabel());
    } else {
      return new Sw(r, s->getOffset(), Reg::fp);
    }
  }
}

Instruction* RegDesc::loadVar(Args arg,ArgType type,Reg r){

  switch(type){
  case ArgType::id:
    return loadVar(r,arg.id);
  case ArgType::constint:
    return new Li(r,arg.constint);
  case ArgType::constfloat:
    return new LiS(r,arg.constfloat);
  default:
    return NULL;
  }
}
