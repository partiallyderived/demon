#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>
#include <variant>

#include "dl/data.hpp"
#include "dl/lex/tokenid.hpp"

namespace dl {

struct Token {
    TokenID id;

    // Data associated with the token, if any.
    Data data;

    Token(const Token& that) = delete;

    Token(Token&& that) noexcept: id(that.id), data(std::move(that.data)) {}

    // For most tokens, the content is inferrable by the ID. For +, for example,
    // there is no need to store the content as "+".
    Token(TokenID id) noexcept: id(id), data() {}

    Token(TokenID id, Data&& data) noexcept:
    id(id), data(std::move(data)) {}

    bool operator==(const Token& that) const noexcept {
        if (id == TokenID::ERROR && that.id == TokenID::ERROR)
            return *std::get<ErrPtr>(data) == *std::get<ErrPtr>(that.data);
        return id == that.id && data == that.data;
    }

    Token copy() const noexcept {
        return Token(id, copy_data(data));
    }
};

std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "Token(" << t.id;
    if (!std::holds_alternative<std::monostate>(t.data))
        os << ", " << t.data;
    return os << ')';
}

}
