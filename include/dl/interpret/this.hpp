#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct This final: Node_<NodeID::THIS> {
    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os;
    }
};

}
