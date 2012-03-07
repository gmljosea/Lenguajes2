#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

#include <map>
#include <string>

class Type {
public:
  virtual int getSize() = 0;
  virtual bool operator==(Type& b);
};

// Tipos básicos escalares
class IntType : public Type {
private:
  const static int size = 4; // en bytes
  IntType();
  void operator=(IntType const&);
public:
  static IntType& getInstance() {
    static IntType instance;
    return instance;
  }
};

class FloatType : public Type {
private:
  const static int size = 4; // en bytes
  FloatType();
  void operator=(FloatType const&);
public:
  static FloatType& getInstance() {
    static FloatType instance;
    return instance;
  }
};

class BoolType : public Type {
private:
  const static int size = 1; // en bytes
  BoolType();
  void operator=(BoolType const&);
public:
  static BoolType& getInstance() {
    static BoolType instance;
    return instance;
  }
};

class CharType : public Type {
private:
  const static int size = 1; // en bytes
  CharType();
  void operator=(CharType const&);
public:
  static CharType& getInstance() {
    static CharType instance;
    return instance;
  }
};

// Tipos especiales
class VoidType : public Type {
private:
  const static int size = 0; // en bytes
  VoidType();
  void operator=(VoidType const&);
public:
  static VoidType& getInstance() {
    static VoidType instance;
    return instance;
  }
};

class StringType : public Type {
private:
  int size;
public:
  StringType(int size);
};

class ErrorType : public Type {
private:
  const static int size = 0; // en bytes
  VoidType();
  void operator=(ErrorType const&);
public:
  static VoidType& getInstance() {
    static VoidType instance;
    return instance;
  }
};

// Tipos compuestos
class ArrayType : public Type {
private:
  Type* type;
  int size;
public:
  ArrayType(Type* type, int size);
  Type* getBaseType();
  int getOffset(int pos); //offset de la posición pos
};

class BoxType : public Type {
private:
  struct BoxField {
    std::string name;
    int offset;
    /* Type es null si el campo es un box que no se encontró en la tabla
     * al momento de parsear. Cuando esto ocurre, el nombre del box se guarda
     * en boxname para buscarlo luego en la 2da pasada.
     */
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
};

#endif
