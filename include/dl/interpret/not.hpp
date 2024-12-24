#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Not final: Node_<NodeID::NOT> {
    NodePtr arg;

    virtual bool equals(const Node& that) const noexcept override {
        return arg == dynamic_cast<const Not&>(that).arg;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << arg;
    }
};

}
