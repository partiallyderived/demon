#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct Binary final: Node {
    NodePtr lhs;
    NodePtr rhs;

    Binary(NodePtr&& lhs, NodePtr&& rhs, Pos src) noexcept:
    Node(src), lhs(std::move(lhs)), rhs(std::move(rhs))  {}

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Binary&>(that);
        return npeq(lhs, casted.lhs) && npeq(rhs, casted.rhs);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, lhs, rhs);
    }
};

using And = Binary<NodeKind::AND>;
using As = Binary<NodeKind::AS>;
using Assign = Binary<NodeKind::ASSIGN>;
using Declare = Binary<NodeKind::DECLARE>;
using Defaulted = Binary<NodeKind::DEFAULTED>;
using Entry = Binary<NodeKind::ENTRY>;
using GetAttr = Binary<NodeKind::GET_ATTR>;
using In = Binary<NodeKind::IN>;
using Or = Binary<NodeKind::OR>;
using TypeMatch = Binary<NodeKind::TYPE_MATCH>;

}
