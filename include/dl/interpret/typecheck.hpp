#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct TypeCheck final: Node_<NodeID::TYPE_CHECK> {
    NodePtr obj;
    NodePtr type;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const TypeCheck&>(that);
        return npeq(obj, casted.obj) && npeq(type, casted.type);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << obj << ", " << type;
    }
};

}
