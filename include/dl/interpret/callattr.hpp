#pragma once

#include <ostream>

#include "dl/interpret/args.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct CallAttr final: Node_<NodeID::CALL_ATTR> {
    NodePtr object;
    ID attr;
    Args args;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const CallAttr&>(that);
        return
            npeq(object, casted.object) &&
            attr == casted.attr &&
            args == casted.args;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << object << ", " << attr << ", " << args;
    }
};

}
