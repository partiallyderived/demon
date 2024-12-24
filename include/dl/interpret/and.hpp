#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct And final: Node_<NodeID::AND> {
    NodePtr lhs;
    NodePtr rhs;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const And&>(that);
        return npeq(lhs, casted.lhs) && npeq(rhs, casted.rhs);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << lhs << ", " << rhs;
    }
};

}
