#pragma once

#include <ostream>

namespace dl {

enum class BinaryKind {
    AND,
    DECLARE,
    ENTRY,
    GET_ATTR,
    IN,
    OR,
    SET
};

std::ostream& operator<<(std::ostream& os, BinaryKind x) {
    using enum BinaryKind;

    switch(x) {
    case AND:
        return os << "AND";
    case DECLARE:
        return os << "DECLARE";
    case ENTRY:
        return os << "ENTRY";
    case GET_ATTR:
        return os << "GET_ATTR";
    case IN:
        return os << "IN";
    case OR:
        return os << "OR";
    case SET:
        return os << "SET";
    }
}

}
