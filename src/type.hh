#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

#include <list>
#include <unordered_map>
#include <string>

class Type {
protected:
  int size;
  int alignment;
public:
  Type(int size, int alignment) : size(size), alignment(alignment) {};
  virtual int getSize();
  virtual int getAlignment();
  virtual void check();
  virtual bool operator==(Type& b);
  virtual bool operator!=(Type& b);
  virtual void print();
  virtual std::string toString();
  virtual int getReferenceSize();
  // Dice si el tipo se pasa siempre por referencia
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
  VoidType() : Type(0, 0) {};
  void operator=(VoidType const&);
public:
  static VoidType& getInstance();
  virtual std::string toString();
};

class StringType : public Type {
public:
  StringType(int length) : Type(length, 1) {};
  virtual bool operator==(Type& t);
  void setLength(int length);
  virtual std::string toString();
  virtual bool alwaysByReference();
};

class ErrorType : public Type {
private:
  ErrorType() : Type(0, 0) {};
  void operator=(ErrorType const&);
public:
  static ErrorType& getInstance();
  virtual std::string toString();
};

// Tipos compuestos
class ArrayType : public Type {
private:
  Type* basetype;
  int length;
  int line;
  int col;
public:
  ArrayType(Type* btype, int length,
            int line,int col) : basetype(btype), length(length),
                                line(line),col(col),Type(0,0) {};
  virtual bool operator==(Type& t);
  virtual std::string toString();
  virtual int getSize();
  virtual int getAlignment();
  virtual void check();
  Type* getBaseType();
  int getLength();
  int getOffset(int pos); //offset de la posición pos
  virtual int getReferenceSize();
  virtual bool alwaysByReference();
};

struct BoxField {
  Type* type;
  std::string name;
  int offset;
  bool grouped;
  int groupnum;
  int line;
  int column;
};

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
protected:
  bool reaches(BoxType& box);
public:
  BoxType(std::string name, bool incomplete)
    : name(name), incomplete(incomplete), groupcount(0),
      offsetsDone(false),Type(0,0) {};
  void addFixedField(Type* type, std::string name,int l,int col);
  void addVariantField(Type* type, std::string name, bool grouped,int l,int col);
  void startGrouping();
  BoxField* getField(std::string field);
  std::list<BoxField*> getFFields();
  std::list<BoxField*> getVFields();
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
  virtual std::string toString();
  virtual bool alwaysByReference();
};

#endif
