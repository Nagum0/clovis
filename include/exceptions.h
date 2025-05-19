#pragma once

#include <exception>
#include <string>

class SymbolNotFoundException : public std::exception {
    public:
        SymbolNotFoundException(std::string msg);
        const char* what() const noexcept override;

    private:
        std::string msg;
};

class SymbolRedeclarationException : public std::exception {
    public:
        SymbolRedeclarationException(std::string msg);
        const char* what() const noexcept override;

    private:
        std::string msg;
};
