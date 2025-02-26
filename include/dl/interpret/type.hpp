#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Type final: Node {
    ID id;
    Nodes parents;
    Nodes body;

    Type(ID&& id, Nodes&& parents, Nodes&& body, Pos src) noexcept:
    Node(src),
    id(std::move(id)),
    parents(std::move(parents)),
    body(std::move(body)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::TYPE;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Type&>(that);
        return
            id == casted.id &&
            nodes_eq(parents, casted.parents) &&
            nodes_eq(body, casted.body);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            id << ", " << parents << ", " << body <<
        ")";
    }
};

}
