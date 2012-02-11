#include <iostream>

#include "program.hh"

void Program::error(std::string error, int line, int col) {
  std::cout << line << ":" << col << ": error: " << error << std::endl;
}
