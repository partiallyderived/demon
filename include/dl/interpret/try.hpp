#pragma once

#include <ostream>

#include "dl/interpret/block.hpp"
#include "dl/interpret/except.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/util.hpp"

namespace dl {

struct Try final: Node_<NodeID::TRY> {
    Block body;
    std::vector<Except> excepts;
    Block finally;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Try&>(that);
        return
            body == casted.body &&
            excepts == casted.excepts &&
            finally == casted.finally;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << body << ", " << out_container(excepts) << ", " << finally;
    }
};

}
