#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct GetAttr final: Node_<NodeID::GET_ATTR> {
    NodePtr object;
    ID attr;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const GetAttr&>(that);
        return npeq(object, casted.object) && attr == casted.attr;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << object << ", " << attr;
    }
};

}
