#include "symbol.hh"
#include<iostream>

int main(){
  /*Simular anidamiento de simbolos*/
  SymTable tabla;
  std::cout << "Alcance: " << tabla.current_scope()<<std::endl;
  
  /*Alcance mas externo, lugar en donde se declaran var globales y func*/
  SymVar var1("var1",10,1,false);
  SymVar var2("var2",11,1,false);
  SymVar param1("param1",14,15,true);
  SymVar param2("param2",14,19,true);
  listSymPairs offunc1;
  offunc1.push_back(std::pair<PassType,SymVar*>(PassType::normal,&param1));
  offunc1.push_back(std::pair<PassType,SymVar*>(PassType::reference,&param2));
  SymFunction func1("func1",14,1,&offunc1);
  CharType tipofunc;
  func1.setType(tipofunc);

  /*Introducir los simbolos en la tabla*/
  tabla.insert(&var1);
  tabla.insert(&var2);
  tabla.insert(&func1);

  /*Siguiente prueba: Un anidamiento mas*/
  if(true){
    tabla.enter_scope();
    std::cout << "Alcance: " << tabla.current_scope()<<std::endl;
    SymVar varif("varif",27,2,false);
    tabla.insert(&varif);
    tabla.leave_scope();
  }

  /*Pruebas para los lookups*/
  SymVar *var1b= tabla.lookup_variable("var1");
  SymVar *var2b= tabla.lookup_variable("var2");
  SymFunction *func= tabla.lookup_function("func1");

 
  /* var1b->print();
     var2b->print();*/
  func->print();
}
