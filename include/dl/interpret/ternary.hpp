#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Ternary final: Node_<NodeID::TERNARY> {
    NodePtr predicate;
    NodePtr if_case;
    NodePtr else_case;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Ternary&>(that);
        return
            npeq(predicate, casted.predicate) &&
            npeq(if_case, casted.if_case) &&
            npeq(else_case, casted.else_case);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << predicate << ", " << if_case << ", " << else_case;
    }
}

}
