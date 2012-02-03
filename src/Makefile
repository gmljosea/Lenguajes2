.PHONY = clean

SRC = lexer.cc parser.cc $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)
CXXFLAGS = -MMD -std=c++0x

devanix: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

lexer.cc: lexer.l parser.hh
	flex lexer.l

parser.hh parser.cc: parser.y
	bison parser.y

-include $(DEP)

clean:
	rm $(OBJ) $(DEP) devanix parser.hh lexer.cc parser.cc