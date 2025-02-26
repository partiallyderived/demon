#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Case {
    NodePtr predicate;
    Nodes body;

    Case(NodePtr&& predicate, Nodes&& body) noexcept:
    predicate(std::move(predicate)), body(std::move(body)) {}

    bool operator==(const Case& that) const noexcept {
        return npeq(predicate, that.predicate) && nodes_eq(body, that.body);
    }
};

std::ostream& operator<<(std::ostream& os, const Case& x) {
    return os << "Case(" << x.predicate << ", " << x.body << ")";
}

}
