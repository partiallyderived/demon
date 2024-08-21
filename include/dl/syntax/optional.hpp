#pragma once

#include <ostream>
#include <utility>

#include "dl/parse/state.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/precedence.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

template<Kind KIND>
struct OptionalNode: Node_<KIND> {
    NodePtr node;

    OptionalNode(NodePtr&& node) noexcept: node(std::move(node)) {}

    bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        auto thatptr = dynamic_cast<const OptionalNode*>(&that);
        return
            node == thatptr->node ||
            (node && thatptr->node && *node == *thatptr->node);
    }

    std::ostream& out(std::ostream& os) const override {
        os << KIND << "(";
        if (node)
            os << *node;
        else
            os << "nullptr";
        return os << ")";
    }
};

// Optional operators may be followed by an optional argument.
template<Kind KIND, typename NodeType, Precedence PRECEDENCE>
struct OptionalOp: Pushed_<KIND, PRECEDENCE, Precedence::UNARY_RIGHT> {
    virtual ErrPtr onprecedes(parse::State& state) const override {
        Pushed::onprecedes(state);
        // This nullptr is used to indicate a lack of an operand.
        state.push(nullptr);
        return nullptr;
    }

    virtual ErrPtr onpop(parse::State& state) const override {
        syntax::NodePtr node = state.pop();
        if (node != nullptr)
            // Pop a nullptr pushed as a mark for nodes expecting an optional
            // child.
            state.pop();
        state.push(new NodeType(std::move(node)));
        return nullptr;
    }
};

// It does not seem likely that the properties of return will apply to optional
// tokens in general (assuming more are even implemented), so no `OptionalToken`
// or `OptionalPack` class is defined.
struct Return {
    struct Node final: OptionalNode<Kind::RETURN> {
        using OptionalNode<Kind::RETURN>::OptionalNode;
    };

    struct Operator final: OptionalOp<Kind::RETURN, Node, Precedence::FLOW> {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: Token_<Kind::RETURN> {
        ActionRes onstart(parse::State& state) noexcept override {
            return Action(parse::Context::OPTIONAL, &OPERATOR);
        }

        ActionRes onleft(parse::State& state) override {
            return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
        }

        ActionRes onright(parse::State& state) override {
            return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
        }
    };
    static Token TOKEN;
};

Return::Token Return::TOKEN = Token();

}
