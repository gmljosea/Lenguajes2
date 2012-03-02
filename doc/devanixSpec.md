# Lenguaje Devanix
* * *

En este curso escribiremos un compilador para un sencillo lenguaje de programación
imperativo llamado Devanix.
La sintaxis del lenguaje se inspira fuertemente en la del lenguaje C.

## Consideraciones léxicas

Las palabras reservadas del lenguaje no pueden ser usadas como identificadores
(de variables, funciones, etc) y son las siguientes:

    if else for in step while int char bool float array string box
    void variant true false return break next write write writeln
    read and or not

Los símbolos reservados del lenguaje son los siguientes:

    { } ( ) [ ] ; , .. $ $$ : + * / % = < > <= >= -. --.  .-- .

Los identificadores son secuencias de caracteres alfanumericos o
el símbolo *underscore* (_), y no pueden comenzar por un caracter numérico.

Los identificadores son sensibles a mayúsculas, es decir, los nombres **ejemplo**,
**Ejemplo**, **EJEMPLO** y **EjEmpLo** son todos distintos.

Los números enteros se pueden escribir en base decimal, base binaria o base
hexadecimal.
Los números en base decimal son cualquier secuencia de dígitos entre 0 y 9 que
comience con un dígito entre 1 y 9.
Los números binarios se escriben comenzando con **b:** seguido de caracteres 0 o 1.
Los números hexadecimales se escriben comenzando con **x:** seguido de dígitos
del 0 al 9 y los caracteres alfabéticos desde la letra A hasta la F, sin
importar si son mayúsculas o minúsculas.

Los números decimales (entiéndase como números con parte fraccionaria, no como
números en base decimal) se pueden escribir únicamente en base decimal.
Consisten de una secuencia no vacía de dígitos del 0 al 9 para la parte entera,
un punto (**.**), una secuencia no vacía de dígitos del 0 al 9 para la parte
fraccionaria.
Opcionalmente, se puede agregar el exponente escribiendo "E" ó "e" seguido de
uno o más dígitos del 0 al 9. Se puede especificar el signo del exponente con
a su vez tener un símbolo de + ó - precediendo el exponente.
El exponente significa que el número que lo antecede se multiplica por diez (10)
elevado al número que lo precede.

Ejemplos de números:

    2012           -. entero en base decimal
    b:11111011100  -. el mismo entero pero en base 2
    x:7DC          -. el mismo entero pero en base 16
    2012.01        -. número decimal
    20.1201e2      -. el mismo decimal con exponente

Los comentarios de una línea se comienzan con **-.** y cubren el resto de la línea.
Los comentarios multilínea se comienzan con **--.** y se terminan con **.--**.

## Gramática

!!! Coming soon

## Estructura del programa

Un programa en Devanix se compone de una secuencia de declaraciones de variables
globales y de funciones. Debe haber una función llamada **main**, de tipo
**int** y que no reciba argumentos; esta función sirve como punto de entrada al
programa.
El lenguaje no permite funciones anidadas por lo que el alcance global es el
único lugar en donde se pueden declarar.

## Reglas de alcance

El alcance en Devanix es estático y soporta anidar alcances.

El alcance mas externo corresponde a las declaraciones globales, luego cada
definición de función e instrucciones If, While y For introducen nuevos
alcances.

Los alcances internos ocultan los alcances más externos.

Las funciones y box solo se pueden declarar en el alcance global y no importa el
orden en que se declaren, todas son visibles entre sí. Además, cada una
vive en su propio espacio de nombres distinto al de las variables. Esto quiere
decir que en un alcance pueden existir una variable, una función y un box
con el mismo nombre. Una declaración de variable nunca oculta una función
o box.

Las variables se pueden declarar en cualquier alcance, pero al contrario de
las funciones, las variables sólo son visibles para las líneas que siguen
a la declaración.

Las variables declaradas en el alcance global tienen la restricción de que
solo pueden ser inicializadas con expresiones constantes y variables globales
previamente definidas. No se pueden utilizar llamadas a funciones para
inicializar variables globales.

No se pueden declarar dos funciones o box con el mismo nombre.

No se pueden declarar dos variables con el mismo nombre en el mismo alcance,
sin importar que tengan el mismo tipo.

## Tipos

Los tipos que manejará el lenguaje son:

Escalares:

*    **int**:
     Tipo entero con signo. De al menos 32 bits.
*    **float**:
     Tipo punto flotante IEEE-754 de al menos 32 bits.
