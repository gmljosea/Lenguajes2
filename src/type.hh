#ifndef DEVANIX_TYPES
#define DEVANIX_TYPES

class Type {
public:
  virtual void print();
};

class IntType : public Type {
};

class FloatType : public Type {
};

class CharType : public Type {
};

class VoidType : public Type {
};

class BoolType : public Type {
};

class StringType : public Type {
};

#endif
