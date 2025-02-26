#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Init final: Node {
    ID id;
    NodePtr type;
    NodePtr val;

    Init(ID&& id, NodePtr&& type, NodePtr&& val, Pos src) noexcept:
    Node(src), id(std::move(id)), type(std::move(type)), val(std::move(val)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::INIT;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Init&>(that);
        return
            id == casted.id && npeq(type, casted.type) && npeq(val, casted.val);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return
            os << category() << "(" << id << ", " << type << ", " << val << ")";
    }
};

}