#pragma once

#include "dl/lex/cursor.hpp"
#include "dl/lex/token.hpp"
#include "dl/res.hpp"

namespace dl {

// ABC for lexer.
struct Lexer {
    // Read the token after `cursor`, putting any related data into `data`.
    virtual Res<Token> next(Cursor& cursor) = 0;
};

}
