#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/matchspec.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct MatchCase final: Node_<NodeID::MATCH_CASE> {
    NodePtr predicate;
    Block body;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const MatchCase&>(that);
        return npeq(predicate, casted.predicate) && body == casted.body;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << predicate << ", " << body;
    }
};

}
