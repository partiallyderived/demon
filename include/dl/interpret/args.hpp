#pragma once

#include <ostream>

#include <vector>

#include "dl/err.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct Args final: Node_<NodeID::ARGS> {
    Nodes args;
    Nodes kwargs;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Args&>(that);
        return nodes_eq(args, casted.args) && nodes_eq(kwargs, casted.kwargs);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << args << ", " << kwargs;
    }
};

}