*    **char**:
     Tipo caracter ASCII, de al menos 1 byte.
*    **bool**:
     Tipo booleano (true o false). Ocupa al menos 1 byte.

Compuestos:

*    **array**:
     Colección homogénea linealmente indexada.
*    **box**

Especial:

*    **string**:
     Secuencia de caracteres ASCII inmutable.
*    **void**:
     Tipo especial que solo puede usarse en funciones. Representa el vacío,
     la nada.

### Tipos **box**

Los tipos **box** son el único mecanismo para que el usuario defina sus propios tipos.
Los box son una colección de campos de tipos heterogéneos. Existen dos tipos de
campos: normales y variantes.

Dado un box cualquiera, cada campo normal ocupa su propia región de memoria,
mientras que la colección de campos variantes comparten la misma memoria.

Los campos variantes son inseguros. Es responsabilidad del programador asegurarse
de que no se usen de forma indebida. Por ejemplo, evitar asignar a un campo
variante de tipo int, e inmediatamente leer otro campo variante de tipo float.
Los bits serán reinterpretados y no hay garantías.

Un tipo **box** básico sin campos variantes se declara de la siguiente manera:

    box nombre {
        tipo variable
	tipo variable
	...
    }

Las variables pueden ser de cualquier tipo excepto void, inclusive otro box.
No puede existir un campo del box que sea del mismo tipo que el propio box, pues
sería una recursión infinita.

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

## Variables

Las variables se pueden declarar de cualquier tipo excepto **void**.

Los argumentos declarados de una función se comportan como variables declaradas
en la primera línea del bloque de la función.

### Declaración de variables

Sintaxis para la declaración de variables de tipos básicos:

    tipo nombreVar ;              -. Declaración sin inicialización
    tipo nombreVar = expresion ;  -. Declaración con inicialización

*   El tipo debe ser cualquier tipo escalar, **string** o nombre de **box**
    definido por el usuario.
*   Si no es inicializada por el usuario no tendrá ningun valor predeterminado
    y no se podrá predecir qué valor asumirá.
*   Las variables de tipo **string** deben ser inicializadas siempre.
*   El tipo de la expresión debe coincidir con el tipo de la declaración.

Devanix permite el uso de declaraciones simultáneas, es decir, más de una
variable puede ser declarada en la misma línea. También es posible
inicializarlas simultáneamente. Por ejemplo:

    float a,b;              -. dos variables sin inicializar
    int c = 5,d = 6, e = 5; -. declaraciones con inicializacion
    int c,d,b = 1;          -. mezcla de declaraciones inicializadas y no inicializadas

Las sintaxis de variables de tipo array varía ligeramente:

    tipo array[num] nombreVar ;
    tipo array[num] nombreVar = expresión ;

*   El tipo del contenido del array debe ser cualquier tipo escalar, **string**
    o nombre de **box** definido por el usuario.
*   **num** debe ser una constante entera en base decimal.

Ejemplos:

    int array[20] a;     -. arreglo de 20 enteros
    float rarray[100] b; -. arreglo de 20 flotantes

## Arreglos

Los arreglos en Devanix son unidimensionales y se indexan comenzando por el 0.

Los arreglos son de tamaño fijo y constante a tiempo de compilación. No existen
arreglos dinámicos.

## Strings (Cadenas de caracteres)

## Funciones

Las funciones en *Devanix* poseen la siguiente sintaxis:

       tipo nombreFunc (tipo arg1, tipo2 arg2 ... ) {
          instrucciones
          [return exp]
       }

El tipo de la función solo puede ser de un tipo escalar o **void**.

Ejemplo:

       int calcularTotal($int monto, $char modo) {
          ...
       }

La instrucción **return** es obligatoria por cualquier camino cuando el tipo de
la función no es **void**.

El pasaje de parametros es de dos modalidades: Para los tipos escalares el
pasaje es por valor y para los compuestos el pasaje es por referencia.
Sin embargo el usuario tiene la opción de realizar el pasaje de tipos escalares
por referencia usando el símbolo reservado **$** antes del tipo del argumento.
Usar el símbolo **$** con argumentos de tipo **box** es redundante.



También es posible el pasaje en modo *solo lectura* de los argumentos, para
lo cual se proporciona el simbolo reservado **$$**.
El pasaje a bajo nivel sigue siendo por referencia pero se prohibe realizar una
asignación a la variable o pasarla a una función por otro modo que no sea
solo lectura.

