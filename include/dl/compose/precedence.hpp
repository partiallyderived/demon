#pragma once

namespace dl {

// Precedence for each operator, in order of increasing (higher) precedence.
enum class Precedence {
    START,
    END,
    STATEMENTS,
    FLOW,
    LABEL,
    STATEMENT,
    LSET,
    RSET,
    SEP,
    TYPE_LABEL,
    BIND,
    OR,
    AND,
    CMP,
    IN,
    LOOP_VAR_SEP,
    BOR,
    BXOR,
    BAND,
    SHIFT,
    ADD,
    MUL,
    LPOW,
    RPOW,
    PREFIX,
    CONCAT,
    GET_CALL,
    SUFFIX,
    UNARY
};

}
