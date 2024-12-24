#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Return final: Node_<NodeID::RETURN> {
    NodePtr value;

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(value, dynamic_cast<const Return&>(that).value);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << value;
    }
}

}
