#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/case.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct If final: Node {
    std::vector<Case> cases;
    Nodes orelse;

    If(std::vector<Case>&& cases, Nodes&& orelse, Pos src) noexcept:
    Node(src), cases(std::move(cases)), orelse(std::move(orelse)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::IF;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const If&>(that);
        return cases == casted.cases && nodes_eq(orelse, casted.orelse);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "("
            << OutContainerManip(cases) << ", " <<
            orelse <<
        ")";
    }
};

}
