#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Or final: Node_<NodeID::OR> {
    NodePtr lhs;
    NodePtr rhs;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Or&>(that);
        return npeq(lhs, casted.lhs) && npeq(rhs, casted.rhs);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << lhs << ", " << rhs;
    }
};

}
