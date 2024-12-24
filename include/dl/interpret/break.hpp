#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Break final: Node_<NodeID::BREAK> {
    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }
};

};
