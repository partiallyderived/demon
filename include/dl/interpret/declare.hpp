#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Declare final: Node {
    ID id;
    NodePtr type;

    Declare(ID&& id, NodePtr&& type, Pos src) noexcept:
    Node(src), id(std::move(id)), type(std::move(type)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::DECLARE;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Declare&>(that);
        return id == casted.id && npeq(type, casted.type);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" << id << ", " << type << ")";
    }
};

}
