#include "../include/semantics.h"

void semantic_error(int line, std::string text) {
    std::cerr << "Line " << line << ": " << text << std::endl;
    exit(1);
}
