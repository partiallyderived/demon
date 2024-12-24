#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct For final: Node_<NodeID::FOR> {
    ID id;
    NodePtr from;
    NodePtr to;
    NodePtr by;
    Block body;
    Block orelse;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const For&>(that);
        return
            id == casted.id &&
            npeq(from, casted.from) &&
            npeq(to, casted.to) &&
            npeq(by, casted.by) &&
            body == casted.body &&
            orelse == casted.orelse;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os
            << id << ", "
            << from << ", "
            << to << ", "
            << by << ", "
            << body << ", "
            << orelse;
    }
};

}
