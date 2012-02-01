devanix: lexer.o parser.o statement.o expression.o symbol.o
	g++ lexer.o parser.o statement.o expression.o symbol.o -o devanix

expression.o: expression.cc

statement.o: statement.cc

symbol.o: symbol.cc
	g++ -std=c++0x -c symbol.cc
lexer.o: lexer.cc parser.cc

lexer.cc: lexer.l
	flex -o lexer.cc lexer.l

parser.o: parser.cc statement.hh expression.hh

parser.cc: parser.y
	bison --defines=parser.hh -o parser.cc parser.y

clean:
	rm lexer.cc parser.cc parser.hh *.o devanix
