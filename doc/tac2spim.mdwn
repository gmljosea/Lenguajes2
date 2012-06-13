# Traducción de TAC a Ensamblable MIPS

Prof. Ernesto Hernández-Novich <<emhn@usb.ve>>

Mayo 2012

## Introducción

Este documento describe como traducir *Three Address Code* (TAC)
intermedio, hacia código ensamblable MIPS R2000 susceptible de ser
ejecutado en SPIM. Está concebido para servidor de material de apoyo al
trabajo práctico de Lenguajes de Programación III (CI-4722) consistente en
la construcción del *back-end* de un compilador simple.

Los archivos resultantes conteniendo ensamblable MIPS deben terminar con
la extensión `'.s'` de manera que puedan ser leídos directamente por SPIM.
Es recomendable combinar la generación de código con la inserción de
comentarios -- personalmente, empleo los comentarios para indicar cuál fue
el TAC traducido, justo antes del fragmento de ensamblable generado.

## Algunas notas sobre MIPS R2000

### Registros de Propósito General

MIPS tiene una arquitectura simple *load/store* en la cual las operaciones
artiméticas operan solamente con registros. Tiene treinta y dos (32)
registros de 32 bits, para propósito general. Los registros están
enumerados del 0 al 31 -- en el lenguaje ensamblable, la expresión $i hace
referencia al i-ésimo registro.

*   El registro 0 (`$0`) es especial siempre contiene el valor cero --
    emplee *siempre* el alias `$zero` para dejar clara la intención.

*   Los registros 1, 26 y 27 (`$1`, `$26` y `$27`) se reservan para uso del
    ensamblador y el ambiente de simulación virtual.

*   El registro 29 (`$29`) corresponde al tope de la pila -- emplee *siempre*
    el alias `$sp` para dejar clara la intención.

*   El registro 30 (`$30`) corresponde al apuntador al registro de
    activación (*frame pointer*) -- emplee *siempre* el alias `$fp` para
    dejar clara la intención.

*   El registro 31 (`$31`) corresponde a la dirección de retorno desde
    el último salto, típicamente empleado para control de llamadas a
    subrutinas -- emplee *siempre* el alias `$ra` para dejar
    clara la intención.

*   El registro 2 (`$2`) es utilizado para contener el resultado de la
    última operación con enteros -- emplee *siempre* el alias `$v0` para
    dejar clara la intención.

*   Los registros (`$f0` a `$f31`) tienen el propósito dual de permitir
    operaciones en punto flotante:

    -   Como treinta y dos (32) registros de punto flotante en
        precisión simple (`float` de 32 bits) -- en este caso se
        usan los registros individuales.

    -   Como diecisés (16) registros de punto flotante en precisión
        doble (`double` de 64 bits) -- en este caso se usan los
        registros *pares* como comienzo de la pareja.

### Origen y destino

Las instrucciones de carga y almacenamiento del MIPS tienen
interpretaciones diferentes en cuanto a la ``dirección'' de movimiento.

*   Las cargas van de derecha a izquierda

        li $t0, 42          # $t0 <-- 42
        lw $t0, $t1         # $t0 <-- $t1

*   Los almacenamientos van de izquierda a derecha

        sw $t0, 8($sp)      # $t0 --> 8($sp)
    
*   La aritmética va de derecha a izquierda

        add $t0, $t1, $t2   # $t0 <-- $t1 + $t2

### Memoria

SPIM aprovecha el orden de bytes de la máquina sobre la cual es ejecutado.
Esto quiere decir que si Ud. utiliza Linux i386/amd64 entonces la
memoria es accesible por bytes en modo *little-endian*, empleando un
esquema plano de direcciones de 32 bits. Como consecuencia:

*   Todas las instrucciones son de 32 bits y *deben* estar alineadas
    en múltiplos de 32 bits.

*   El registro `$sp` siempre apunta al byte *menos* significativo de la
    palabra en el tope de la pila. La pila crece hacia direcciones
    *bajas* de memoria y como el manejo de la pila es *manual*,
    si se desea empilar algún valor es necesario escribir

        sw $i,  -4($sp)       # (-->)
        la $sp, -4(sp)        # (<--)

### Conversión de Tamaño

*   Para convertir un byte, presumiblemente representando un caracter
    o un booleano, a su correspondiente valor entero de 32 bits
    con *extensión de signo*.

        lb $i, foo

