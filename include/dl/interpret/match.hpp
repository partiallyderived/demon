#pragma once

#include <ostream>
#include <vector>

#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/util.hpp"

namespace dl {

struct Match final: Node_<NodeID::MATCH> {
    NodePtr matchee;
    std::vector<MatchCase> cases;
    Block orelse;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Match&>(that);
        return
            npeq(matchee, casted.matchee) &&
            cases == casted.cases &&
            orelse == casted.orelse;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << matchee << ", " << out_container(cases) << ", " << orelse;
    }
};

}
