#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Symbol final: Node_<NodeID::SYMBOL> {
    ID of;

    virtual bool equals(const Node& that) const noexcept override {
        return of == dynamic_cast<const Symbol&>(that).of;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << of;
    }
}

}
