#pragma once

#include <ostream>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/source.hpp"

namespace dl {

struct Call final: Node_<NodeID::CALL> {
	NodePtr callee;
	Args args;
    Source src;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Call&>(that);
        return 
            npeq(callee, casted.callee) &&
            args == casted.args &&
            src == casted.src;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << callee << ", " << args << ", " << src;
    }
};

}
