.PHONY = clean

SRC = $(wildcard *.cc) parser.cc lexer.cc
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)
CXXFLAGS = -MMD -std=c++0x

devanix: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

lexer.cc: lexer.l
	flex -o lexer.cc lexer.l

parser.hh parser.cc: parser.y
	bison --defines=parser.hh -o parser.cc parser.y

-include $(DEP)

clean:
	rm $(OBJ) $(DEP) devanix parser.hh lexer.cc parser.cc