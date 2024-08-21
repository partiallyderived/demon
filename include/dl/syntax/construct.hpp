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
struct ConstructNode: Node_<KIND> {
    // Arg refers to the predicate/body of the first of multiple multi-block
    // constructs, such as the labeled node following `if`.
    NodePtr arg;

    // Next refers to the predicate/body of the *second* of multiple multi-block
    // constructs, such as the labeled node following `elif` which itself
    // follows an `if`.
    // More constructs may be found recursively through the `next` field of
    // `next`.
    NodePtr next;

    ConstructNode(NodePtr&& arg) noexcept: arg(std::move(arg)), next(nullptr) {}

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        auto thatptr = dynamic_cast<const ConstructNode*>(&that);
        return
            *arg == *(thatptr->arg) && (
                next == thatptr->next || (
                    next && thatptr->next && *next == *thatptr->next
                )
            );
    }

    virtual std::ostream& out(std::ostream& os) const override {
        os << KIND << "(" << *arg << ", ";
        if (next)
            os << *this->next;
        else
            os << "nullptr";
        return os << ")";
    }
};

// Construct tokens correspond to constructs like "if" and "for".
template<Kind KIND, auto OPERATOR>
struct ConstructToken: Token_<KIND> {
    virtual ActionRes onstart(parse::State& state) override {
        return Action(parse::Context::LEFT, OPERATOR);
    }

    virtual ActionRes onleft(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }

    virtual ActionRes onright(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }
};

// Base class for If and Elif.
template<Kind KIND>
struct IfBase {
    struct Node final: ConstructNode<KIND> {
        using ConstructNode<KIND>::ConstructNode;

        bool offer(NodePtr&& node) override {
            if (this->ready(node->kind()))
                // Not elif or else, don't accept.
                return false;
            // Otherwise, see whether `next` is a nullptr or not.
            if (this->next)
                // If not, defer to `next` for consumption.
                return this->next->offer(std::move(node));
            // Otherwise, accept immediately by moving into next.
            this->next = std::move(node);
            return true;
        }

        bool ready() const noexcept override {
            // If we have accepted an else, no more parsing is necessary.
            return this->next() &&
                (this->next.kind() == Kind::ELSE || this->next->ready());
        }

        bool ready(Kind next) const noexcept override {
            return
                this->next->kind() != Kind::ELIF &&
                this->next->kind() != Kind::ELSE;
        }
    };

    struct Operator final: UnaryOp<KIND, Node, Precedence::FLOW> {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: ConstructToken<KIND, &OPERATOR> {};
    static Token TOKEN;
};

template<Kind KIND>
typename IfBase<KIND>::Token IfBase<KIND>::TOKEN = Token();

using If = IfBase<Kind::IF>;
using Elif = IfBase<Kind::ELIF>;

struct For {
    struct Node final: ConstructNode<Kind::FOR> {
        using ConstructNode<Kind::FOR>::ConstructNode;
        bool offer(NodePtr&& node) override {
            if (node->kind() != Kind::ELSE || this->next)
                // Either the offered node is not an else or we have already
                // consumed an else. Either way, reject it.
                // Note that the error resulting from a standalone "else" is
                // handled when the interpreter attempts to execute that "else",
                // not here.
                return false;
            this->next = std::move(node);
            return true;
        }

        bool ready(Kind next) const noexcept override {
            return this->next->kind() != Kind::ELSE;
        }
    };

    struct Operator final: UnaryOp<Kind::FOR, Node, Precedence::FLOW> {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: ConstructToken<Kind::FOR, &OPERATOR> {};
    static Token TOKEN;
};

auto For::TOKEN = Token();

struct Else {
    // Else can have no child constructs so it may as well be a unary node.
    struct Node final: UnaryNode<Kind::ELSE> {
        using UnaryNode<Kind::ELSE>::UnaryNode;
    };

    struct Operator final: UnaryOp<Kind::ELSE, Node, Precedence::FLOW> {};
    static constexpr auto OPERATOR = Operator();

    struct Token final: ConstructToken<Kind::ELSE, &OPERATOR> {};
    static Token TOKEN;
};

auto Else::TOKEN = Token();

}
