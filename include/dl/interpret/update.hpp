#pragma once

#include <ostream>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Update final: Node_<NodeID::UPDATE> {
    NodePtr object;
    Args args;
    NodePtr value;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Update&>(that);
        return
            npeq(object, casted.object) &&
            args == casted.args &&
            npeq(value, casted.value);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << object << ", " << args << ", " << value;
    }
};

}
