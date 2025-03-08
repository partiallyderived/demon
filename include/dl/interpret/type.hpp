#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

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

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Type&>(that);
        return
            id == casted.id &&
            nodes_eq(parents, casted.parents) &&
            nodes_eq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::TYPE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, id, parents, body);
    }
};

}
