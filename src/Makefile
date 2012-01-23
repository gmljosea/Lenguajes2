all: lexer.o parser.o
	g++ lexer.o parser.o -o devanix

lexer.o: lexer.cc parser.cc

lexer.cc: lexer.l
	flex -o lexer.cc lexer.l

parser.o: parser.cc

parser.cc: parser.y
	bison --defines=parser.h -o parser.cc parser.y


clean:
	rm lexer.cc parser.cc parser.h *.o devanix
