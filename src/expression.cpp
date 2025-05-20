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

std::string Expression::debug_function() {
    switch (type) {
        case BOOL: return "dbg_print_bool";
        case INT: return "dbg_print_int";
        default: return "";
    }
}