*   Para convertir un entero de 32 bits, suponiendo que solamente interesa
    su byte menos significativo presumiblemente para representar un
    caracter o un booleano.

        sb foo, $i
    
### Operaciones de Punto Flotante

Para operar con punto flotante es necesario aprovechar
*pseudo-instrucciones* de MIPS para cargar los valores literales en
registros del coprocesador de punto flotante y completar la operación

Para precisión simple

    li.s  $f0, 0.27
    li.s  $f1, 0.15
    add.s $f2, $f0, $f1

Para precisión doble

    li.d  $f0, 0.27
    li.d  $f2, 0.15
    add.d $f4, $f0, $f2

note que en este caso se usa la *pareja* `$f0-$f1` para contener 0.27,
la *pareja* `$f2-$f3` para contener 0.15, calculando el resultado en
la *pareja* `$f4-$f5`.

La conversión desde, hacia y entre representaciones de punto flotante se
consigue con operaciones especiales

*   Precisión simple a precisión doble

    cvt.d.s $fd, $fs     # (<--) Destinos d y (d+1), (s != d) y (s != d+1)

*   Precisión doble a precisión simple

    cvt.s.d $fd, $fs     # (<--) Destino d, (d != s) y (d != s+1)

*   Entero de 32 bits a precisión simple -- el entero fue copiado como
    patrón de bits hacia algún registro FPU usando `mtc1`

    cvt.s.w $fd, $fs     # (<--) Destino d, (d != s)

*   Entero de 32 bits *con signo* a precisión doble -- el entero fue
    copiado como patrón de bits hacia algún registro FPU usando `mtc1`

    cvt.d.w $fd, $fs     # (<--) Destinos d y (d+1), (s != d) y (s != d+1)

*   Precisión simple a entero de 32 bits *con signo* -- el patrón de bits
    puede ser recuperado como entero hacia algún regisro CPU usando `mfc1`

    cvt.w.s $fd, $fs     # (<--) Destino d, (d != s)

*   Precisión doble a entero de 32 bits *con signo* -- el patrón de bits
    puede ser recuperado como entero hacia algún regisro CPU usando `mfc1`

    cvt.w.d $fd, $fs     # (<--) Destino d, (d != s) y (d != s+1)

## Generación de Código

### Segmentos Text y Data

Un conjunto de declaraciones para datos correspondientes a variables
de programa, variables temporales o constantes a tiempo de compilación,
deben estar precedidas por una línea

        .data

Las secciones de código ensamblable deben estar precedidas por una línea

        .text

### Identificadores y etiquetas

Una variable global *id* del programa original, debe convertirse en la
etiqueta `id` del programa ensamblable. Sin embargo, las variables
locales del programa original **no** será convertida a identificador
alguno, sino que será accedida usando desplazamientos relativos a `$fp`.

Las constantes a tiempo de compilación, numéricas o cadenas, deben ser
identificadas con una etiqueta interna generada a propósito. Note que
debe asegurarse de que las etiquetas internas *jamás* entren en conflicto
con las etiquetas generadas a propósito de los nombres de variable del
programa original -- la técnica usual consiste en que las etiquetas
internas usan un prefijo imposible de generar en el programa original.

Con esta estrategia, la reserva de espacio para variables globales puede
completarse un identificador a la vez -- un identificador global *foo*
que requiera *42* bytes de almacenamiento se traducirá como

        foo: .space 42

Para las cadenas constantes a tiempo de compilación, pueden usarse
`.ascii` o `.asciiz`. Entonces, una cadena como

        "foo, bar, baz\n"

puede definirse con cualquiera de las siguientes formas

        .ascii "foo, bar, baz\n\0"
        .ascii "foo, bar, baz\12\0"
        .asciiz "foo, bar, baz\n"
        .asciiz "foo, bar, baz\12"

A menos que Ud. tenga razones de peso para **no** hacerlo, se sugiere usar
`.asciiz` *siempre* y dejar explícitas cualesquiera secuencias especiales
(en este caso `\n`) en lugar de convertirlas a octal.

Si alguna etiqueta del programa principal está asociada a un tipo de datos
que requiere una alineación especial, puede obligarse esa restricción
empleando la instrucción

        .align n

de manera que la *siguiente* etiqueta (o instrucción) estará ubicada en
una dirección divisible por la *n*-ésima potencia de 2 -- típicamente se
querrá alinear en múltiplos de cuatro bytes, i.e. *n* = 2.

