#pragma once

namespace dl::syntax {

// Precedence for each operator, in order of increasing (higher) precedence.
enum class Precedence {
    START,
    END,
    STATEMENTS,
    FLOW,
    FOR_PRED,
    BLOCK,
    LSET,
    RSET,
    SEP,
    LABEL,
    TERNARY,
    OR,
    AND,
    NOT,
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
