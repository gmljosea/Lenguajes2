%option noyywrap
%{
#include <iostream>
#include "parser.h"
int num_linea=0;
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

if { return TK_IF;}

else { std::cout << "token else";}

for { std::cout << "token for";}

in  {std::cout << "token in";}

step { std::cout << "token step";}

while { std::cout << "while";}

int { std::cout << "token int";}

char { std::cout << "token char";}

bool { std::cout << "token bool";}

float { std::cout << "token float";}

array { std::cout << "token array";}

string { std::cout << "token string";}

box { std::cout << "token box";}

void { std::cout << "token void";}

variant: { std::cout << "token variant";}

true { std::cout << "token true";}

false {std::cout  << "token false";}

return {std::cout << "token return";}

\+ std::cout << "token plus";
- std::cout << "token minus";
\* std::cout << "token times";
\/ std::cout << "token div";
% std::cout << "token mod";
= std::cout << "token EQU";
\< std::cout << "token LT";
\> std::cout << "token GT";
\>= std::cout << "token GTE";
\<= std::cout << "token LTE";
and  std::cout << "token and";
or  std::cout << "token or";
not  std::cout << "token not";

\{ std::cout << "token Lbrace";
\} std::cout << "token Rbrace";
\[ std::cout << "token Lbraket";
\] std::cout << "token Rbraket";
\( std::cout << "token Lparent";
\) std::cout << "token Rparent";

\$\$ std::cout << "token ddollar";
\$ std::cout << "token dollar";

{ID} std::cout << "token id";
{STRING} std::cout << "token conststring";
{ComentL} 
{ComentB}
{NumDec} std::cout << "Num base decimal";
{NumBin} std::cout << "Num base Binaria";
{NumHex} std::cout << "Num base Hexadecimal";
{NumFlot} std::cout << "Num flotante";


%%
