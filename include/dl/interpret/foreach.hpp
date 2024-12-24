#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct ForEach final: Node_<NodeID::FOR_EACH> {
    ID id;
    NodePtr iterable;
    Block body;
    Block orelse;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const ForEach&>(that);
        return
            id == casted.id &&
            npeq(iterable, casted.iterable) &&
            body == casted.body &&
            orelse == casted.orelse;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << iterable << ", " << body << ", " << orelse;
    }
};

}
