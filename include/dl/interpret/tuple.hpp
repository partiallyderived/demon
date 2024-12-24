#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Tuple final: Node_<NodeID::TUPLE> {
    Nodes elements;

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(elements, dynamic_cast<const Tuple&>(that).elements);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << elements;
    }
};

}
