#pragma once

namespace dl {

enum class TokenKind {
    BINARY,
    CONSTRUCT_FIRST,
    CONSTRUCT_FIRST_OR_BINARY,
    CONSTRUCT_LAST,
    CONSTRUCT_LAST_OR_BINARY,
    CONSTRUCT_MIDDLE,
    DUAL_AFFIX,
    LEFT,
    MULTIARY,
    NULLARY,
    OPTIONAL_STMT,
    RIGHT,
    SPECIAL,
    UNARY,
    VALUE
};

}
