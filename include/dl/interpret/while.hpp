#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct While final: Node {
    NodePtr predicate;
    Nodes body;
    Nodes orelse;

    While(NodePtr&& predicate, Nodes&& body, Nodes&& orelse, Pos src) noexcept:
    Node(src),
    predicate(std::move(predicate)),
    body(std::move(body)),
    orelse(std::move(orelse)) {}

    virtual NodeKind kind() const noexcept override {
        return NodeKind::WHILE;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const While&>(that);
        return
            npeq(predicate, casted.predicate) &&
            nodes_eq(body, casted.body) &&
            nodes_eq(orelse, casted.orelse);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, body, orelse);
    }
};

}
