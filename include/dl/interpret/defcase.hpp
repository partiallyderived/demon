#pragma once

#include <ostream>
#include <vector>

#include "dl/interpret/argspec.hpp"
#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct DefCase final: Node_<NodeID::DEF_CASE> {
    ArgSpec spec;
    NodePtr returns;
    Block body;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const DefCase&>(that);
        return
            spec == casted.spec &&
            npeq(returns, casted.returns) &&
            body == casted.body;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << spec << ", " << returns << ", " << body;
    }
};

}