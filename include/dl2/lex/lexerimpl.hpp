#pragma once

#include "dl/file.hpp"
#include "dl/lex/lex.hpp"
#include "dl/lex/lexer.hpp"
#include "dl/lex/tokendata.hpp"
#include "dl/lex/tokenid.hpp"

namespace dl {

struct LexerImpl: Lexer {
    TokenID next(Cursor& cursor) override {
        return lex(cursor);
    }
};

}
