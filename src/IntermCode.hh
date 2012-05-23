#include <list>
#include <unordered_map>
#include "quad.hh"


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

public:
  IntermCode(): nextlabel(0){};
  Label* newLabel();
  SymVar* newTemp();
  void addInst(Quad* quad);
  void emitLabel(Label* label);
  void emitLabel2(Quad* instr);
  bool areUnSet();
};



// Temp tendra varios constructores dependiendo de si es constant float int sym etc
class Temp{

};
