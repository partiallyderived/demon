#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct TypeMatch final: Node_<NodeID::TYPE_MATCH> {
    ID symbol;
    NodePtr type;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const TypeMatch&>(that);
        return symbol == casted.symbol && npeq(type, casted.type);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << symbol << ", " << type;
    }
};

}
