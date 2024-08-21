#pragma once

#include <ostream>
#include <type_traits>

#include "dl/parse/state.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/precedence.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

template<Kind KIND>
struct UnaryNode: Node_<KIND> {
    NodePtr node;

    UnaryNode(NodePtr&& node) noexcept: node(std::move(node)) {}

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        return *node == *(dynamic_cast<const UnaryNode*>(&that)->node);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << KIND << "(" << *node << ")";
    }
};

template<Kind KIND>
struct ChainedUnaryNode: Node_<KIND> {
    std::uint32_t count;
    NodePtr node;

    ChainedUnaryNode(std::uint32_t count, NodePtr&& node) noexcept:
    count(count), node(std::move(node)) {}

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        auto thatptr = dynamic_cast<const ChainedUnaryNode*>(&that);
        return count == thatptr->count && *node == *(thatptr->node);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << KIND << "(" << count << ", " << *node << ")";
    }
};

// Unary operators yield a node which is a parent to a single node.
template<Kind KIND, typename NodeType, Precedence PRECEDENCE>
struct UnaryOp: Pushed_<KIND, PRECEDENCE, Precedence::UNARY_RIGHT> {
    // When popped, use the node on top of the stack to construct the result
    // node.
    virtual ErrPtr onpop(parse::State& state) const override {
        state.push(new NodeType(state.pop()));
        return nullptr;
    }
};

// Chained unary operator, which means we only need to keep track of a count for
// this operator.
template<Kind KIND, typename NodeType, Precedence PRECEDENCE>
struct ChainedUnaryOp: ChainedOp<KIND, 1, PRECEDENCE, Precedence::UNARY_RIGHT> {
    virtual ErrPtr onpop(parse::State& state) const override {
        state.push(new NodeType(state.opinfo().count, state.pop()));
        return nullptr;
    }
};

template<Kind KIND, auto OPERATOR>
struct UnaryToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) noexcept override {
        return Action(parse::Context::LEFT, OPERATOR);
    }

    virtual ActionRes onright(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }
};

template<Kind KIND, Precedence PRECEDENCE, bool CHAINED = false>
struct UnaryPack {
    using NodeBase = std::conditional_t<
        CHAINED, ChainedUnaryNode<KIND>, UnaryNode<KIND>
    >;
    struct Node final: NodeBase {
        using NodeBase::NodeBase;
    };

    struct Operator final: std::conditional_t<
        CHAINED,
        ChainedUnaryOp<KIND, Node, PRECEDENCE>,
        UnaryOp<KIND, Node, PRECEDENCE>
    > {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: UnaryToken<KIND, &OPERATOR> {};
    static Token TOKEN;
};

template<Kind KIND, Precedence PRECEDENCE, bool CHAINED>
typename UnaryPack<KIND, PRECEDENCE, CHAINED>::Token
UnaryPack<KIND, PRECEDENCE, CHAINED>::TOKEN = Token();

using Not   = UnaryPack<Kind::NOT , Precedence::NOT   >;
using Tilde = UnaryPack<Kind::BNOT, Precedence::PREFIX>;

}
