# Lenguaje Devanix

+ potencia, base,exponente nan 
+ map
+ rec cola
+ bool difuso
+ break no hace daño. next/continue. restart

* * *

En este curso escribiremos un compilador para un sencillo lenguaje de programación
imperativo llamado Devanix.
La sintaxis del lenguaje se inspira fuertemente en la del lenguaje C.

## Consideraciones léxicas

Las palabras reservadas del lenguaje no pueden ser usadas como identificadores
(de variables, funciones, etc) y son las siguientes:

    if else for in step while int char bool float array string box 
    void variant: true false

Los identificadores son secuencias de caracteres alfanumericos o 
el símbolo *underscore* (_), y no pueden comenzar por un caracter numérico.

Los identificadores son sensibles a mayúsculas, es decir, los nombres **ejemplo**,
**Ejemplo**, **EJEMPLO** y **EjEmpLo** son todos distintos.

Los números enteros se pueden escribir en base decimal, base binaria o base
hexadecimal.
Los números en base decimal son cualquier secuencia de dígitos entre 0 y 9.
Los números binarios se escriben comenzando con **b:** seguido de caracteres 0 o 1.
Los números hexadecimales se escriben comenzando con **x:** seguido de dígitos
del 0 al 9 y los caracteres alfabéticos desde la letra A hasta la F, sin
importar si son mayúsculas o minúsculas.

Los números decimales (entiéndase como números con parte fraccionaria, no como
números en base decimal) se pueden escribir únicamente en base decimal.
Consisten de una secuencia no vacía de dígitos del 0 al 9 para la parte entera,
un punto (**.**), y una secuencia no vacía de dñígitos del 0 al 9 para la parte
fraccionaria.

Ejemplos de números:

    2012           -. entero en base decimal
    b:11111011100  -. el mismo entero pero en base 2
    x:7DC          -. el mismo entero pero en base 16
    2012.01        -. número decimal

    -- Nota: cómo escribir strings? como se hace en C.

Los comentarios de una línea se comienzan con **-.** y cubren el resto de la línea.
Los comentarios multilínea se comienzan con **--.** y se terminan con **.--**.

. ; { } ( ) + * / % [ ] = -. --.  .-- _ :

## Gramática

Esto viene cuando estemos más avanzados en el diseño.

## Estructura del programa

## Reglas de alcance

## Tipos

Los tipos que manejará el lenguaje son:

Escalares:

*    **char** 
*    **bool**
*    **int**
*    **float**

Compuestos:

*    **array**
*    **string**
*    **box**

Especial:

*    **void**

### Tipos **box**

Los tipos **box** son el único mecanismo para que el usuario defina sus propios tipos.

Un tipo **box** básico sin campos variantes se declara de la siguiente manera:

    box nombre {
        tipo variable
	tipo variable
	...
    }

Las variables pueden ser de cualquier tipo excepto void.

Un tipo **box** con campos variantes se declara de la siguiente manera:

    box nombre {
       variant:
        tipo variable
	tipo variable
	...
    }
    
Se pueden agrupar campos variantes usando llaves para declarar que todas las variables
del grupo están activas al mismo tiempo:

    box nombre {
       variant:
        tipo variable1
	...
	{
	    tipo variable2
	    tipo variable3
	    ...
	}
	tipo variablex
	...
    }
    
También se pueden combinar campos fijos con campos variantes:

    box nombre {
        tipo variable    -. campos fijos
	tipo variable
	...
       variant:
        tipo variable    -. campos variantes
	tipo variable
    }

Por ejemplo, el usuario podría definir el siguiente tipo para representar elementos
atómicos. De entre los campos variantes solo uno puede estar activo a la vez, en este
caso está activo el campo **tiempo_de_vida** o el grupo de campos **molecula_comun**
y **concentracion**.

    box elemento {
        char array[2] nombre
	int numero_atomico
	float peso_atomico
	bool metalico
	bool ocurrencia_natural
       variant:
        float tiempo_de_vida
	{
	    char array[10] molecula_comun
	    float concentracion
	}
    }
	    

## Intrucciones

Las instrucciones disponibles en Devanix son:

