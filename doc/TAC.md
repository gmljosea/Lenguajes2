# TAC usado en el compilador
* * *

Para denotar variables temporales, hemos utilizados identificadores de la forma

    \.t[0-9]+
    
Y para los labels hemos usado identificadores de la forma:

    l[0-9]+:

Para las asignaciones usamos la mismas instrucciones del TAC presentado en clase:

     a := b op c
     a := op b
     a := b
     a := *b    
    *a := b
     a := &b    
     a := b[c]
     a[b] := c

Para los saltos también usamos las mismas instrucciones:

     goto label
     if a op b goto label
     ifnot a op b goto label
     
Para las llamadas a funciones expandimos la instrucción **param** para indicar
el tipo de pasaje de parámetro: por valor o por referencia.

También agregamos la instrucción **prologue** para indicar el comienzo de una
función, y luego poder generar el prólogo con mayor facilidad.

Nosotros no distinguimos entre llamar a una función y descartar o no su valor
por retorno. Las llamadas a funciones siempre van en una asignación a alguna
variable, temporal o no. Si la función fuera void, se asignaría a un temporal
que luego no sería usado.

     param val a    -- Pasar parámetro a por valor
     param ref b    -- Pasar parámetro b por referencia
     prologue main  -- Prólogo de la función main
     a := call f n  -- Llamar f con n argumentos
     return
     return a

También incluimos varias instrucciones para manejar I/O, son
instrucciones casi equivalentes a los **write** y **read** en Devanix.

Estas instrucciones llevan adicionalmente el tipo de la operación. Por ejemplo,
no es lo mismo imprimir un entero a un número punto flotante.

    write type a
    writeln type a
    a := read type
    *a := read type
    a[b] := read type

Los operadores disponibles son:

    -- Operadores aritméticos sobre enteros
    a := b + c
    a := b - c  
    a := b * c
    a := b / c
    a := b % c
    a := - b
    
    -- Operadores aritméticos sobre flotantes
    a := b f_sum c
    a := b f_sub c
    a := b f_mul c
    a := b f_div c
    
    -- Operadores relacionales
    b > c
    b >= c
    b == c
    b != c
    b < c
    b <= c
    
    -- Operadores lógicos
    a and b
    a or b
    not a
    
     
 

