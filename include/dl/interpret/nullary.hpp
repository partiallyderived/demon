#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/interpret/nullarykind.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Nullary final: Node {
    NullaryKind kind;

    Nullary(NullaryKind kind, Pos src) noexcept: Node(src), kind(kind) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::NULLARY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        return kind == dynamic_cast<const Nullary&>(that).kind;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << kind;
    }
};

}