Como ejemplo, suponga las declaraciones globales

        char  foo;
        int   bar, baz[42];
        float qux;

el código ensamblable a generar debería ser similar a

        .data
        # char foo - 1 byte
        foo: .space 1

        # int bar - 4 bytes
             .align 2
        bar: .space 4

        # int baz[42] - 42 * 4 bytes
        baz: .space 168

        # float qux - 8 bytes
        qux: .space 8

Idealmente los comentarios fueron introducidos por el generador de código
a partir de la información en la tabla de símbolo, y le permitirán depurar
mejor su *back-end*. No hay problema en mezclar segmentos de texto y
datos, siempre y cuando la alineación sea adecuada.

### Acceso a valores constantes

Para cargar un valor constante en el i-ésimo registro

        li $i, 42

Nunca está bien cargar el valor cero -- use el registro $zero.

### Acceso a variables globales

Para cargar el valor asociado a una variable global en el i-ésimo
registro, se utiliza directamente la etiqueta definida para el nombre

        lw $i, foo

En este caso, se carga una palabra de 32 bits (un entero)

### Acceso a posiciones de un arreglo o campos de una estructura

El acceso a posiciones de un arreglo o campos de una estructura siempre se
completará empleando desplazamientos a partir de la base. Para el caso de
arreglos o estructuras globales, el fragmento de código será similar a

        la  $i, foo        # base del arreglo/registro global
        ...                # calcular el offset en el registro $j
        add $i,$i,$j       # sumarlo a la base
        lw  $k,($i)        # cargar el valor deseado en $k

El cálculo del offset dependerá del tipo de datos: si se trata de una
estructura esa información se encuentra en la tabla de símbolos, pero si
se trata de un arreglo, es necesario emitir el código para calcularlo.

### Aritmética

Las operaciones aritméticas se completan en registros. Las plantillas
*triviales* de generación a emplear son

1.  Para operadores TAC binarios como

            x := y op z

    el código trivial a generar es

            calcular y en $i
            calcular z en $j
            op $k,$i,$j
            almacenar $k en x                   

2.  Para operadores TAC unarios como

            x := op y

    el código trivial a generar es

            calcular y en $i
            op $j, $i
            almacenar $j en x

Puede aplicar todas las mejoras de código estudiadas en clase, en
particular para la asignación eficiente de registros, reducción
de fuerza aritmética y aplicación idiomática de la plataforma.

### Saltos incondicionales

La traducción del salto incondicional TAC hacia la etiqueta *L*

        goto L

se traduce directamente como

        j L

Note que la distancia entre la instrucción actual y *L* debe ser
a lo sumo 2^26 bytes por restricciones de la plataforma.

### Saltos condicionales

La traducción del salto condicional TAC hacia la etiqueta *L*

        if x BOP y goto L

se traduce directamente como

        cargar x en $i
        cargar y en $j
        JMP  $i, $j, L

donde `XY` se toma de la tabla

        BOP     JMP
        ===     ===
        <       blt
        <=      ble
        ==      beq
        !=      bne
        >=      bge
        >       bgt

Note que la distancia entre la instrucción actual y *L* debe ser
a lo sumo 2^15 bytes por restricciones de la plataforma.

## Llamadas a Funciones y Procedimientos

Como es habitual en procesadores RISC, el MIPS R2000 prefiere pasar hasta
cuatro argumentos utilizando registros y el resto a través de la pila. Sin
embargo, y con propósitos académicos, utilizaremos una convención de pasaje
de parámetros mucho más simple. Está convención es de uso **obligatorio**
en su generador de código con el propósito de evaluar su correcta comprensión
del tema.

*   Todos los parámetros serán pasados a través de la pila.

*   Los parámetros son pasados de *derecha* a *izquierda*, empilando
    el valor concreto del parámetro. Usted puede cambiar esta convención
    si es conveniente para su lenguaje -- lo importante es que haya
    un orden particular de empilado de parámetros.

### Secuencia de Llamada a Procedimiento

La traducción de la instrucción TAC de pasaje de parámetros

        param x

se traduce directamente como 

        calcular x en $i
        sw $i,  -4($sp)
        la $sp, -4($sp)

Note que el cálculo de `x` puede ser para el valor o la dirección,
dependiendo del tipo de pasaje de parámetros.

