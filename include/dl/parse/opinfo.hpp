#pragma once

#include <cstdint>

#include "dl/pos.hpp"

namespace dl::syntax {
    struct Pushed;
}

namespace dl::parse {

// Holds data relevant to a pushed operator.
struct OpInfo {
     // Operator on top of the stack.
    const syntax::Pushed* op;

    // Location of `op` in the file being processed.
    Pos loc;

    // Tracks whether we are enclosed between brackets.
    // This has implications for whether leading spaces and dedents are allowed,
    // and whether newlines separate statements.
    bool in_brackets;

    // Keeps track of number of nodes in the chain for chained operators.
    std::uint32_t count;

    // Simple init constructor, defaults count to 0.
    OpInfo(
        const syntax::Pushed* op,
        Pos loc,
        bool in_brackets,
        std::uint32_t count = 0
    ) noexcept: op(op), loc(loc), in_brackets(in_brackets), count(count) {}
};

}
