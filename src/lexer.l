%option noyywrap
%{
#include <iostream>
#include "parser.h"
%}

plus \+

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

variant { std::cout << "token variant";}

true { std::cout << "token true";}

false {std::cout  << "token false";}

\+ std::cout << "token plus";
- std::cout << "token minus";
\* std::cout << "token times";
\/ std::cout << "token div";
% std::cout << "token mod";

\{ std::cout << "token Lbrace";
\} std::cout << "token Rbrace";
\[ std::cout << "token Lbraket";
\] std::cout << "token Rbraket";
\( std::cout << "token Lparent";
\) std::cout << "token Rparent";



%%