Una vez empilados todos los parámetros, la traducción de la instrucción
TAC de llamada al procedimiento

        call p, n

se traduce directamente como

        jal p
        la $sp, k($sp)

donde *k* es el espacio ocupado por todos los parámetros actuales.

#### Prólogo

Al ingresar a un procedimiento es necesario mantener el registro de
activación. Como el MIPS no provee instrucciones especiales para ajustar
la pila, la dirección de retorno, ni el apuntador al registro de
activación, es necesario hacerlo manualmente.

En este sentido puede ser conveniente contar con una instrucción TAC

        prologue f

donde *k* es un apuntador a la tabla de símbolos, de manera que se pueda
tener acceso a la información relevante necesaria. Esta instrucción se
traduce directamente como

        sw $fp, -4($sp)     # (-->) Salvar Frame Pointer previo
        sw $ra, -8($sp)     # (-->) Salvar dirección de retorno
        la $fp,  0($sp)     # (<--) Establecer Frame Pointer
        la $sp, -n($sp)     # (<--) Desplazar pila para espacio local

donde $n$ es la cantidad de espacio necesaria para almacenamiento local
del procedimiento $f$ incluyendo variables locales, variables temporales
y registros a salvar. Si fuera necesario salvar registros, esto debe
hacerse inmediatamente.

Los contenidos de la pila serían

              +---------------------+
              | Registro previo     |<-+
              | Parámetro derecho   |  |
              | ...                 |  |
              | Parámetro 2         |  |
        $fp ->| Parámetro izquierdo |  |
              | $fp del llamador  --+--+
              | $ra del llamador   |
              | Primer local       |
              | ...                |
              | Ultimo local       |
        $sp ->+--------------------+

Note que este estilo de secuencia de llamada imita el estilo de GCC en
lugar de utilizar el estilo definido en el ABI MIPS que es más eficiente
pero un poco más complicado de implantar.

#### Acceso a los parámetros actuales

El acceso a los parámetros se logra con el método indirecto provisto por
el procesador -- se sugiere utilizar desplazamientos en relación a `$fp`
porque eso *garantiza* que tanto parámetros como objetos locales siempre
están a desplazamientos conocidos a tiempo de compilación.

Según la organización que hemos establecido para la pila, el *primer*
parámetro actual (el "más a la izquierda") siempre estará en
`0($sp)` y el resto de los parámetros tendrán *offset* **negativo**.

#### Epílogo

Al salir de un procedimiento es necesario desmantelar el registro de
activación. Como el MIPS no provee instrucciones especiales para ajustar
la pila, la dirección de retorno, ni el apuntador al registro de
activación, es necesario hacerlo manualmente.

En este sentido puede ser conveniente contar con una instrucción TAC

        epilogue f

donde *k* es un apuntador a la tabla de símbolos, de manera que se pueda
tener acceso a la información relevante necesaria. Esta instrucción se
traduce directamente con la restauración de los registros que hayan
sido salvados.

El resto del código a generar depende si se trata de un procedimiento
o de una función.

Si se trata de un procedimiento que no retorna valor, terminaría con la
instrucción TAC

        return

que se traduce directamente como

        la $sp,  0($fp)     # (<--) Recuperar espacio local en pila
        lw $ra, -8($sp)     # (<--) Restablecer dirección de retorno
        lw $fp, -4($sp)     # (<--) Restaurar Frame Pointer previo 
        jr $ra              # Regresar al llamador

Si se trata de una función que debe retornar *un* valor *escalar*
(valor concreto o referencia) seguiremos la convención de utilizar el
registro `$v0` para retornarlo.

        return x

que se traduce directamente como

        cargar x en $v0     # Calcular valor de retorno en $v0
        la $sp,  0($fp)     # (<--) Recuperar espacio local en pila
        lw $ra, -8($sp)     # (<--) Restablecer dirección de retorno
        lw $fp, -4($sp)     # (<--) Restaurar Frame Pointer previo
        jr $ra              # Regresar al llamador
    
#### Acceso al valor de retorno

Las llamadas a funciones deben hacer uso de la instrucción TAC

        retrieve x

la cual se traduce directamente como

        cargar $v0 en x

## Operaciones ambientales

Le resultará necesario implantar algunas funciones y procedimientos para
que sus programas puedan completar entrada y salida de los diferentes
tipos de datos soportados por el sistema.

### Escribir un entero

