#pragma once

#include <ostream>

#include <string>

#include "dl/err.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"

namespace dl {

struct ID final: Node_<NodeID::ID> {
    std::string id;

    virtual bool equals(const Node& that) const noexcept override {
        return id == dynamic_cast<const ID&>(that).id;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id;
    }
};

}
