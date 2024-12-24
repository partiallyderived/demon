#pragma once

#include <ostream>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct KeywordArg final: Node_<NodeID::KEYWORD_ARG> {
    ID id;
    NodePtr arg;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const KeywordArg&>(that);
        return id == casted.id && npeq(arg, casted.arg);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << arg;
    }
};

}