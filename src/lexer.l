%option yylineno noyywrap
%option outfile="lexer.cc"

%{
#include <iostream>
#include <stdexcept>
#include <string>

#include "parser.hh"

#define YY_USER_ACTION lexer::set_yylloc();

extern YYLTYPE yylloc;
extern Program program;

namespace lexer {
  int yycolumn = 1;
  int lastline = 1;

  /**
   * LLena los campos de yylloc con la ubicación actual del token que se está
   * procesando. Utiliza las variables globales yycolumn, yylineno y lastline.
   * Basado en el Cap.8 del libro Flex & Bison
   */
  void set_yylloc() {
    yylloc.first_line = yylloc.last_line = yylineno;
    if (lastline < yylineno) {
      lastline = yylineno;
      yycolumn = 1;
    }
    yylloc.first_column = yycolumn;
    yylloc.last_column = yycolumn+yyleng-1;
    yycolumn += yyleng;
  }

  void trace(std::string rule, std::string yytext, YYLTYPE yylloc) {
   /*std::cerr << "Token " << rule << " (" << yytext << ") en "
              << yylloc.first_line << ":" << yylloc.first_column << std::endl;*/
  }

  /**
   * Substituye las cadenas de caracteres del token conststring que representan
   * caracteres escapados por su caracter correspondiente.
   */
  void treatstring(){
    size_t pos;

    if((pos=yylval.str->find("\\n"))!=std::string::npos) 
       yylval.str->replace(pos ,2,"\n"); 

    if((pos=yylval.str->find("\\t"))!=std::string::npos) 
       yylval.str->replace(pos,2,"\t");

    if((pos=yylval.str->find("\\\""))!=std::string::npos) 
       yylval.str->replace(pos,2,"\"");

    if((pos=yylval.str->find("\\\\"))!=std::string::npos) 
       yylval.str->replace(pos,2,"\\");

    yylval.str->replace(0,1,"");

    int tam= yylval.str->size()-1;
    yylval.str->erase(tam,1);
  }
}

%}

ID      [_a-zA-Z][_a-zA-Z0-9]*
DecNum  [1-9][0-9]*
BinNum  "b:"[0-1]+
HexNum  "x:"[0-9A-Fa-f]+
Float   [0-9]+\.[0-9]+([Ee][\-+]?[0-9]+)?
LineComment  "-.".*
 /* BlockComment "--."(.|\n)*".--"*/
