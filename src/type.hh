#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

#include <map>
#include <string>

class Type {
protected:
  int size;
  int alignment;
public:
  Type(int size, int alignment) : size(size), alignment(alignment) {};
  virtual int getSize();
  virtual int getAlignment();
  virtual bool operator==(Type& b);
};

// Tipos básicos escalares
class IntType : public Type {
private:
  IntType() : Type(4, 4) {};
  void operator=(IntType const&);
public:
  static IntType& getInstance();
};

class FloatType : public Type {
private:
  FloatType() : Type(4, 4) {};
  void operator=(FloatType const&);
public:
  static FloatType& getInstance();
};

class BoolType : public Type {
private:
  BoolType() : Type(1, 1) {};
  void operator=(BoolType const&);
public:
  static BoolType& getInstance();
};

class CharType : public Type {
private:
  CharType() : Type(1, 1) {};
  void operator=(CharType const&);
public:
  static CharType& getInstance();
};

// Tipos especiales
class VoidType : public Type {
private:
  VoidType() : Type(0, 0) {};
  void operator=(VoidType const&);
public:
  static VoidType& getInstance();
};

class StringType : public Type {
public:
  StringType(int length) : Type(length, 1) {};
};

class ErrorType : public Type {
private:
  ErrorType() : Type(0, 0) {};
  void operator=(ErrorType const&);
public:
  static ErrorType& getInstance();
};

// Tipos compuestos
class ArrayType : public Type {
private:
  Type* basetype;
  int length;
public:
  ArrayType(Type* btype, int length) : basetype(btype), length(length),
                                       Type(0,0) {};
  virtual bool operator==(Type& t);
  virtual int getSize();
  virtual int getAlignment();
  Type* getBaseType();
  int getLength();
  int getOffset(int pos); //offset de la posición pos
};

/*
class BoxType : public Type {
private:
  struct BoxField {
    std::string name;
    int offset;
    /* Type es null si el campo es un box que no se encontró en la tabla
     * al momento de parsear. Cuando esto ocurre, el nombre del box se guarda
     * en boxname para buscarlo luego en la 2da pasada.
     /
    Type* type;
    std::string boxname;
  };
  std::String name;
  std::map<std::string, BoxField> fixed_fields;
  std::map<std::string, BoxField> variant_fields;
  int size;
public:
  BoxType(std::string name);
  void addFixedField(Type* type, std::string name);
  void addVariantField(Type* type, std::string name);
  Type* getFieldType(std::string field);
  int getFieldOffset(std::String field);
  std::string getName();
};*/

#endif
