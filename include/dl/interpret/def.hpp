#pragma once

#include <ostream>
#include <vector>

#include "dl/interpret/defcase.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/util.hpp"

namespace dl {

struct Def final: Node_<NodeID::DEF> {
	std::vector<DefCase> cases;
	ID id;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const Def&>(that);
        return cases == casted.cases && id == casted.id;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
    	return out_container(os, cases) << ", " << id;
    }
};

}
