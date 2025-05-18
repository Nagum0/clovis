#include "../include/expression.h"
#include <string>

Expression::Expression() {}

Expression::Expression(Type type) {
    this->type = type;
}

Expression::Expression(Type type, std::string code) {
    this->type = type;
    this->code = code;
}

Type Expression::get_type() const {
    return type;
}

void Expression::set_type(Type type) {
    this->type = type;
}

std::string Expression::get_code() const {
    return code;
}

void Expression::set_code(std::string code) {
    this->code = code;
}
