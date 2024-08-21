#pragma once

#include <type_traits>

#include "dl/parse/state.hpp"
#include "dl/syntax/binary.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/precedence.hpp"
#include "dl/syntax/token.hpp"
#include "dl/syntax/unary.hpp"

namespace dl::syntax {

template<Kind KIND, auto UNARY_OPERATOR, auto BINARY_OPERATOR>
struct MultiaryToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) override {
        return Action(parse::Context::LEFT, UNARY_OPERATOR);
    }

    virtual ActionRes onright(parse::State& state) override {
        return Action(parse::Context::LEFT, BINARY_OPERATOR);
    }
};

template<
    Kind UNARY_KIND,
    Kind BINARY_KIND,
    Precedence UNARY_PRECEDENCE,
    Precedence BINARY_LEFT_PRECEDENCE,
    Precedence BINARY_RIGHT_PRECEDENCE = BINARY_LEFT_PRECEDENCE,
    bool UNARY_CHAINED = false,
    bool BINARY_CHAINED = false
>
struct MultiaryPack {
    using UnaryNodeBase = std::conditional_t<
        UNARY_CHAINED,
        ChainedUnaryNode<UNARY_KIND>,
        syntax::UnaryNode<UNARY_KIND>
    >;
    using BinaryNodeBase = std::conditional_t<
        BINARY_CHAINED,
        ChainedBinaryNode<BINARY_KIND>,
        syntax::BinaryNode<BINARY_KIND>
    >;

    struct UnaryNode final: UnaryNodeBase {
        using UnaryNodeBase::UnaryNodeBase;
    };

    struct UnaryOp final: std::conditional_t<
        UNARY_CHAINED,
        ChainedUnaryOp<UNARY_KIND, UnaryNode, UNARY_PRECEDENCE>,
        syntax::UnaryOp<UNARY_KIND, UnaryNode, UNARY_PRECEDENCE>
    > {};
    static constexpr auto UNARY_OPERATOR = UnaryOp();

    struct BinaryNode final: BinaryNodeBase {
        using BinaryNodeBase::BinaryNodeBase;
    };

    struct BinaryOp final: std::conditional_t<
        BINARY_CHAINED,
        ChainedBinaryOp<
            BINARY_KIND,
            BinaryNode,
            BINARY_LEFT_PRECEDENCE,
            BINARY_RIGHT_PRECEDENCE
        >,
        syntax::BinaryOp<
            BINARY_KIND,
            BinaryNode,
            BINARY_LEFT_PRECEDENCE,
            BINARY_RIGHT_PRECEDENCE
        >
    > {};
    static constexpr auto BINARY_OPERATOR = BinaryOp();

    // Tokens will always use the kind of the binary operator rather than the
    // unary operator.
    struct Token final: MultiaryToken<
        BINARY_KIND, &UNARY_OPERATOR, &BINARY_OPERATOR
    > {};
    static Token TOKEN;
};

template<
    Kind UNARY_KIND,
    Kind BINARY_KIND,
    Precedence UNARY_PRECEDENCE,
    Precedence BINARY_LEFT_PRECEDENCE,
    Precedence BINARY_RIGHT_PRECEDENCE,
    bool UNARY_CHAINED,
    bool BINARY_CHAINED
>
typename MultiaryPack<
    UNARY_KIND,
    BINARY_KIND,
    UNARY_PRECEDENCE,
    BINARY_LEFT_PRECEDENCE,
    BINARY_RIGHT_PRECEDENCE,
    UNARY_CHAINED,
    BINARY_CHAINED
>::Token MultiaryPack<
    UNARY_KIND,
    BINARY_KIND,
    UNARY_PRECEDENCE,
    BINARY_LEFT_PRECEDENCE,
    BINARY_RIGHT_PRECEDENCE,
    UNARY_CHAINED,
    BINARY_CHAINED
>::TOKEN = Token();

// Convenience using definition that gives the same precedence to both sides of
// the binary operator.
template<
    Kind UNARY_KIND,
    Kind BINARY_KIND,
    Precedence UNARY_PRECEDENCE,
    Precedence BINARY_PRECEDENCE,
    bool UNARY_CHAINED = false,
    bool BINARY_CHAINED = false
>
using SymmetricMultiaryPack = MultiaryPack<
    UNARY_KIND,
    BINARY_KIND,
    UNARY_PRECEDENCE,
    BINARY_PRECEDENCE,
    BINARY_PRECEDENCE,
    UNARY_CHAINED,
    BINARY_CHAINED
>;

using Colon =
    MultiaryPack<Kind::EXPR, Kind::LABEL, Precedence::LABEL, Precedence::LABEL>;
using Dot =
    SymmetricMultiaryPack<
        Kind::MEMBER, Kind::GET, Precedence::PREFIX, Precedence::GET, true
    >;
using Minus =
    MultiaryPack<Kind::NEG, Kind::SUB, Precedence::PREFIX, Precedence::ADD>;
using Percent =
    SymmetricMultiaryPack<
        Kind::VAR, Kind::MOD, Precedence::PREFIX, Precedence::MUL, true
    >;
using Star =
    MultiaryPack<Kind::EXPAND, Kind::MUL, Precedence::PREFIX, Precedence::MUL>;



}
