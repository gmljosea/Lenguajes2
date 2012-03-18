#include <iostream>
#include "program.hh"
#include "type.hh"

extern Program program;
// Type
int Type::getSize() {
  return this->size;
}

int Type::getAlignment() {
  return this->alignment;
}

bool Type::operator==(Type& b) {
  return this == &b;
}

bool Type::operator!=(Type& b) {
  return !(*this == b);
}

void Type::print() {
  std::cout << this->toString() << std::endl;
}

std::string Type::toString() {
  return std::string("Algun tipo");
}

void Type::check(){
  return;
}

int Type::getReferenceSize() {
  return 4;
}

bool Type::alwaysByReference() {
  return false;
}

// IntType
IntType& IntType::getInstance() {
  static IntType instance;
  return instance;
}

std::string IntType::toString() {
  return std::string("int");
}

// FloatType
FloatType& FloatType::getInstance() {
  static FloatType instance;
  return instance;
}

std::string FloatType::toString() {
  return std::string("float");
}

// BoolType
BoolType& BoolType::getInstance() {
  static BoolType instance;
  return instance;
}

std::string BoolType::toString() {
  return std::string("bool");
}

// CharType
CharType& CharType::getInstance() {
  static CharType instance;
  return instance;
}

std::string CharType::toString() {
  return std::string("char");
}

// VoidType
VoidType& VoidType::getInstance() {
  static VoidType instance;
  return instance;
}

std::string VoidType::toString() {
  return std::string("void");
}

// StringType
bool StringType::operator==(Type& t) {
  return true;
  StringType* ta;
  if (ta = dynamic_cast<StringType*>(&t)) {
    return this->size == ta->getSize();
  } else {
    return false;
  }
}

void StringType::setLength(int length) {
  this->size = length;
}

std::string StringType::toString() {
  return std::string("string");
}

bool StringType::alwaysByReference() {
  return true;
}

// ErrorType
ErrorType& ErrorType::getInstance() {
  static ErrorType instance;
  return instance;
}

std::string ErrorType::toString() {
  return std::string("error");
}

// ArrayType
bool ArrayType::operator==(Type& t) {
  ArrayType* ta;
  if (ta = dynamic_cast<ArrayType*>(&t)) {
    return this->basetype == ta->getBaseType();
      //      && this->length == ta->getLength();
  } else {
    return false;
  }
}

std::string ArrayType::toString() {
  return std::string(this->basetype->toString()+" array["
		     +std::to_string(length)+"]");
}

bool ArrayType::alwaysByReference() {
  return true;
}

/**
 * Como el tipo base de un array podría ser un box incompleto al momento
 * de instanciar el array, entonces no podemos calcular el tamaño y
 * alineación del arreglo en el constructor, entonces lo hacemos en las
 * propias llamadas a getSize y getAlignment.
 * La precondición es que se llamen cuando se esté seguro que el tipo base
 * no es un box incompleto.
 */
int ArrayType::getSize() {
  return this->basetype->getSize() * this->length;
}

int ArrayType::getAlignment() {
  return this->basetype->getAlignment();
}

Type* ArrayType::getBaseType() {
  return this->basetype;
}

int ArrayType::getLength() {
  return this->length;
}

void ArrayType::check(){
  ArrayType *cast_tarr= dynamic_cast<ArrayType*>(basetype);
  StringType *cast_str = dynamic_cast<StringType*>(basetype);
  if (cast_tarr or cast_str)
    program.error("tipo base del arreglo es '"+basetype->toString()
                  + "' pero se esperaba un tipo basico o box",line,col);

}

int ArrayType::getOffset(int pos) {
  return this->basetype->getSize()*pos;
}

int ArrayType::getReferenceSize() {
  return 8;
}


// BoxType
void BoxType::addFixedField(Type* type, std::string name) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->offset = 0;
  field->grouped = false;
  field->groupnum = 0;
  this->fixed_fields.push_back(field);
  this->fields_hash[name] = field;
}

void BoxType::addVariantField(Type* type, std::string name, bool grouped) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->offset = 0;
  field->grouped = grouped;
  field->groupnum = this->groupcount;
  this->variant_fields.push_back(field);
  this->fields_hash[name] = field;
}

void BoxType::startGrouping() {
  this->groupcount++;
}


BoxField* BoxType::getField(std::string field) {
  std::unordered_map<std::string, BoxField*>::iterator it;
  it= this->fields_hash.find(field);
  if (it != this->fields_hash.end()) {
    return it->second;
  }
  return NULL;
}

bool BoxType::isIncomplete() {
  return this->incomplete;
}

void BoxType::setIncomplete(bool ic) {
  this->incomplete = ic;
}

int BoxType::getFieldCount() {
  return this->fields_hash.size();
}

std::string BoxType::getName() {
  return this->name;
}

