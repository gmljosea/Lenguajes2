#include <list>
#include "expression.hh"

// Nos basamos en el ejemplo del Apéndice A del Compilers para diseñar nuestras
// clases.

/**
 * Representa una instrucción o una secuencia de instrucciones en el lenguaje
 * Devanix.
 */
class Statement {
  // El número de contexto asignado durante el parseo.
  int scope_number;
  // Apunta a la instrucción que contiene a esta. Si nadie la contiene entonces
  // es null.
  Statement *enclosing;

public:
  Statement(int scope) {
    this.scope_number = scope;
  }
};

class Null : public Statement {
};

class Sequence : public Statement {
  Statement *stmt1;
  Statement *stmt2;
};

class MultipleAsignment : public Statement {
  list<BasicAsignment> *asgs;
};

class SingleAsignment : public Statement {
  Variable *var;
  ArrayIndex *acc;
  Expression *exp;
}

class Selection : public Statement {
};

class If : public Selection {
  Expression *cond;
  Statement *stmt;
};

class Else : Selection {
  Expression *cond;
  Statement *stmt_true;
  Statement *stmt_false;
}

class Iteration : Statement {
  // Etiqueta (para el break y next)
};

class IntegerFor : Iteration {
  Expression *lowerb;
  Expression *upperb;
  Expression *step;
  Statement *stmt;
  // Símbolo de la variable de iteración
};

class ArrayFor : Iteration {
  // Símbolo de la variable de iteración
  // Símbolo del arreglo
  Statement *stmt;
};

class While : Iteration {
  Expression *cond;
  Statement *stmt;
};

class Return : Statement {
  Expression *exp;
  // Símbolo de la función correspondiente
};

class Break : Statement {
  // Etiqueta del ciclo a romper
};

class Next : Statement {
  // Etiqueta del ciclo a continuar
};