Char "'"(.|\\n|\\t|\\'|\\\\)"'"
String \"([^\"\\\n]|\\n|\\t|\\\"|\\\\)*\"


%x BlockComm

%%

 /* Palabrasd reservadas */
if      { lexer::trace("if", yytext, yylloc); return TK_IF;}

else    { lexer::trace("else", yytext, yylloc); return TK_ELSE;}

for     { lexer::trace("for", yytext, yylloc); return TK_FOR;}

in      { lexer::trace("in", yytext, yylloc); return TK_IN;}

step    { lexer::trace("step", yytext, yylloc); return TK_STEP;}

while   { lexer::trace("while", yytext, yylloc); return TK_WHILE;}

int     { lexer::trace("int", yytext, yylloc); return TK_INT;}

char    { lexer::trace("char", yytext, yylloc); return TK_CHAR;}

bool    { lexer::trace("bool", yytext, yylloc); return TK_BOOL;}

float   { lexer::trace("float", yytext, yylloc); return TK_FLOAT;}

array   { lexer::trace("array", yytext, yylloc); return TK_ARRAY;}

string  { lexer::trace("string", yytext, yylloc); return TK_STRING;}

box     { lexer::trace("box", yytext, yylloc); return TK_BOX;}

void    { lexer::trace("void", yytext, yylloc); return TK_VOID;}
 /* Le quité los : a variant para convertirlos luego en un token. */
variant { lexer::trace("variant", yytext, yylloc); return TK_VARIANT;}

true    { lexer::trace("true", yytext, yylloc); return TK_TRUE;}

false   { lexer::trace("false", yytext, yylloc); return TK_FALSE;}

return  { lexer::trace("return", yytext, yylloc); return TK_RETURN;}

break   { lexer::trace("break", yytext, yylloc); return TK_BREAK;}

next    { lexer::trace("next", yytext, yylloc); return TK_NEXT;}

write   { lexer::trace("write", yytext, yylloc); return TK_WRITE;}

read    { lexer::trace("read", yytext, yylloc); return TK_READ; }

writeln { lexer::trace("writeln", yytext, yylloc); return TK_WRITELN; }

 /* Operadores */
\+  { lexer::trace("+", yytext, yylloc); return TK_PLUS;}
-   { lexer::trace("-", yytext, yylloc); return TK_MINUS;}
\*  { lexer::trace("*", yytext, yylloc); return TK_TIMES;}
\/  { lexer::trace("/", yytext, yylloc); return TK_DIV;}
%   { lexer::trace("%", yytext, yylloc); return TK_MOD;}
=   { lexer::trace("=", yytext, yylloc); return TK_EQU;}
!=  { lexer::trace("!=", yytext, yylloc); return TK_NEQ;}
\<  { lexer::trace("<", yytext, yylloc); return TK_LT;}
\>  { lexer::trace(">", yytext, yylloc); return TK_GT;}
\>= { lexer::trace(">=", yytext, yylloc); return TK_GTE;}
\<= { lexer::trace("<=", yytext, yylloc); return TK_LTE;}
and { lexer::trace("and", yytext, yylloc); return TK_AND;}
or  { lexer::trace("or", yytext, yylloc); return TK_OR;}
not { lexer::trace("not", yytext, yylloc); return TK_NOT;}

 /* Delimitadores de bloques, instrucciones y listas de cosas */
\{ { lexer::trace("{", yytext, yylloc); return TK_LBRACE; }
\} { lexer::trace("}", yytext, yylloc); return TK_RBRACE; }
\[ { lexer::trace("[", yytext, yylloc); return TK_LBRACKET;}
\] { lexer::trace("]", yytext, yylloc); return TK_RBRACKET;}
\( { lexer::trace("(", yytext, yylloc); return TK_LPARENT;}
\) { lexer::trace(")", yytext, yylloc); return TK_RPARENT;}
;  { lexer::trace(";", yytext, yylloc); return TK_SCOLON;}
,  { lexer::trace(",", yytext, yylloc); return TK_COMMA;}

 /* Símbolos especiales misceláneos */
\.\. { lexer::trace("..", yytext, yylloc); return TK_DPERIOD;}
\$\$ { lexer::trace("$$", yytext, yylloc); return TK_DOLLAR;}
\$   { lexer::trace("$", yytext, yylloc); return TK_DDOLLAR;}
:    { lexer::trace(":", yytext, yylloc); return TK_COLON; }
\.   { lexer::trace(".",yytext,yylloc); return TK_DOT;}

 /* Símbolos con valor semántico: strings, identificadores y números*/
{ID}      { lexer::trace("identificador", yytext, yylloc);
            yylval.str = new std::string(yytext); return TK_ID;}
{String}  { lexer::trace("string", yytext, yylloc);
            yylval.str = new std::string(yytext);
            lexer::treatstring(); return TK_CONSTSTRING;}
{Char}    { lexer::trace("char", yytext, yylloc);
            yylval.str = new std::string(yytext); return TK_CONSTCHAR;}

{DecNum}  { lexer::trace("entero base decimal", yytext, yylloc);
            try {
                yylval.ival = std::stoi(yytext, 0, 10);
            } catch (std::out_of_range oor) {
                program.error("constante '"+std::string(yytext)+"' demasiado grande.",
                              yylloc.first_line, yylloc.first_column);
                yylval.ival = 0;
            }
            return TK_CONSTINT;
          }

{BinNum} { lexer::trace("entero base binaria", yytext, yylloc);
             try {
                yylval.ival = std::stoi(yytext+2, 0, 2);
            } catch (std::out_of_range oor) {
                program.error("constante '"+std::string(yytext+2)+"' demasiado grande.",
                              yylloc.first_line, yylloc.first_column);
                yylval.ival = 0;
            }
            return TK_CONSTINT;
          }

{HexNum}  { lexer::trace("entero base hexadecimal", yytext, yylloc);
            try {
                yylval.ival = std::stoi(yytext+2, 0, 16);
            } catch (std::out_of_range oor) {
                program.error("constante '"+std::string(yytext)+"' demasiado grande.",
                              yylloc.first_line, yylloc.first_column);
                yylval.ival = 0;
            }
            return TK_CONSTINT;
          }
{Float}   { lexer::trace("punto flotante", yytext, yylloc);
             try {
                yylval.fval = std::stof(yytext);
            } catch (std::out_of_range oor) {
                program.error("constante '"+std::string(yytext)+"' no se puede representar.",
                              yylloc.first_line, yylloc.first_column);
                yylval.fval = 0.0;
            }
            return TK_CONSTFLOAT; }

 /* Comerse los comentarios, tomado del manual de Flex */
{LineComment} ;

 /* Esto lo hice así porque según el manual de Flex si se usa yylineno es más
    eficiente evitar que las reglas puedan matchear más de un salto de línea.
    Y convenientemente el manual tenía un ejemplo de como matchear comentarios
    multilínea. */
"--."      BEGIN(BlockComm);

<BlockComm>".--"      BEGIN(0);
<BlockComm>[^"."\n]+ ;
<BlockComm>"."[^-]   ;
<BlockComm>\n

[[:blank:]]+ ;
\n { lexer::yycolumn--;}
   /* Esta regla modifica yycolumn porque cuando se ejecuta, flex ya actualizó
      la variable yylineno, entonces la función set_yylloc() cuenta el salto de
      línea como el primer caracter de cada línea, dañando la ubicación de los
      otros tokens */

 /* Detectar caracteres raros */
. { program.errorCount++;
    program.error("caracter '"+std::string(yytext)+"' no reconocido.",
                  yylloc.first_line, yylloc.first_column); }

%%