Ejemplo:

       void rentarPelicula($$pelicula peli, $$cliente client1) {
          pelicula peli2;
          ...
          peli = peli2;    -. ¡Error de compilación!
       }

Se admite recursión.

La instrucción **return** permite finalizar la ejecución de la función y
devolver un valor. Si la función es void, se omite la expresión. Por ejemplo:

    return 1;  -. Retornar el valor entero 1
    return;    -. Cuando la función es void, se omite la expresión

## Invocación de funciones

Se pueden utilizar llamadas a funciones en cualquier expresión, excepto las
funciones declaradas con tipo **void**. Por ejemplo:

    a = test(1,2,3) + 1;

Una función también se puede invocar como si fuera una instrucción.
Esto permite invocar funciones de tipo **void**. Por ejemplo:

    print_hello("hello");

## Equivalencia y compatibilidad de tipos

!!! Coming soon

## Asignación

Sintaxis:

    a1, a2, a3, ..., an = e1, e2, e3, ..., en;

Lo que produce el efecto de asignar simultáneamente a las variables a1 .. an
el resultado de evaluar las expresiones e1 a en.

El efecto es semánticamente equivalente a lo siguiente:

    t1 = e1
    t2 = e2
    ...
    tn = en
    a1 = t1
    a2 = t2
    ...
    an = tn

Es decir, asignar todas las expresiones a variables temporales, y al final
copiar cada variable temporal a la variable real correspondiente.
El orden en que se evalúan las expresiones no es determinístico, así que debe
tenerse cuidado con los efectos de borde.

## Estructuras de control

### Selección condicional

Sintaxis:

       if condición {
          instrucciones
       } else {
          instrucciones
       }

La condición debe ser una expresión de tipo booleano.
Se puede omitir la parte del else si se quiere.

### Iteración acotada

Sintaxis:

       label: for var in num..num2 step num3 {
           instrucciones
       }

*   La variable **var** se declara automáticamente de tipo **int** y solo es
    visible dentro del bloque de la iteración.
*   La variable **var** no puede ser modificada por el programador así que se
    considera un error intentar asignarla o pasarla por referencia a una función.
*   Los limites del **for** y el aumento (**num**, **num2**, **num3**) pueden ser
    expresiones y serán evaluadas una sola vez antes del primer ciclo.
    Debe evitar que las expresiones tengan efectos de borde debido a que la
    evaluacion de las expresiones (*num*,*num2*,*num3*) no garantiza ningun orden
    en particular.
*   El aumento debe ser un número de tipo **int** y a su vez debe ser positivo.
*   Los límites se chequean antes de comenzar la primera iteración.
    En caso de que los límites sean vacíos(**num < num2**) no se ejecutará ningún
    ciclo.
*   **label** es una etiqueta opcional que será explicada en la sección **Etiquetas**. 

Existe una variante de esta estructura que permite iterar sobre arreglos de
manera sencilla:

    label: for var in arreglo {
        instrucciones
    }

*    La variable **var** asume el tipo del tipo contenido por el arreglo.
*    El ciclo se ejecuta una vez por cada elemento del arreglo, con la variable *var*
     asumiendo el valor de la posición correspondiente del arreglo.
*    La variable **var** no es de solo lectura; se permite modificar los elementos
     del arreglo.

### Iteración condicional

Sintaxis:

       while cond {
          ...
       }

*   La condición **cond** debe ser una expresión de tipo booleano.
*   La condición se chequea al comienzo de cada iteración.

### Etiquetas, **break** y **next**

Cualquier iteración (While o For) se puede etiquetar. Por ejemplo:

    loop1: while cond {
        ...
    }

Se pueden anidar iteraciones, cada una con su etiqueta, pero todas las etiquetas
deben ser distintas.

Dentro de una iteración se puede usar la instrucción **break** para finalizar
incondicionalmente la iteración. Por defecto un **break** rompe la iteración
más interna que contiene el **break**. Opcionalmente, se puede hacer un
**break** con etiqueta. En este caso se rompe la iteración cuya
etiqueta sea igual. Por ejemplo:

    loop1: while true {
      loop2: while true {
        ...
	break;       -. rompe la iteración interna
        ...
	break loop1; -. rompe la iteración más externa (cuya etiqueta es loop1)
      }
    }

El **next** funciona de igual manera que el **break**. Pero en vez de finalizar
la iteración salta incondicionalmente al siguiente ciclo.

## Expresiones

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


## Chequeos dinámicos

!!! Coming soon
