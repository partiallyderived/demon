#pragma once

#include <cstdint>

#include <string>
#include <utility>

#include "dl/lex/tokenid.hpp"

namespace dl {
struct Token {
    TokenID id;

    // String content of the token, if not inferrable from the ID.
    std::string content;

    // Number of repetitions of the token. Only supported for space.
    std::uint32_t count;

    // For most tokens, the content is inferrable by the ID. For +, for example,
    // there is no need to store the content as "+".
    Token(TokenID id) noexcept: id(id), content(), count(1) {}

    Token(TokenID id, std::string&& content) noexcept:
    id(id), content(std::move(content)), count(1) {}

    Token(TokenID id, std::uint32_t count) noexcept:
    id(id), content(), count(count) {}
};

}
