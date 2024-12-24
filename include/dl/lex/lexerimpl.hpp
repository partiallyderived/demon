#pragma once

#include "dl/file.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/lex/lex.hpp"
#include "dl/lex/lexer.hpp"
#include "dl/lex/token.hpp"
#include "dl/res.hpp"

namespace dl {

struct LexerImpl: Lexer {
    Res<Token> next(Cursor& cursor) override {
        return lex(cursor);
    }
};

}
