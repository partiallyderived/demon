#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/data.hpp"
#include "dl/interpret/defcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Def final: Node {
    NodePtr id;
    std::vector<DefCase> cases;

    Def(NodePtr&& id, std::vector<DefCase>&& cases, Pos src) noexcept:
    Node(src),
    id(std::move(id)),
    cases(std::move(cases)) {}

    virtual NodeKind kind() const noexcept override {
        return NodeKind::DEF;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Def&>(that);
        return npeq(id, casted.id) && cases == casted.cases;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << OutContainerManip(cases);
    }
};

}
