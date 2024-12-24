#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct VarKwargsDef final: Node_<NodeID::VAR_KWARGS_DEF> {
    ID id;
    NodePtr type;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const ArgDef&>(that);
        return
            id == casted.id &&
            npeq(type, casted.type)
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << type;
    }
};

}
