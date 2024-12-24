#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Addr final: Node_<NodeID::ADDR> {
    NodePtr of;

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(of, dynamic_cast<const Addr&>(that).of);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << of;
    }
};

}
