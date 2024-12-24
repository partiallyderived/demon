#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/source.hpp"

namespace dl {

struct AddrType final: Node_<NodeID::ADDR_TYPE> {
    NodePtr of;
    Source src;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const AddrType&>(that);
        return npeq(of, casted.of) && src == that.src;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << of << ", " << src;
    }
};

}
