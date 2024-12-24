#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Except final: Node_<NodeID::EXCEPT> {
    NodePtr predicate;
    Block code;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Except&>(that);
        return npeq(predicate, casted.predicate) && code == casted.code;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << predicate << ", " << code;
    }
};

}
