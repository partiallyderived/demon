#pragma once

#include "dl/lex/cursor.hpp"
#include "dl/lex/tokenres.hpp"

namespace dl {

// ABC for lexer.
struct Lexer {
    // Read the token after `cursor`, putting any related data into `data`.
    virtual TokenRes next(Cursor& cursor) = 0;
};

}
