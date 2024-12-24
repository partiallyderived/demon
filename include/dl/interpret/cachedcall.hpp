#pragma once

#include <ostream>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct CachedCall final: Node_<NodeID::CACHED_CALL> {
    NodePtr callee;
    Args args;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const CachedCall&>(that);
        return npeq(callee, casted.callee) && args == casted.args;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << callee << ", " << args;
    }
}

}
