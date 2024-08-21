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
struct BinaryNode: Node_<KIND> {
    NodePtr left;
    NodePtr right;

    BinaryNode(NodePtr&& left, NodePtr&& right) noexcept:
    left(std::move(left)), right(std::move(right)) {}

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        auto thatptr = dynamic_cast<const BinaryNode*>(&that);
        return *left == *(thatptr->left) && *right == *(thatptr->right);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << KIND << "(" << *left << ", " << *right << ")";
    }
};

// Binary operators yield a node which is a parent to an lhs node and a rhs
// node.
template<
    Kind KIND,
    typename NodeType,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
struct BinaryOp: Pushed_<KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE> {
    // When popped, consolidate the left and right nodes into the result node.
    virtual ErrPtr onpop(parse::State& state) const override {
        syntax::NodePtr rhs = state.pop();
        syntax::NodePtr lhs = state.pop();
        state.push(new NodeType(std::move(lhs), std::move(rhs)));
        return nullptr;
    }
};

// Chained binary operator, which applies itself to all collected elements once
// rather than successive on pairs.
template<
    Kind KIND,
    typename NodeType,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE
>
using ChainedBinaryOp = CollectedChainedOp<
    KIND, NodeType, 2, LEFT_PRECEDENCE, RIGHT_PRECEDENCE
>;

template<Kind KIND>
struct ChainedBinaryNode: Node_<KIND> {
    Nodes nodes;

    ChainedBinaryNode(NodePtr* begin, NodePtr* end): nodes(end - begin) {
        for (NodePtr* p = begin; p != end; p++)
            nodes.emplace_back(p->release());
    }

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        auto thatptr = dynamic_cast<const ChainedBinaryNode*>(&that);
        for (std::uint32_t i = 0; i < nodes.size(); i++) {
            if (*nodes[i] != *(thatptr->nodes[i]))
                return false;
        }
        return true;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        os << KIND << "(";
        for (std::uint32_t i = 0; i < nodes.size() - 1; i++) {
            os << *nodes[i] << ", ";
        }
        return os << *nodes.back() << ")";
    }
};

template<Kind KIND, auto OPERATOR>
struct BinaryToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }

    virtual ActionRes onright(parse::State& state) noexcept override {
        return Action(parse::Context::LEFT, OPERATOR);
    }
};

// `BinaryOpPack` is a grouping of a binary operator and its corresponding node.
// A corresponding token is omitted: some binary operators are implied through
// concatenation, like function calls.
template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE,
    bool CHAINED = false
>
struct BinaryOpPack {
    using NodeBase = std::conditional_t<
        CHAINED, ChainedBinaryNode<KIND>, BinaryNode<KIND>
    >;
    struct Node final: NodeBase {
        using NodeBase::NodeBase;
    };

    struct Operator final: std::conditional_t<
        CHAINED,
        ChainedBinaryOp<KIND, Node, LEFT_PRECEDENCE, RIGHT_PRECEDENCE>,
        BinaryOp<KIND, Node, LEFT_PRECEDENCE, RIGHT_PRECEDENCE>
    > {};
    static constexpr auto OPERATOR = Operator();
};

template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
using ChainedBinaryOpPack =
    BinaryOpPack<KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, true>;

// `BinaryPack` is like `BinaryOpPack`, but it includes the token.
template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE,
    bool CHAINED = false
>
struct BinaryPack: BinaryOpPack<
    KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, CHAINED
> {
    struct Token final: BinaryToken<
        KIND, &BinaryOpPack<
            KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, CHAINED
        >::OPERATOR
    > {};
    static Token TOKEN;
};

template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE,
    bool CHAINED
>
typename BinaryPack<KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, CHAINED>::Token
BinaryPack<
    KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, CHAINED
>::TOKEN = Token();

template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
using ChainedBinaryPack =
    BinaryPack<KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE, true>;

using Call   = BinaryOpPack<Kind::CALL,   Precedence::CALL  >;
using Suffix = BinaryOpPack<Kind::SUFFIX, Precedence::SUFFIX>;

using Concat = ChainedBinaryOpPack<Kind::CONCAT, Precedence::CONCAT>;

using Ampersand        = BinaryPack<Kind::BAND, Precedence::BAND >;
using And              = BinaryPack<Kind::AND , Precedence::AND  >;
using Carot            = BinaryPack<Kind::BXOR, Precedence::BXOR >;
using DoubleLeftAngle  = BinaryPack<Kind::LSH , Precedence::SHIFT>;
using DoubleRightAngle = BinaryPack<Kind::RSH,  Precedence::SHIFT>;
using Or               = BinaryPack<Kind::OR ,  Precedence::OR   >;
using Pipe             = BinaryPack<Kind::BOR , Precedence::BOR  >;
using Plus             = BinaryPack<Kind::ADD , Precedence::ADD  >;
using Slash            = BinaryPack<Kind::DIV , Precedence::MUL  >;

using BangEquals       = BinaryPack<Kind::NEQ, Precedence::CMP>;
using DoubleEquals     = BinaryPack<Kind::EQ , Precedence::CMP>;
using LeftAngle        = BinaryPack<Kind::LT , Precedence::CMP>;
using LeftAngleEquals  = BinaryPack<Kind::LTE, Precedence::CMP>;
using RightAngle       = BinaryPack<Kind::GT , Precedence::CMP>;
using RightAngleEquals = BinaryPack<Kind::GTE, Precedence::CMP>;

using By   = BinaryPack<Kind::BY  , Precedence::FOR_PRED>;
using From = BinaryPack<Kind::FROM, Precedence::FOR_PRED>;
using To   = BinaryPack<Kind::TO  , Precedence::FOR_PRED>;

using Comma = ChainedBinaryPack<Kind::SEQ, Precedence::SEQ>;

// Convenience typedef for assignment operators.
template<Kind KIND>
using SetPack = BinaryPack<KIND, Precedence::LSET, Precedence::RSET>;

using Equals                 = SetPack<Kind::SET  >;
using AmpersandEquals        = SetPack<Kind::IBAND>;
using CarotEquals            = SetPack<Kind::IBXOR>;
using DoubleLeftAngleEquals  = SetPack<Kind::ILSH >;
using DoubleRightAngleEquals = SetPack<Kind::IRSH >;
using MinusEquals            = SetPack<Kind::ISUB >;
using PercentEquals          = SetPack<Kind::IMOD >;
using PipeEquals             = SetPack<Kind::IBOR >;
using PlusEquals             = SetPack<Kind::IADD >;
using SlashEquals            = SetPack<Kind::IDIV >;
using StarEquals             = SetPack<Kind::IMUL >;

}
