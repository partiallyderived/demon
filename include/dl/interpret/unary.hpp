#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"

namespace dl {

template<NodeKind KIND>
struct Unary final: Node {
    NodePtr arg;

    Unary(NodePtr&& arg, Pos src) noexcept: Node(src), arg(std::move(arg)) {}

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(arg, dynamic_cast<const Unary&>(that).arg);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << arg;
    }
};

using Addr = Unary<NodeKind::ADDR>;
using AddrType = Unary<NodeKind::ADDR_TYPE>;
using Expansion = Unary<NodeKind::EXPANSION>;
using Lambda = Unary<NodeKind::LAMBDA>;
using Not = Unary<NodeKind::NOT>;
using Raise = Unary<NodeKind::RAISE>;
using Return = Unary<NodeKind::RETURN>;
using Symbol = Unary<NodeKind::SYMBOL>;

}
