#pragma once

#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/source.hpp"

namespace dl {

// ABC for the parser, which accepts tokens as input and outputs operations.
struct Parser {
    // Feed a token to the parser.
    virtual ErrPtr feed(Token&& token, const Source& src) = 0;

    // Read an Operation from the parser.
    // Returns `OpID::WAITING` if the parser is waiting for more tokens.
    virtual Op next() = 0;
};

}
