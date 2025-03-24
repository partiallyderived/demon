#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Match final: Node {
    NodePtr matchee;
    std::vector<MatchCase> cases;

    Match(NodePtr&& matchee, std::vector<MatchCase>&& cases, Pos src) noexcept:
    Node(src), matchee(std::move(matchee)), cases(std::move(cases)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Match&>(that);
        return npeq(matchee, casted.matchee) && cases == casted.cases;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << matchee << ", " << OutContainerManip(cases);
    }
};

}
