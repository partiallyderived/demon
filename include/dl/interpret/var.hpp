#pragma once

#include <cstdint>

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

// Like an ID, but can refer to vars in an outer scope.
struct Var final: Node_<NodeID::VAR> {
    ID id;
    std::uint32_t up;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Vars&>(that);
        return id == casted.id && up == casted.up;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << up;
    }
};

}
