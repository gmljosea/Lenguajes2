#include <iostream>
#include "program.hh"
#include "type.hh"

extern Program program;
// Type
int Type::getSize() {
  std::cout << "size of " << this->toString() << " is " << std::to_string(size) << std::endl;
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
  return dynamic_cast<StringType*>(&t) != NULL;
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
    return *(this->basetype) == *(ta->getBaseType());
      //      && this->length == ta->getLength();
  } else {
    return false;
  }
}

std::string ArrayType::toString() {
  return std::string(this->basetype->toString()+" array[]");
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
void BoxType::addFixedField(Type* type, std::string name,int l,int col) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->line=l;
  field->column=col;
  field->offset = 0;
  field->grouped = false;
  field->groupnum = 0;
  this->fixed_fields.push_back(field);
  this->fields_hash[name] = field;
}

void BoxType::addVariantField(Type* type, std::string name, bool grouped,
			      int l,int col) {
  BoxField* field = new BoxField();
  field->type = type;
  field->name = name;
  field->line=l;
  field->column=col;
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

bool BoxType::areOffsetsDone(){
  return this->offsetsDone;
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

void BoxType::calcOffsets(){

  // Calcular offsets de los campos fijos
  for (std::list<BoxField*>::iterator FieldIt= this->fixed_fields.begin();
       FieldIt != this->fixed_fields.end(); FieldIt++){
    BoxType *cast_tbox= dynamic_cast<BoxType*>((*FieldIt)->type);
    if(cast_tbox and !cast_tbox->areOffsetsDone())
      cast_tbox->calcOffsets();

    int align =(*FieldIt)->type->getAlignment();
    // los box en conjunto se alinean de acuerdo al campo cuya alineación sea
    // máxima, por ejemplo, un box que solo tenga bools, se alineará a 1,
    // pero si contiene varios bools y un int, se alineará a 4
    this->alignment = align > this->alignment ? align : this->alignment;

    this->size += (align - (this->size % align)) % align;
    (*FieldIt)->offset = this->size;
    this->size += (*FieldIt)->type->getSize();
  }

  int offset=this->size;
  int maxAlign = 0;
  int maxSizeVariant=this->size;
  int group=-1;
  // Calcular offsets de los campos variantes
  for (std::list<BoxField*>::iterator FieldIt= this->variant_fields.begin();
       FieldIt != this->variant_fields.end(); FieldIt++){

    if((*FieldIt)->grouped){
      // Si es una nueva agrupacion
      if(group!=(*FieldIt)->groupnum){
        //guardo el num de agrupacion y se reinicia el offset
        group= (*FieldIt)->groupnum;
        offset=this->size;
      }
    }else{
      // Si el campo no pertenece a una agrupacion se reinicia
      offset=this->size;
    }

    BoxType *cast_tbox= dynamic_cast<BoxType*>((*FieldIt)->type);
    // Si el tipo del campo es un box asegurar que el tamaño este calculado
    if(cast_tbox and !cast_tbox->areOffsetsDone())
      cast_tbox->calcOffsets();

    int align =(*FieldIt)->type->getAlignment();
    this->alignment = align > this->alignment ? align : this->alignment;

    offset += (align - (offset % align)) % align;
    (*FieldIt)->offset = offset;
    offset += (*FieldIt)->type->getSize();

    if(offset>maxSizeVariant) maxSizeVariant= offset;
  }
  this->size= maxSizeVariant;
  this->offsetsDone=true;
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
        +"' no puede ser de tipo '"+ (*FieldIt)->type->toString()+"'";
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
        +"' no puede ser de tipo '"+ (*FieldIt)->type->toString()+"'";
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

void BoxType::print(){
  std::cout << "box " << this->name << std::endl;
}

void BoxType::printDetail() {
  std::cout <<"Tipo Box: "<<name<<" ("<< line << ":" << column << ")"<< std::endl;
  std::cout << "Campos fijos: "<< std::endl;
  std::list<BoxField*>::iterator it = fixed_fields.begin();
  if(it == fixed_fields.end())
    std::cout << "  No posee campos fijos"<< std::endl;
 
  for (; it != fixed_fields.end(); it++) {
    std::cout << "  Campo: "<<(**it).name<< std::endl;
    std::cout << "    Tipo: " << (**it).type->toString();
    std::cout <<" ("<<(**it).line<<":"<<(**it).column<<")"
      " [Offset: "<<(**it).offset<< "]" << std::endl;
  }
  std::cout << "Campos variantes: "<< std::endl;
  it = variant_fields.begin();
   if(it == variant_fields.end())
    std::cout << "  No posee campos variantes"<< std::endl;

   for (; it != variant_fields.end(); it++) {
     std::cout << "  Campo: "<<(**it).name<< std::endl;
     std::cout << "    Tipo: " << (**it).type->toString();
       std::cout <<" ("<<(**it).line<<":"<<(**it).column<<")"
       " [Offset: "<<(**it).offset<< "]" << std::endl;
   }
   std::cout << "Tamaño: " << this->size   << std::endl;
   std::cout << "Alineación: " << this->alignment << std::endl;

   std::cout << std::endl;
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
