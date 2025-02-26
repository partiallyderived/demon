#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct LambdaPosVar final: Node {
    Int32 pos;

    LambdaPosVar(Int32 pos, Pos src) noexcept: Node(src), pos(pos) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::LAMBDA_POS;
    }

    virtual bool equals(const Node& that) const noexcept override {
        return pos == dynamic_cast<const LambdaPosVar&>(that).pos;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" << pos << ")";
    }
};

}
