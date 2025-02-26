#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct GetAttr final: Node {
    NodePtr object;
    ID attr;

    GetAttr(NodePtr&& object, ID&& attr, Pos src) noexcept:
    Node(src), object(std::move(object)), attr(std::move(attr)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::GET_ATTR;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const GetAttr&>(that);
        return npeq(object, casted.object) && attr == casted.attr;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" << object << ", " << attr << ")";
    }
};

}
