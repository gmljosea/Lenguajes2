%option noyywrap c++
%{
using namespace std;
%}


%%

if { cout << "token if";}

else { cout << "token else";}

for { cout << "token for";}

in  {cout << "token in";}

step { cout << "token step";}

while { cout << "while";}

int { cout << "token int";}

char { cout << "token char";}

bool { cout << "token bool";}

float { cout << "token float";}

array { cout << "token array";}

string { cout << "token string";}

box { cout << "token box";}

void { cout << "token void";}

variant { cout << "token variant";}

true { cout << "token true";}

false {cout  << "token false";}

%%

int main() {
    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex() != 0);
    return 0;
}
