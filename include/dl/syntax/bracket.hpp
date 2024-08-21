#pragma once

#include "dl/parse/state.hpp"
#include "dl/syntax/binary.hpp"
#include "dl/syntax/err.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/optional.hpp"
#include "dl/syntax/precedence.hpp"
#include "dl/syntax/token.hpp"
#include "dl/syntax/unary.hpp"

namespace dl::syntax {

// Indicates that a "right" operator did not match with its left counterpart
// during processing, like if a right parenthesis saw a left square bracket on
// the stack.
struct UnmatchedRightErr: SyntaxErr {
    Kind op;
    Kind right;

    UnmatchedRightErr(Kind op, Kind right, Pos loc) noexcept:
    SyntaxErr(loc), op(op), right(right) {}

    bool equals(const Err& that) const noexcept override {
        auto thatptr = dynamic_cast<const UnmatchedRightErr*>(&that);
        return
            LocatedErr::equals(that) &&
            op == thatptr->op &&
            right == thatptr->right;
    }

    std::ostream& out_data(std::ostream& os) const override {
        return LocatedErr::out_data(os) << ", " << op << ", " << right;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnmatchedRightErr";
    }
};

// "Left" operators are the first of a pair of matching operators, like a left
// parenthesis or an indent.
// They are an optional operator because they may not have any enclosed
// elements, as in the case of a left parenthesis immediately followed by a
// right parenthesis.
template<Kind KIND, typename NodeType>
struct LeftOp: OptionalOp<KIND, NodeType, Precedence::LEFT_LEFT> {};

// Operators corresponding to left brackets of all kinds.
template<Kind KIND, typename NodeType>
struct LeftBracketOp: LeftOp<KIND, NodeType> {
    bool left_bracket() const noexcept final override {
        return true;
    }
};

// "Right" operators are the second of a pair of matching operators, like a
// right parenthesis or a dedent.
template<Kind KIND, Kind LEFT_KIND>
struct RightOp: Precedented_<KIND, Precedence::RIGHT_RIGHT> {
    virtual ErrPtr onprecedes(parse::State& state) const override {
        if (state.op()->kind() != LEFT_KIND)
            return ErrPtr(new UnmatchedRightErr(
                state.op()->kind(), this->kind(), state.opinfo().loc
            ));
        return Precedented::process(state);
    }
};

template<Kind KIND, auto OPERATOR, auto JOIN_OPERATOR>
struct LeftToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) override {
        return Action(parse::Context::LEFT, OPERATOR);
    }

    virtual ActionRes onright(parse::State& state) override {
        // We have two operators to process: One invisible operator from
        // concatenation, such as call, and the left operator itself, such as a
        // parenthetical grouping. Queue the left operator to be handled
        // afterwards.
        state.next_op = OPERATOR;
        return Action(parse::Context::LEFT, JOIN_OPERATOR);
    }
};

template<Kind KIND, auto OPERATOR>
struct RightToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) override {
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }

    virtual ActionRes onright(parse::State& state) noexcept override {
        return Action(parse::Context::RIGHT, OPERATOR);
    }
};

template<Kind LEFT_KIND, Kind RIGHT_KIND>
struct BracketPack {
    struct Node final: UnaryNode<LEFT_KIND> {
        using UnaryNode<LEFT_KIND>::UnaryNode;
    };

    struct LeftOperator final: LeftOp<LEFT_KIND, Node> {};
    static constexpr auto LEFT_OPERATOR = LeftOperator();

    struct RightOperator final: RightOp<RIGHT_KIND, LEFT_KIND> {};
    static constexpr auto RIGHT_OPERATOR = RightOperator();

    struct LeftToken final: syntax::LeftToken<
        LEFT_KIND, &LEFT_OPERATOR, &Call::OPERATOR
    > {};
    static LeftToken LEFT_TOKEN;

    struct RightToken final: syntax::RightToken<RIGHT_KIND, &RIGHT_OPERATOR> {};
    static RightToken RIGHT_TOKEN;
};

template<Kind LEFT_KIND, Kind RIGHT_KIND>
typename BracketPack<LEFT_KIND, RIGHT_KIND>::LeftToken
BracketPack<LEFT_KIND, RIGHT_KIND>::LEFT_TOKEN = LeftToken();

template<Kind LEFT_KIND, Kind RIGHT_KIND>
typename BracketPack<LEFT_KIND, RIGHT_KIND>::RightToken
BracketPack<LEFT_KIND, RIGHT_KIND>::RIGHT_TOKEN = RightToken();

using Curved = BracketPack<Kind::GROUP, Kind::END_GROUP>;
using Square = BracketPack<Kind::LIST , Kind::END_LIST>;

}