*   Asignación
*   Declaración de variable
*   Selección condicional **if**/**else**
*   Iteración acotada **for** .. **in**
*   Iteración condicional **while**
*   Llamada a función de tipo void \*
*   Declaración de **box** \*

\* Lo estoy agregando a lo loco, tenemos que discutirlo.

El caracter utilizado para la terminación de instrucciones sera el punto y coma 
(**;**) o el salto de línea.

## Estructuras de control

### Condicional

Sintaxis:

       if condición {
          instrucciones
       } else {
          instrucciones
       }

La condición debe ser una expresión de tipo booleano.
Se puede obviar la parte del else si se quiere.

### Iteración acotada 

Sintaxis:

       for var in num..num2 step num3 {
           instrucciones 
       }

*   La variable **var** se declara automáticamente de tipo **int** y solo es visible
    dentro del bloque de la iteración.
*   La variable **var** no puede ser modificada por el programador así que se
    considera un error intentar asignarla o pasarla por referencia a una función.
*   Los limites del **for** y el aumento (**num**, **num2**, **num3**) pueden ser
    expresiones y serán evaluadas una sola vez antes del primer ciclo.
    Debe evitar que las expresiones tengan efectos de borde debido a que la
    evaluacion de las expresiones (*num*,*num2*,*num3*) no garantiza ningun orden
    en particular.
*   El aumento debe ser un numero de tipo **int** y a su vez debe ser positivo.
*   Los límites se chequean antes de comenzar la primera iteración.
    En caso de que los límites sean vacíos(**num < num2**) no se ejecutará ningún
    ciclo.
*   Dentro del bloque **for** se puede usar la palabra reservada **break**, para
    finalizar la iteración.
*   También se puede usar la palabra reservada **next**. Su efecto es "saltar"
    las instrucciones que le siguen para entrar a la siguiente iteracion del mismo
    for.   

Existe una variante de esta estructura que permite iterar sobre arreglos de manera
sencilla:

    for var in arreglo {
        instrucciones
    }

*    La variable *var* asume el tipo del tipo contenido por el arreglo.
*    El ciclo se ejecuta una vez por cada elemento del arreglo, con la variable *var*
     asumiendo el valor de la posición correspondiente del arreglo.
*    Se pueden usar las palabras reservadas **break** y **netx**, estos actuan 
     igual que para el for.

pregunta!!--> se puede modificar el arreglo mientras se recorre???

### Iteración condicional

Sintaxis:

       while cond {
          ...
       }
       
*   La condición **cond** debe ser una expresión de tipo booleano.
*   La condición se chequea al comienzo de cada iteración.
*   Es permitido utilizar las palabras reservadas **break** y **next**

DESCRIBIR LAS ETIQUETAS 

## Variables 

    -- Nota: falta definir cómo asignar a arreglos y box.
       Yo apoyaría la idea de definir una expresión de tipo arreglo
       y otra de tipo box. Ej:
       tipo array[5] bla = [1,2,3,4,5]
       Variables globales y sus implicaciones?

### Variables de tipos básicos y **box**

Sintaxis para la declaración de variables de tipos básicos:

    tipo nombreVar              -. Declaración sin inicialización
    tipo nombreVar = expresion  -. Declaración con inicialización
    
*   El tipo debe ser cualquier tipo básico o nombre de **box** definido por el 
    usuario.
*   Si no es inicializada por el usuario no tendrá ningun valor predeterminado y no
    se podrá predecir qué valor asumirá.
*   El tipo de la expresión debe coincidir con el tipo de la declaración.

Devanix permite el uso de declaraciones simultáneas, es decir, más de una variable
puede ser declarada en la misma línea. También es posible inicializarlas
simultáneamente. Por ejemplo:

    float num,a,b;
    int c = 5,d = 6, e = 5;
    int c,d,e= 5,6,5;
    int c,d,e= 1

    c,d= 1,2 
    c=1,d=2 no 
 
    
    -- Nota: qué pasa con los efectos de borde? Comportamiento no determinado?

### Variables de tipo **array**

Sintaxis:

    tipo array[num] nombreVar
    tipo array[num] nombreVar = expresión

*   El tipo del contenido del array debe ser cualquier tipo escalar, string o nombre de **box**
    definido por el usuario.

## Funciones

Las funciones en *Devanix* poseen la siguiente sintaxis:

       tipo nombreFunc (tipo arg1, tipo2 arg2 ... ){
          instrucciones
          [return exp]
       }

La instrucción **return** es obligatoria por cualquier camino
cuando el tipo de la función no es **void**. 

El pasaje de parametros es de dos modalidades: Para los tipos escalares el pasaje 
es por valor y para los compuestos el pasaje es por referencia.
Sin embargo el usuario tiene la opción de realizar el pasaje de tipos escalares por
referencia usando el símbolo reservado **$** antes del tipo del argumento.

Ejemplo:

       int calcularTotal($int monto, $char modo){
          ...
       }

Usar el símbolo **$** para tipos no escalares produce un error de compilación.

También es posible el pasaje en modo *sólo lectura* de los tipos compuestos, para
lo cual se proporciona el simbolo reservado **$$**.
El pasaje a bajo nivel sigue siendo por referencia pero se prohibe realizar una
asignación a la variable o pasarla como referencia a otra función.

Ejemplo:

       void rentarPelicula($$pelicula peli, $$cliente client1) {
          pelicula peli2
          ...
          peli = peli2     -. ¡Error de compilación!
       }

Las funciones pueden ser declaradas en cualquier orden y son visibles para cualquier
otra función declarada en el programa. El lenguaje permite recursión.

## Compatibilidad y equivalencia de tipos

## Asignación

## Expresiones

Las expresiones en Devanix son blah blah ...

Los operadores proporcionados para cada tipo en el lenguaje Devanix se enumeran 
a continuación. Estos sólo se pueden aplicar entre objetos del mismo tipo. Si
desea, por ejemplo, realizar la suma entre un objeto de tipo int y otro float, debe 
realizar la conversión explícita de alguno de los dos tipos para poder efectuar la adición.  
 

### Para los tipos int y float

Los operadores binarios son: 

*    **+**  Operador para la suma 
*    **-**  Operador para la resta 
*    *****  Operador para la multiplicacion
*    **/**  Operador para la división
*    **%**  Operador para el modulo
*    **=**  Operador para la igualdad
*    **>**  Mayor que
*    **>=** Mayor igual 
*    **<**  Menor
*    **<=** Menor igual

Operador unario:

*    **-** Menos unario.

### Para el tipo bool 

Los operadores binarios son:

*    **and** Conjunción lógica
*    **or**  Disyunción lógica
*    **=**   Igualdad

Operador unario:

*    **not** Negación lógica 

### Para los tipos array, string y char

El operador binario que se proporciona para estos tipos es únicamente el de igualdad (**=**)

## Funciones de librería

+ todo lo que este en este documento de diseño sera evaluado para la primera entrega 
+ No se va a evaluar la suma etc etc
+ no listas aun
+ aclarar que puede devolver una funcion