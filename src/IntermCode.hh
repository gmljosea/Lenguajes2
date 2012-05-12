
typedef std::unordered_multimap<std::string,Label> labels;

class ItermCode {
private:
  // Conjunto de etiquetas del programa
  labels labelset;
  // Lista de instrucciones
  std::list<Quad> inst;
  // Num prox etiqueta
  int nextlabel;

public:
  IntermCode ():nextlabel(0);

};

class Label {
private:
  Quad instruction;

public:
  Label();
};
