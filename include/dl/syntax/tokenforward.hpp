#pragma once

#include <cstddef>

#include <memory>

#include "dl/pos.hpp"

namespace dl::syntax {

struct Token;

struct TokenDeleter {
    inline void operator()(Token* x) const noexcept;
};

using TokenPtr = std::unique_ptr<Token, TokenDeleter>;

struct Located {
    TokenPtr token;
    Pos loc;

    Located(std::nullptr_t) noexcept: token(nullptr), loc() {}

    Located(Token* token, Pos loc) noexcept: token(token), loc(loc) {}

    Located(TokenPtr&& token, Pos loc) noexcept:
    token(std::move(token)), loc(loc) {}

    bool operator==(const Located& that) const noexcept;
};

}
