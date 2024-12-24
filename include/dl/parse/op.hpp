#pragma once

#include <deque>
#include <string>
#include <utility>
#include <variant>

#include "dl/data.hpp"
#include "dl/parse/opid.hpp"
#include "dl/source.hpp"
#include "dl/util.hpp"

namespace dl {

struct Op {
    OpID id;
    Data data;
    Source src;

    Op(OpID id, const Source& src) noexcept:
    id(id), data(), src(src) {}

    Op(OpID id, Data&& data, const Source& src) noexcept:
    id(id), data(std::move(data)), src(src) {}

    bool operator==(const Op& that) const noexcept = default;
};

std::ostream& operator<<(std::ostream& os, const Op& o) {
    os << "Op(" << o.id;
    if (!std::holds_alternative<std::monostate>(o.data))
        os << ", " << o.data;
    return os << ", " << src << ')';
}

// Useful for testing, since tokens which result in multiple operators being
// emitted can be have results compared against the deque of operators.
std::ostream& operator<<(std::ostream& os, const std::deque<Op>& d) {
    return out_container(os, d);
}

}
