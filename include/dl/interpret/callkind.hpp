#pragma once

#include <ostream>

namespace dl {

enum class CallKind {
    CALL,
    CACHE,
    MATCH
};

std::ostream& operator<<(std::ostream& os, CallKind x) {
    using enum CallKind;

    switch(x) {
    case CALL:
        return os << "CALL";
    case CACHE:
        return os << "CACHE";
    case MATCH:
        return os << "MATCH";
    }
}

}
