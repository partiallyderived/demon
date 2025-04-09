#pragma once

#include "dl/lex/token.hpp"
#include "dl/parse/op.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"

namespace dl {

// ABC for the parser, which accepts tokens as input and outputs operations.
struct Parser {
    // Feed a token to the parser.
    virtual void feed(Token&& token, Span src) = 0;

    // Read an Operation from the parser.
    // Returns Op with ID `OpID::WAITING` if the parser is waiting for more
    // tokens.
    virtual Res<Op> next() = 0;
};

}
