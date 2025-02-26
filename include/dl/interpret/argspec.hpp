#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/node.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct ArgSpec {
    std::vector<ArgDef> args;
    std::vector<ArgDef> kwargs;
    ArgDef var_args;
    ArgDef var_kwargs;

    ArgSpec(
        std::vector<ArgDef>&& args,
        std::vector<ArgDef>&& kwargs,
        ArgDef&& var_args,
        ArgDef&& var_kwargs
    ) noexcept:
    args(std::move(args)),
    kwargs(std::move(kwargs)),
    var_args(std::move(var_args)),
    var_kwargs(std::move(var_kwargs)) {}

    bool operator==(const ArgSpec& that) const noexcept {
        return
            args == that.args &&
            kwargs == that.kwargs &&
            var_args == that.var_args &&
            var_kwargs == that.var_kwargs;
    }
};

std::ostream& operator<<(std::ostream& os, const ArgSpec& x) {
    return os << "ArgSpec(" <<
        OutContainerManip(x.args) << ", " <<
        OutContainerManip(x.kwargs) << ", " <<
        x.var_args << ", " <<
        x.var_kwargs <<
    ")";
}

}
