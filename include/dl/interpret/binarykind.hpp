#pragma once

#include <ostream>

namespace dl {

enum class BinaryKind {
    AND,
    ENTRY,
    IN,
    OR,
    SET
};

std::ostream& operator<<(std::ostream& os, BinaryKind x) {
    using enum BinaryKind;

    switch(x) {
    case AND:
        return os << "AND";
    case ENTRY:
        return os << "ENTRY";
    case IN:
        return os << "IN";
    case OR:
        return os << "OR";
    case SET:
        return os << "SET";
    }
}

}
