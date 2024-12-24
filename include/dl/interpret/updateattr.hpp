#pragma once

#include <ostream>

#include "dl/interpret/args.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct UpdateAttr final: Node_<NodeID::UPDATE_ATTR> {
    NodePtr object;
    ID attr;
    Args args;
    NodePtr value;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const UpdateAttr&>(that);
        return
            npeq(object, casted.object) &&
            attr == casted.attr &&
            args == casted.args &&
            npeq(value, casted.value);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << object << ", " << attr << ", " << args << ", " << value;
    }
};

}
