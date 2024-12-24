#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct ArgDef final: Node_<NodeID::ARG_DEF> {
    ID id;
    NodePtr type;
    NodePtr dflt;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const ArgDef&>(that);
        return
            id == casted.id &&
            npeq(type, casted.type) &&
            npeq(dflt, casted.dflt);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << type << ", " << dflt;
    }
};

}
