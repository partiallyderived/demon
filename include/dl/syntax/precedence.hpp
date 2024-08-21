#pragma once

namespace dl::syntax {

// Precedence for each operator, in order of increasing (higher) precedence.
enum class Precedence {
    LEFT_LEFT,
    RIGHT_RIGHT,
    STATEMENTS,
    FLOW,
    FOR_PRED,
    LABEL,
    LSET,
    RSET,
    SEQ,
    OR,
    AND,
    NOT,
    CMP,
    BOR,
    BXOR,
    BAND,
    SHIFT,
    ADD,
    MUL,
    PREFIX,
    CALL,
    SUFFIX,
    CONCAT,
    GET,
    UNARY_RIGHT
};

}
