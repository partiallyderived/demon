#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Expansion final: Node_<NodeID::EXPANSION> {
    NodePtr of;

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(of, dynamic_cast<const Expansion&>(that).of);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << of;
    }
};

}