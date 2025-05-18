#pragma once

#include <string>

enum Type {
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

    private:
        Type type;
        std::string code;
};
