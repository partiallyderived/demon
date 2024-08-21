#pragma once

#include <ostream>

#include "dl/err.hpp"
#include "dl/parse/state.hpp"
#include "dl/syntax/operator.hpp"

namespace dl::syntax {

// Represents the action to take as a consequence of parsing a token.
struct Action {
    // Context to transition to.
    parse::Context transition;

    // Operator to process.
    const Operator* op;

    Action(parse::Context transition, const Operator* op) noexcept:
    transition(transition), op(op) {}

    bool operator==(const Action& that) const noexcept {
        if (transition != that.transition)
            return false;
        return
            transition == that.transition &&
            (op == that.op || (op && that.op && *op == *that.op));
    }

    bool operator!=(const Action& that) const noexcept = default;
};

std::ostream& operator<<(std::ostream& os, const Action& action) {
    os << "Action(" << action.transition << ", ";
    if (action.op)
        return os << "&" << *action.op << ")";
    return os << "nullptr)";
}

using ActionRes = Res<Action, ErrPtr>;

}
