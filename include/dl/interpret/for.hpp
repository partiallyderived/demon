#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct For final: Node {
    std::vector<ID> vars;
    NodePtr iterable;
    Nodes body;
    Nodes orelse;

    For(
        std::vector<ID>&& vars,
        NodePtr&& iterable,
        Nodes&& body,
        Nodes&& orelse,
        Pos src
    ) noexcept:
    Node(src),
    vars(std::move(vars)),
    iterable(std::move(iterable)),
    body(std::move(body)),
    orelse(std::move(orelse)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::FOR;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const For&>(that);
        return
            vars == casted.vars &&
            npeq(iterable, casted.iterable) &&
            nodes_eq(body, casted.body) &&
            nodes_eq(orelse, casted.orelse);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            OutContainerManip(vars) << ", " <<
            iterable << ", " <<
            body << ", " <<
            orelse <<
        ")";
    }
};

}
