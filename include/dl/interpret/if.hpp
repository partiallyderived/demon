#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct If final: Node_<NodeID::IF> {
    NodePtr predicate;
    Block body;
    Block orelse;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const If&>(that);
        return
            npeq(predicate, casted.predicate) &&
            body == casted.body &&
            orelse == casted.orelse;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << predicate << ", " << body << ", " << orelse;
    }
};

}
