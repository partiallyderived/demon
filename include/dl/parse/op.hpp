#pragma once

#include <deque>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "dl/data.hpp"
#include "dl/parse/opid.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Op {
    OpID id;
    Data data;
    Pos src;

    Op(OpID id, Pos src) noexcept: id(id), data(), src(src) {}

    Op(OpID id, Data&& data, Pos src) noexcept:
    id(id), data(std::move(data)), src(src) {}

    bool operator==(const Op& that) const noexcept = default;
};

std::ostream& operator<<(std::ostream& os, const Op& o) {
    os << "Op(" << o.id;
    if (!std::holds_alternative<std::monostate>(o.data))
        os << ", " << o.data;
    return os << ", " << o.src << ')';
}

// Useful for testing, since tokens which result in multiple operators being
// emitted can have results compared against the deque of operators.
std::ostream& operator<<(std::ostream& os, const std::deque<Op>& d) {
    return os << OutContainerManip(d);
}

std::ostream& operator<<(std::ostream& os, const std::vector<Op>& d) {
    return os << OutContainerManip(d);
}

}
