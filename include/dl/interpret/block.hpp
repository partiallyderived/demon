#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Block final: Node_<NodeID::BLOCK> {
    Nodes code;

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(code, dynamic_cast<const Block&>(that).code);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << code;
    }
};

}
