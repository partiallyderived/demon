#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/binarykind.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Binary final: Node {
    BinaryKind kind;
    NodePtr lhs;
    NodePtr rhs;

    Binary(BinaryKind kind, NodePtr&& lhs, NodePtr&& rhs, Pos src) noexcept:
    Node(src), kind(kind), lhs(std::move(lhs)), rhs(std::move(rhs))  {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::BINARY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Binary&>(that);
        return
            kind == casted.kind &&
            npeq(lhs, casted.lhs) &&
            npeq(rhs, casted.rhs);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << kind << "(" << lhs << ", " << rhs << ")";
    }
};

}
