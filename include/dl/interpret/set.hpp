#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Set final: Node_<NodeID::SET> {
	ID id;
	NodePtr value;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Set&>(that);
        return id == casted.id && npeq(value, casted.value);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << value;
    }
};

}
