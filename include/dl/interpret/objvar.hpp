#pragma once

#include <ostream>

#include <cstdint>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

// Like Var, but can refer to an enclosing object's attributes.
struct ObjVar final: Node_<NodeID::OBJ_VAR> {
    ID id;
    std::uint32_t up;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const ObjVar&>(that);
        return id == casted.id && up == casted.up;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << up;
    }
};

}
