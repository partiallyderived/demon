#pragma once

#include <ostream>

namespace dl {

enum class UnaryKind {
    ADDR,
    ADDR_TYPE,
    EXPANSION,
    LAMBDA,
    LAMBDA_VAR,
    NOT,
    RAISE,
    RETURN,
    SYMBOL
};

std::ostream& operator<<(std::ostream& os, UnaryKind x) {
    using enum UnaryKind;

    switch(x) {
    case ADDR:
        return os << "ADDR";
    case ADDR_TYPE:
        return os << "ADDR_TYPE";
    case EXPANSION:
        return os << "EXPANSION";
    case LAMBDA:
        return os << "LAMBDA";
    case LAMBDA_VAR:
        return os << "LAMBDA_VAR";
    case NOT:
        return os << "NOT";
    case RAISE:
        return os << "RAISE";
    case RETURN:
        return os << "RETURN";
    case SYMBOL:
        return os << "SYMBOL";
    }
}

}
