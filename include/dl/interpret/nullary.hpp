#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"

namespace dl {

template<NodeKind KIND>
struct Nullary final: Node {
    Nullary(Pos src) noexcept: Node(src) {}

    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os;
    }
};

using Break = Nullary<NodeKind::BREAK>;
using Continue = Nullary<NodeKind::CONTINUE>;
using None = Nullary<NodeKind::NONE>;
using Null = Nullary<NodeKind::NULL_>;
using This = Nullary<NodeKind::THIS>;

}
