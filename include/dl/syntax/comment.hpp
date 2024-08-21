#pragma once

#include "dl/parse/state.hpp"
#include "dl/syntax/action.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

// Hash token is for comments.
struct Hash {
    struct Token final: Token_<Kind::HASH> {
        ActionRes onstart(parse::State& state) noexcept override {
            return Action(parse::Context::START, nullptr);
        }

        ActionRes onleft(parse::State& state) noexcept override {
            return Action(parse::Context::LEFT, nullptr);
        }

        ActionRes onright(parse::State& state) noexcept override {
            return Action(parse::Context::RIGHT, nullptr);
        }

        ActionRes onend(parse::State& state) noexcept override {
            return Action(parse::Context::END, nullptr);
        }

        ActionRes onoptional(parse::State& state) noexcept override {
            return Action(parse::Context::OPTIONAL, nullptr);
        }
    };

    static Token TOKEN;
};

Hash::Token Hash::TOKEN = Token();

}