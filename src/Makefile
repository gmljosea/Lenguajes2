all: lex.yy.o
	g++ lex.yy.o -o lexer -lfl

lex.yy.o: lex.yy.cc
	g++ -c -o lex.yy.o lex.yy.cc

lex.yy.cc: devanixLexer.l
	flex++ devanixLexer.l

clean:
	rm lex.yy.* parser
