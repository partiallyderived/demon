#pragma once

namespace dl {

enum class TokenKind {
    BINARY,
    CONSTRUCT_FIRST,
    CONSTRUCT_FIRST_OR_BINARY,
    CONSTRUCT_LAST,
    CONSTRUCT_LAST_OR_BINARY,
    CONSTRUCT_MIDDLE,
    END_OF_FILE,
    ERR,
    HASH,
    LEFT,
    MULTIARY,
    MULTIARY_POSTFIX,
    MULTIARY_VALUE,
    NEWLINE,
    NULLARY,
    RETURN,
    RIGHT,
    SPACE,
    UNARY,
    VALUE
};

}
