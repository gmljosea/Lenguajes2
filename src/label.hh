#ifndef DEVANIX_LABEL
#define DEVANIX_LABEL

#include <string>

#include "Quad.hh"

class Quad;

/**
 * Representa una etiqueta asociada a una instruccion
 */
class Label {
private:
  Quad* instruction;
  int id;
  std::string new_id;
  bool active;
public:
  Label(int id): id(id), active(false){};
  Label(std::string str) : new_id(str), id(-1), active(false) {};
  void setInstruction(Quad* quad);
  void setActive(bool act);
  bool isActive();
  Quad* getInstruction();
  int getId();
  std::string toString();
};

#endif
