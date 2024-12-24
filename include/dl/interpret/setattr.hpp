#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {
    
struct SetAttr final: Node_<NodeID::SET_ATTR> {
    NodePtr object;
    ID attr;
    NodePtr value;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const SetAttr&>(that);
        return
            npeq(object, casted.object) &&
            attr == casted.attr &&
            npeq(value, casted.value);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << object << ", " << attr << ", " << value;
    }
};
    
}
