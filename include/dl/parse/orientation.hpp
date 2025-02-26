#pragma once

#include <ostream>

namespace dl {

// Represents the "orientation" of the parser, which is used to determines how
// different types of tokens evolve the parser's state, most notable the
// orientation itself.

// START: At the start of any instruction.
// STMT: At the start of a statement. Excludes constructs like "if".
// BEFORE: Before a value, where we may expect one.
// AFTER: After a value, where a binary operator is allowed.
// OPTIONAL: Where a value is optional.
// AFTER_NOT: Special case: "not in" is parsed as a single operator, so while
//            "not" is usually a unary operator, it may be followed by "in" to
//            be parsed as the "not in" operator.
// AFTER_STAR: Special case: after encountering * in parentheses where a value
//             is expected. Special orientation is needed because * may
//             present as either a unary operator or a value in that case.
// END: End of a statement, where a newline is expected.
enum class Orientation {
    START,
    STMT,
    BEFORE,
    AFTER,
    OPTIONAL,
    AFTER_NOT,
    AFTER_STAR,
    END
};

std::ostream& operator<<(std::ostream& os, Orientation orientation) {
    using enum Orientation;
    switch(orientation) {
    case START:
        return os << "START";
    case STMT:
        return os << "STMT";
    case BEFORE:
        return os << "BEFORE";
    case AFTER:
        return os << "AFTER";
    case OPTIONAL:
        return os << "OPTIONAL";
    case AFTER_NOT:
        return os << "AFTER_NOT";
    case AFTER_STAR:
        return os << "AFTER_STAR";
    case END:
        return os << "END";
    }
}

}
