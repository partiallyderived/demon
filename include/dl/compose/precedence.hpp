#pragma once

namespace dl {

// Precedence for each operator, in order of increasing (higher) precedence.
enum class Precedence {
    START,
    END,
    STATEMENTS,
    FLOW,
    FOR_PRED,
    LSET,
    RSET,
    SEP,
    LABEL,
    OR,
    AND,
    CMP,
    IN,
    BOR,
    BXOR,
    BAND,
    SHIFT,
    ADD,
    MUL,
    LEXP,
    REXP,
    PREFIX,
    CALL,
    SUFFIX,
    CONCAT,
    GET,
    UNARY
};

}
