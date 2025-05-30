#include "../include/exceptions.h"
#include <string>

SymbolNotFoundException::SymbolNotFoundException(std::string msg) : msg(msg) {}

const char* SymbolNotFoundException::what() const noexcept {
    return msg.c_str();
}

SymbolRedeclarationException::SymbolRedeclarationException(std::string msg) : msg(msg) {}

const char* SymbolRedeclarationException::what() const noexcept {
    return msg.c_str();
}
