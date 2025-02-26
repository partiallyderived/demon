#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/interpret/unarykind.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Unary final: Node {
    UnaryKind kind;
    NodePtr arg;

    Unary(UnaryKind kind, NodePtr&& arg, Pos src) noexcept:
    Node(src), kind(kind), arg(std::move(arg)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::UNARY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Unary&>(that);
        return kind == casted.kind && npeq(arg, casted.arg);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << kind << "(" << arg << ")";
    }
};

}
