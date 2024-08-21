#pragma once

namespace dl {

// Represents the "orientation" of the parser, which is used to determines how
// different types of tokens evolve the parser's state, most notable the
// orientation itself.

// START: At the start of a statement
// BEFORE: "Before" a value, where a value is expected.
// AFTER: "After" a value, where a binary operator is allowed.
// AFTER_BINARY_OR_POSTFIX: After a token which is ambiguously either a binary
//     operator or a unary postfix operator.
// AFTER_UNARY_OR_VALUE: After a token which is ambiguously either a unary
//     operator or a value.
// OPTIONAL: Where a value is optional.
// END: End of a statement, where a newline is expected.
enum class Orientation {
    START,
    BEFORE,
    AFTER,
    AFTER_BINARY_OR_POSTFIX,
    AFTER_UNARY_OR_VALUE,
    OPTIONAL,
    END
};



}