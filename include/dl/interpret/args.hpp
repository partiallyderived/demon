#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Args {
    Nodes args;
    Nodes kwargs;

    Args(Nodes&& args, Nodes&& kwargs) noexcept:
    args(std::move(args)), kwargs(std::move(kwargs)) {}

    bool operator==(const Args& that) const noexcept {
        return nodes_eq(args, that.args) && nodes_eq(kwargs, that.kwargs);
    }
};

std::ostream& operator<<(std::ostream& os, const Args& x) {
    return os << "Args(" << x.args << ", " << x.kwargs << ")";
}

}
