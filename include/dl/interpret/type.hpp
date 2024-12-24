#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Type final: Node_<NodeID::TYPE> {
    ID id;
    Nodes parents;
    Block body;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Type&>(that);
        return
            id == casted.id &&
            nodes_eq(parents, casted.parents) &&
            body == casted.body;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << parents << ", " << body;
    }
};

}
