#pragma once

#include <ostream>

namespace dl {

enum class SeqKind {
    LIST,
    TUPLE,
    SET,
    MAP
};

std::ostream& operator<<(std::ostream& os, SeqKind x) {
    using enum SeqKind;

    switch(x) {
    case LIST:
        return os << "LIST";
    case TUPLE:
        return os << "TUPLE";
    case SET:
        return os << "SET";
    case MAP:
        return os << "MAP";
    }
}

}