#pragma once

#include "dl/syntax/binary.hpp"
#include "dl/syntax/bracket.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/unary.hpp"

namespace dl::syntax {

struct Block {
    struct Node final: UnaryNode<Kind::BLOCK> {
        using UnaryNode<Kind::BLOCK>::UnaryNode;
    };

    struct Operator final: LeftOp<Kind::BLOCK, Node> {};

    static constexpr auto OPERATOR = Operator();
};

struct EndBlock {
    struct Operator final: RightOp<Kind::END_BLOCK, Kind::BLOCK> {};
    static constexpr auto OPERATOR = Operator();
};

struct Statements {
    struct Node final: ChainedBinaryNode<Kind::STATEMENTS> {
        using ChainedBinaryNode<Kind::STATEMENTS>::ChainedBinaryNode;
    };

    struct Operator final: CollectedChainedOp<
        Kind::STATEMENTS, Node, 1, Precedence::STATEMENTS
    > {};

    static constexpr auto OPERATOR = Operator();
};

}
