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

struct Match final: Node {
    NodePtr matchee;
    std::vector<Case> cases;
    Nodes orelse;

    Match(NodePtr&& matchee, std::vector<Case>&& cases, Nodes&& orelse, Pos src)
    noexcept:
    Node(src),
    matchee(std::move(matchee)),
    cases(std::move(cases)),
    orelse(std::move(orelse)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::MATCH;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Match&>(that);
        return
            npeq(matchee, casted.matchee) &&
            cases == casted.cases &&
            nodes_eq(orelse, casted.orelse);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            matchee << ", " << OutContainerManip(cases) << ", " << orelse <<
        ")";
    }
};

}
