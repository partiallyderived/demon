#pragma once

#include <ostream>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

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

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::SET_ATTR;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const SetAttr&>(that);
        return
            npeq(object, casted.object) &&
            npeq(attr, casted.attr) &&
            npeq(value, casted.value);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            object << ", " << attr << ", " << value <<
        ")";
    }
};
    
}
