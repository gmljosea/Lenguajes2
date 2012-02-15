#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

enum TypeKind {
  Scalar,
  Void,
  String
};

class Type {
protected:
  TypeKind type;
public:
  TypeKind getTypeKind();
  virtual void print();
  virtual bool operator==(Type& b);
  /*
    Hay un problema. operator== debería ser abstracto, pero como en otros lados
    hemos usado variables Type por valor, el compilador explota porque da
    errores de que no s epuede instanciar una clase abstracta, lo cual tiene
    sentido. Hay que arreglar los lugares donde usemos Type para que usen una
    referencia a Type.
    Por ahora lo dejo así y le pongo una implementación dummy para que compile
    y nos ahorremos esos cambios para esta entrega.
    C++ es complicado.
   */
};

class VoidType : public Type {
public:
  VoidType ();
  virtual bool operator==(Type& b);
  virtual void print();
};

class StringType : public Type {
public:
  StringType ();
  virtual bool operator==(Type& b);
  virtual void print();
};

enum ScalarKind {
  Integer,
  Float,
  Char,
  Bool
};

class ScalarType : public Type {
protected:
  ScalarKind scalartype;
public:
  ScalarType();
  ScalarKind getScalarKind();
  virtual bool operator==(Type& b);
};

class IntType : public ScalarType {
public:
  IntType ();
  virtual void print();
};

class FloatType : public ScalarType {
public:
  FloatType ();
  virtual void print();
};

class CharType : public ScalarType {
public:
  CharType ();
  virtual void print();
};

class BoolType : public ScalarType {
public:
  BoolType ();
  virtual void print();
};

#endif
