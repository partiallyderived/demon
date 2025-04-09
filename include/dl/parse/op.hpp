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
    Span src;

    Op(const Op& that) = delete;

    Op(Op&& that) noexcept:
    id(that.id), data(std::move(that.data)), src(that.src) {}

    Op(OpID id, Span src) noexcept: id(id), data(), src(src) {}

    Op(OpID id, Data&& data, Span src) noexcept:
    id(id), data(std::move(data)), src(src) {}

    bool operator==(const Op& that) const noexcept {
        if (src != that.src)
            return false;
        if (id == OpID::ERROR && that.id == OpID::ERROR)
            return *std::get<ErrPtr>(data) == *std::get<ErrPtr>(that.data);
        return id == that.id && data == that.data;
    }

    Op copy() const noexcept {
        return Op(id, copy_data(data), src);
    }
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
