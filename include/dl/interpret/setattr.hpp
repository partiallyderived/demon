#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {
    
struct SetAttr final: Node {
    NodePtr object;
    NodePtr attr;
    NodePtr value;

    SetAttr(
        NodePtr&& object, NodePtr&& attr, NodePtr&& value, Pos src
    ) noexcept:
    Node(src),
    object(std::move(object)),
    attr(std::move(attr)),
    value(std::move(value)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const SetAttr&>(that);
        return
            npeq(object, casted.object) &&
            npeq(attr, casted.attr) &&
            npeq(value, casted.value);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::SET_ATTR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, object, attr, value);
    }
};
    
}
