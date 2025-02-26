#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/data.hpp"
#include "dl/interpret/defcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Def final: Node {
    ID id;
    std::vector<DefCase> cases;

    Def(ID&& id, std::vector<DefCase>&& cases, Pos src) noexcept:
    Node(src), id(std::move(id)), cases(std::move(cases)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::DEF;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Def&>(that);
        return id == casted.id && cases == casted.cases;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << "Def(" << id << ", " << OutContainerManip(cases) << ")";
    }
};

}
