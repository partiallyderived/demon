#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Raise final: Node_<NodeID::RAISE> {
    NodePtr raised;

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(raised, dynamic_cast<const Raise&>(that).raised);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << raised;
    }
};

}
