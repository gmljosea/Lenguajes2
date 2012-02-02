%option yylineno
%option noyywrap bison-locations
%option outfile="lexer.cc"
%{
#include <iostream>
#include "parser.hh"

int yycolumn = 1;
int lastline = 1;

#define YY_USER_ACTION update_yylloc(yylloc);

void update_yylloc(YYLTYPE *yylloc) {
  yylloc->first_line = yylloc->last_line = yylineno;
  if (lastline < yylineno) {
     lastline = yylineno;
     yycolumn = 1;
  }
  yylloc->first_column = yycolumn;
  yylloc->last_column = yycolumn+yyleng-1;
  yycolumn += yyleng;
  std::cout << yylloc->first_line << ":" << yylloc->first_column << "," << yylloc->last_line << ":" << yylloc->last_column << std::endl;
}
%}

ID      [_a-zA-Z][_a-zA-Z0-9]*
NumDec  [0-9]+
NumBin  b:[0-1]+
NumHex  x:[0-9A-Fa-f]+
NumFlot [0-9]+.[0-9]+
ComentL -\..*
ComentB --\.(.|\n)*\.--
STRING  \".*\"

%%

if { std::cout << "token if" << std::endl; return TK_IF;}

else { std::cout << "token else"  << std::endl; return TK_ELSE;}

for { std::cout << "token for"  << std::endl; return TK_FOR;}

in  {std::cout << "token in" << std::endl; return TK_IN;}

step { std::cout << "token step" << std::endl; return TK_STEP;}

while { std::cout << "while" << std::endl; return TK_WHILE;}

int { std::cout << "token int" << std::endl; return TK_INT;}

char { std::cout << "token char" << std::endl; return TK_CHAR;}

bool { std::cout << "token bool" << std::endl; return TK_BOOL;}

float { std::cout << "token float" << std::endl; return TK_FLOAT;}

array { std::cout << "token array" << std::endl; return TK_ARRAY;}

string { std::cout << "token string"; return TK_STRING;}

box { std::cout << "token box"; return TK_BOX;}

void { std::cout << "token void";return TK_VOID;}

variant: { std::cout << "token variant";return TK_VARIANT;}

true { std::cout << "token true";return TK_TRUE;}

false {std::cout  << "token false"; return TK_FALSE;}

return {std::cout << "token return"; return TK_RETURN;}

break {std::cout << "token break"; return TK_BREAK;}

next  {std::cout << "token next"; return TK_NEXT;}

\.\. {std::cout << "token dperiod"; return TK_DPERIOD;}
\+  {std::cout << "token plus";return TK_PLUS;}
-   {std::cout << "token minus";return TK_MINUS;}
\*  {std::cout << "token times";return TK_TIMES;}
\/  {std::cout << "token div"; return TK_TIMES;}
%   {std::cout << "token mod"; return TK_DIV;}
=   {std::cout << "token EQU"; return TK_EQU;}
\<  {std::cout << "token LT"; return TK_LT;}
\>  {std::cout << "token GT"; return TK_GT;}
\>= {std::cout << "token GTE"; return TK_GTE;}
\<= {std::cout << "token LTE"; return TK_LTE;}
and {std::cout << "token and"; return TK_AND;}
or  {std::cout << "token or"; return TK_OR;}
not {std::cout << "token not"; return TK_NOT;}

\{ { std::cout << "token Lbrace"; return TK_LBRACE; }
\} { std::cout << "token Rbrace"; return TK_RBRACE; }
\[ { std::cout << "token Lbraket"; return TK_LBRACKET;}
\] { std::cout << "token Rbraket"; return TK_RBRACKET;}
\( { std::cout << "token Lparent"; return TK_LPARENT;}
\) { std::cout << "token Rparent"; return TK_RPARENT;}
;  { std::cout << "token SCOLON"; return TK_SCOLON;}
,  { std::cout << "token COMMA"; return TK_COMMA;}

\$\$ {std::cout << "token ddollar"; return TK_DOLLAR;}
\$   {std::cout << "token dollar"; return TK_DDOLLAR;}

{ID}      {std::cout << "token id";return TK_ID;}
{STRING}  {std::cout << "token conststring"; return TK_CONSTSTRING;}
{ComentL} 
{ComentB}
{NumDec}  {std::cout << "Num base decimal"; return TK_CONSTINT;}
{NumBin}  {std::cout << "Num base Binaria"; return TK_CONSTINT;}
{NumHex}  {std::cout << "Num base Hexadecimal"; TK_CONSTINT;}
{NumFlot} {std::cout << "Num flotante";return TK_CONSTFLOAT;}
[[:blank:]]+
\n { yycolumn--;}


%%
