#pragma once

#include <ostream>

namespace dl {

// Contexts are used to to indicate that we are currently parsing an operation
// which has special conditions to indicate when we are finished
// parsing it.
enum class Context {
    BLOCK,
    CURVED,
    SQUARE,
    CONSTRUCT,
    CONSTRUCT_END
};

std::ostream& operator<<(std::ostream& os, Context ctx) {
    using enum Context;
    switch(ctx) {
    case BLOCK:
        return os << "BLOCK";
    case CURVED:
        return os << "CURVED";
    case SQUARE:
        return os << "SQUARE";
    case CONSTRUCT:
        return os << "CONSTRUCT";
    case CONSTRUCT_END:
        return os << "CONSTRUCT_END";
    }
}

}
