#pragma once

#include <ostream>

namespace dl {

enum class NullaryKind {
    BREAK,
    CONTINUE,
    NONE,
    NULL_,
    THIS
};

std::ostream& operator<<(std::ostream& os, NullaryKind x) {
    using enum NullaryKind;

    switch(x) {
    case BREAK:
        return os << "BREAK";
    case CONTINUE:
        return os << "CONTINUE";
    case NONE:
        return os << "NONE";
    case NULL_:
        return os << "NULL";
    case THIS:
        return os << "THIS";
    }
}

}
