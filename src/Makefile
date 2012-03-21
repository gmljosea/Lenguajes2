.PHONY = clean format

SRC = lexer.cc parser.cc $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)
CXXFLAGS = -g -MMD -std=c++0x

devanix: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

lexer.cc: lexer.l parser.hh
	flex lexer.l

parser.hh parser.cc: parser.y
	bison parser.y

-include $(DEP)

clean:
	rm $(OBJ) $(DEP) devanix parser.hh lexer.cc parser.cc

format:
	astyle -A2 -s2 -f -H -k1 -n *.hh *.cc

check-syntax:
	g++ $(CXXFLAGS) $(LINK_OPS) -o nul -S $(CHK_SOURCES)
