#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/argspec.hpp"
#include "dl/interpret/node.hpp"
#include "dl/pos.hpp"

namespace dl {

struct DefCase {
    ArgSpec spec;
    NodePtr returns;
    Nodes body;

    DefCase(ArgSpec&& spec, NodePtr&& returns, Nodes&& body) noexcept:
    spec(std::move(spec)),
    returns(std::move(returns)),
    body(std::move(body)) {}

    bool operator==(const DefCase& that) const noexcept {
        return
            spec == that.spec &&
            npeq(returns, that.returns) &&
            nodes_eq(body, that.body);
    }
};

std::ostream& operator<<(std::ostream& os, const DefCase& x) {
    return os <<
        "DefCase(" << x.spec << ", " << x.returns << ", " << x.body << ")";
}

}