#pragma once

#include <cstdint>

#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/parse/op.hpp"

namespace dl {

// ABC for the parser, which accepts tokens as input and outputs operations.
struct Parser {
    // Feed a token to the parser.
    virtual ErrPtr feed(Token& token, std::uint64_t src_id) = 0;

    // Read an Operation from the parser.
    // Returns `OpID::WAITING` if the parser is waiting for more tokens.
    virtual Op next() = 0;
};

}