Supone que se invoca con un número entero *n* como único parámetro, que
según nuestra convención está en `0($sp)`. Como las llamadas al sistema de
emulación SPIM *requieren* que los argumentos estén en `$a0`, debemos
escribir

        showint:
            li $v0, 1
            lw $a0, 0($sp)
            syscall
            jr $ra

### Escribir un punto flotante de precisión sencilla

Supone que se invoca con un número punto flotante de precisión sencilla
(32 bits) *f* como único parámetro, que según nuestra convención está
en `0($sp)` pero como un *patrón de bits*. Esto es, en la secuencia de
llamada ocurrió algo similar a

        mfc1 $t0, $f2        # (<--) Transferir 32 bits del FPU
        sw   $t0, -4($sp)    # (-->) Empilar los 32 bits
        la   $sp, -4($sp)    # (<--) Ajustar pila
        jal  showfloat
        la   $sp, 4($sp)     # (<--) Recuperar espacio en pila

Como la llamada al sistema de emulación SPIM para imprimir punto
flotante *requiere* que el argumento esté en `$f12`, debemos escribir
una rutina de envoltorio

        showfloat:
            lw    $t0, 0($sp)     # (<--) Tomar 32 bits de la pila
            mtc1  $t0, $f12       # (-->) Transferirlos al FPU
            li    $v0,  2         # Print single-precision float
            syscall
            jr $ra

Note que `showfloat` puede ahorrarse el establecimiento de la cadena
dinámica y almacenar la dirección de retorno en la pila pues no hace
llamados a rutinas auxiliares. No obstante, debería salvar y restaurar
`$t0`.

### Escribir un punto flotante de precisión doble

Supone que se invoca con un número punto flotante de precisión doble
(64 bits) *f* como único parámetro. Estableceremos la convención de que
la palabra de 32 bits del registro *impar* de la pareja se empila
antes que la palabra de 32 bits del registro *par* de la pareja, de modo
que según nuestra convención quedarán `4($sp)` y `0($sp)` respectivamente,
en ambos casos como *patrón de bits*. Esto es, en la secuencia de
llamada ocurrió algo similar a

        mfc1 $t1, $f5        # (<--) Transferir 32 bits del impar FPU
        mfc1 $t0, $f4        # (<--) Transferir 32 bits del par FPU
        sw   $t1, -4($sp)    # (-->) Empilar los 32 bits del impar
        sw   $t0, -8($sp)    # (-->) Empilar los 32 bits del par
        la   $sp, -8($sp)    # (<--) Ajustar pila
        jal  showdouble
        la   $sp, 8($sp)     # (<--) Recuperar espacio en pila

Como la llamada al sistema de emulación SPIM para imprimir punto
flotante *requiere* que el argumento esté en la *pareja* `$f12`,
debemos escribir una rutina de envoltorio

        showdouble:
            lw    $t0, 0($sp)     # (<--) Tomar 32 bits de la pila (par)
            lw    $t1, 4($sp)     # (<--) Tomar 32 bits de la pila (impar)
            mtc1  $t0, $f12       # (-->) Transferirlos al FPU
            mtc1  $t1, $f13       # (-->) Transferirlos al FPU
            li    $v0,  3         # Print double-precision float
            syscall
            jr $ra

Note que `showdouble` puede ahorrarse el establecimiento de la cadena
dinámica y almacenar la dirección de retorno en la pila pues no hace
llamados a rutinas auxiliares. No obstante, debería salvar y restaurar
`$t0` y `$t1`.

### Escribir una cadena

Supone que se invoca con la dirección de la cadena *s* como único parámetro,
que según nuestra convención está en `0($sp)`. Como las llamadas al sistema de
emulación SPIM *requieren* que los argumentos estén en `$a0`, debemos
escribir

        showstr:
           li $v0, 4
           lw $a0, 0($sp)
           syscall
           jr $ra

### SPIM syscalls

    Servicio              Código      Argumentos   Resultado
    ===================   ======      ==========   =========
    Imprimir Entero          1           $a0
    Imprimir Flotante        2           $f12
    Imprimir Doble           3           $f12
    Imprimir String          4           $a0

    Leer     Entero          5                       $v0
    Leer     Flotante        6                       $f0
    Leer     Entero          7                       $f0
    Leer     String          8         $a0, $a1

    Ampliar HEAP             9           $a0         $v0
    Salir                   10

