#pragma once

#include <string>

enum Type {
    BOOL = 1,       // BYTE
    INT = 4,        // DOUBLEWORD
};

class Expression {
    public:
        Expression();
        Expression(Type type);
        Expression(Type type, std::string code);

        Type get_type() const;

        void set_type(Type type);

        std::string get_code() const;

        void set_code(std::string code);

        std::string debug_function();

        std::string get_type_str();

        std::string get_type_register();

    private:
        Type type;
        std::string code;
};