int BoxType::getLine(){
  return this->line;
}

int BoxType::getColumn(){
  return this->column;
}

void BoxType::setLine(int l){
  this->line=l;
}

void BoxType::setColumn(int c){
  this->column=c;
}


void BoxType::check() {
  if(this->getFieldCount()==0){
    program.error("tipo box '"+name+"' sin campos",line,column);
    return;
  }

  // Verificar los campos fijos
  for (std::list<BoxField*>::iterator FieldIt= this->fixed_fields.begin();
       FieldIt != this->fixed_fields.end(); FieldIt++){
    // Verificar tipo 
    StringType *cast_tboxfield= dynamic_cast<StringType*>((*FieldIt)->type);
    if(((*FieldIt)->type== &(VoidType::getInstance())) or cast_tboxfield){
      std::string error="campo '"+((*FieldIt)->name)+"' del tipo '"
        +this->toString()
        +"' no puede ser de tipo '"+ (*FieldIt)->type->toString();
      program.error(error,(*FieldIt)->line,(*FieldIt)->column);
      continue;
    }
    // Si es type box hacer check y ver si no llega a mi
    BoxType *cast_tbox= dynamic_cast<BoxType*>((*FieldIt)->type);
    ArrayType *cast_tarray= dynamic_cast<ArrayType*>((*FieldIt)->type);
    if(cast_tarray) (*FieldIt)->type->check();
    if(cast_tbox ){
      // Verificar que el box ha sido declarado
      if( !cast_tbox->isIncomplete() ){
        // Verificar si existen ciclos 
        if( this->reaches(*( dynamic_cast<BoxType*>((*FieldIt)->type) )) )
          program.error("tipo '"+(*FieldIt)->type->toString()+
                        "' tiene referencia ciclica a traves del campo '"
                        +((*FieldIt)->name)+"'",(*FieldIt)->line,
                        (*FieldIt)->column);
      }else{
        program.error("tipo '"+(*FieldIt)->type->toString()+"' no ha sido definido",
                      (*FieldIt)->line,(*FieldIt)->column );
      }
    }
  }

  // Verificar los campos de la parte variant
  for (std::list<BoxField*>::iterator FieldIt= this->variant_fields.begin();
       FieldIt != this->variant_fields.end(); FieldIt++){
    // Verificar tipo 
    StringType *cast_tboxfield= dynamic_cast<StringType*>((*FieldIt)->type);
    if(((*FieldIt)->type== &(VoidType::getInstance())) or cast_tboxfield){
      std::string error="campo '"+((*FieldIt)->name)+"' del tipo '"
        +this->toString()
        +"' no puede ser de tipo '"+ (*FieldIt)->type->toString();
      program.error(error,(*FieldIt)->line,(*FieldIt)->column);
      continue;
    }
    // Si es type box hacer check y ver si no llega a mi
    BoxType *cast_tbox= dynamic_cast<BoxType*>((*FieldIt)->type);
    ArrayType *cast_tarray= dynamic_cast<ArrayType*>((*FieldIt)->type);
    if(cast_tarray) (*FieldIt)->type->check();
    if(cast_tbox ){
      // Verificar que el box ha sido declarado
      if( !cast_tbox->isIncomplete() ){
        // Verificar si existen ciclos 
        if( this->reaches(*( dynamic_cast<BoxType*>((*FieldIt)->type) )) )
          program.error("tipo '"+(*FieldIt)->type->toString()+
                        "' tiene referencia ciclica a traves del campo '"
                        +((*FieldIt)->name)+"'",(*FieldIt)->line,
                        (*FieldIt)->column);
      }else{
        program.error("tipo '"+(*FieldIt)->type->toString()+"' no ha sido definido",
                      (*FieldIt)->line,(*FieldIt)->column );
      }
    }
  }
  
  // Si calcular offsets despues 

}


bool BoxType::reaches(BoxType& box) {
  if(*(this)==box){
    return true;
  }
  bool reachable= false;
  for (std::list<BoxField*>::iterator FieldIt= box.getFFields().begin();
       FieldIt != box.getFFields().end(); FieldIt++){ 
    BoxType *cast_tbox= dynamic_cast<BoxType*>((*FieldIt)->type);
    if(cast_tbox ){
      // Verificar que el box ha sido declarado
      if( !cast_tbox->isIncomplete() ){
        reachable= reachable or this->reaches(*cast_tbox);
        if(reachable) return true;
      }
    }
  }
  return false;
}

std::list<BoxField*> BoxType::getFFields(){
  return fixed_fields;
}

std::list<BoxField*> BoxType::getVFields(){
  return variant_fields;
}

std::string BoxType::toString() {
  return std::string("box "+this->name);
}

bool BoxType::alwaysByReference() {
  return true;
}
