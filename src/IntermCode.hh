#ifndef DEVANIX_CODE
#define DEVANIX_CODE

#include <list>
#include <unordered_map>
#include "Quad.hh"

class Label;
class Temp;

typedef std::unordered_multimap<int,Label*> labels;

class IntermCode{
private:
  // Conjunto de etiquetas asociadas a instrucciones
  labels labelset;
  // Conjunto de etiquetas sin asignar
  std::list<Label*> unSet;
  // Lista de instrucciones
  std::list<Quad*> inst;
  // Num prox etiqueta
  int nextlabel;
  // Num prox temporal
  int nextTemp;

public:
  IntermCode(): nextlabel(0),nextTemp(0){};
  Label* newLabel();
  SymVar* newTemp();
  void addInst(Quad* quad);
  void emitLabel(Label* label);
  void emitLabel2(Quad* instr);
  bool areUnSet();
};

#endif
