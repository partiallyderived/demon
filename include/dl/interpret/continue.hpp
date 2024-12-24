#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Continue final: Node_<NodeID::CONTINUE> {
    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }
};

}
