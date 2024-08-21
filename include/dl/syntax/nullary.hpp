#pragma once

#include <ostream>

#include "dl/parse/state.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/precedence.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

template<Kind KIND>
struct NullaryNode: Node_<KIND> {
    virtual bool operator==(const Node& that) const noexcept override {
        return this->kind() == that.kind();
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << KIND;
    }
};

// Nullary operators push a nullary node immediately upon gaining precedence.
template<Kind KIND, typename NodeType, Precedence PRECEDENCE>
struct NullaryOp: Precedented_<KIND, PRECEDENCE> {
    virtual ErrPtr onprecedes(parse::State& state) const override {
        state.push(new NodeType());
        return nullptr;
    }
};

template<Kind KIND, auto OPERATOR>
struct NullaryToken: Token_<KIND> {
    // It is common to all nullary operators to only be valid at the start of a
    // statement.
    virtual ActionRes onstart(parse::State& state) noexcept override {
        return Action(parse::Context::END, OPERATOR);
    }

    virtual ActionRes onleft(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }

    virtual ActionRes onright(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }
};

template<Kind KIND, Precedence PRECEDENCE>
struct NullaryPack {
    struct Node final: NullaryNode<KIND> {};

    struct Operator final: NullaryOp<KIND, Node, PRECEDENCE> {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: NullaryToken<KIND, &OPERATOR> {};
    static Token TOKEN;
};

template<Kind KIND, Precedence PRECEDENCE>
typename NullaryPack<KIND, PRECEDENCE>::Token
NullaryPack<KIND, PRECEDENCE>::TOKEN = Token();

using Break = NullaryPack<Kind::BREAK, Precedence::FLOW>;
using Continue = NullaryPack<Kind::CONTINUE, Precedence::FLOW>;

}
