#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

#include <list>
#include <unordered_map>
#include <string>

/**
 * Representa un tipo en el sistema de tipos en Devanix
 */
class Type {
protected:
  // Tamaño y alineación de este tipo en bytes
  int size;
  int alignment;
public:
  Type(int size, int alignment) : size(size), alignment(alignment) {};
  // Tamaño y alineación en bytes del tipo
  virtual int getSize();
  virtual int getAlignment();

  // Comparación de igualdad entre tipos
  virtual bool operator==(Type& b);
  virtual bool operator!=(Type& b);

  virtual void print();
  virtual std::string toString();

  virtual void check();

  /**
   * Actualmente no tenemos un tipo 'referencia a', así que el pase de
   * argumentos por referencia es un chanchullo entre SymVar y Type.
   * Ya es muy tarde para pensar algo mejor así que luego veremos en
   * una mejor manera de modelar el asunto.
   */
  // Devuelve el tamaño en bytes de una referencia a una variable de este tipo
  // Era necesario porque inicialmente planeamos tener arreglo de una sola
  // dimensión así que junto a la referencia se iba a pasar el tamaño.
  virtual int getReferenceSize();
  // Dice en que modo se pasa este tipo por default
  virtual bool alwaysByReference();
};

// Tipos básicos escalares
class IntType : public Type {
private:
  IntType() : Type(4, 4) {};
  void operator=(IntType const&);
public:
  static IntType& getInstance();
  virtual std::string toString();
};

class FloatType : public Type {
private:
  FloatType() : Type(4, 4) {};
  void operator=(FloatType const&);
public:
  static FloatType& getInstance();
  virtual std::string toString();
};

class BoolType : public Type {
private:
  BoolType() : Type(1, 1) {};
  void operator=(BoolType const&);
public:
  static BoolType& getInstance();
  virtual std::string toString();
};

class CharType : public Type {
private:
  CharType() : Type(1, 1) {};
  void operator=(CharType const&);
public:
  static CharType& getInstance();
  virtual std::string toString();
};

// Tipos especiales
class VoidType : public Type {
private:
  VoidType() : Type(1, 1) {};
  void operator=(VoidType const&);
public:
  static VoidType& getInstance();
  virtual std::string toString();
};

class StringType : public Type {
public:
  StringType(int length) : Type(4, 4) {};
  virtual bool operator==(Type& t);
  void setLength(int length);
  virtual std::string toString();
  virtual bool alwaysByReference();
};

/// Tipo error, indica el tipo de una expresión que no tenga sentido
class ErrorType : public Type {
private:
  ErrorType() : Type(1, 1) {};
  void operator=(ErrorType const&);
public:
  static ErrorType& getInstance();
  virtual std::string toString();
};

// Tipos compuestos
// Tipo arreglo, el tipo base de un arreglo no puede void, string ni otro arreglo
class ArrayType : public Type {
private:
  Type* basetype;
  int length;
  int line, col;
public:
  ArrayType(Type* btype, int length,
            int line,int col) : basetype(btype), length(length),
                                line(line),col(col),Type(0,0) {};
  virtual bool operator==(Type& t);
  virtual std::string toString();
  virtual int getSize();
  virtual int getAlignment();
  // Chequea que no sea arreglo de más de 1 dimensión, ni arreglo de strings
  virtual void check();
  Type* getBaseType();
  int getLength();
  int getOffset(int pos); //offset de la posición pos
  virtual int getReferenceSize();
  virtual bool alwaysByReference();
};

// Representa un campo de un box, sea variant o no
struct BoxField {
  Type* type;
  std::string name;
  int offset;

  int line, column;

  // Campos útiles solo para campos variant
  bool grouped; // Si está en una agrupación
  int groupnum; // Número de campo, servirá para hacer los chequeos dinámicos
};

// Representa un tipo Box, con cualquier cantidad de campos fijos o variantes
class BoxType : public Type {
private:
  std::string name;
  std::unordered_map<std::string, BoxField*> fields_hash;
  std::list<BoxField*> fixed_fields;
  std::list<BoxField*> variant_fields;
  int line;
  int column;
  bool incomplete;
  int groupcount;
  bool offsetsDone;
  bool visited; // usado durante la detección de ciclos entre boxes
protected:
  bool reaches(BoxType& box);
public:
  BoxType(std::string name, bool incomplete)
    : name(name), incomplete(incomplete), groupcount(0), visited(false),
      offsetsDone(false),Type(1,1) { };
  void addFixedField(Type* type, std::string name,int l,int col);
  void addVariantField(Type* type, std::string name, bool grouped,int l,int col);
  void startGrouping();
  BoxField* getField(std::string field);
  std::list<BoxField*>& getFFields();
  std::list<BoxField*>& getVFields();
  virtual void check();
  void calcOffsets();
  bool isIncomplete();
  bool areOffsetsDone();
  void setIncomplete(bool ic);
  int getFieldCount();
  std::string getName();
  void setLine(int l);
  void setColumn(int c);
  int getLine();
  int getColumn();
  void print();
  void printDetail();
  virtual std::string toString();
  virtual bool alwaysByReference();
};

#endif
