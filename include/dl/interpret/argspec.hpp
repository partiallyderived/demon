#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct ArgSpec final: Node {
    std::vector<ArgDef> args;
    std::vector<ArgDef> kwargs;
    ArgDef var_args;
    ArgDef var_kwargs;

    ArgSpec(
        std::vector<ArgDef>&& args,
        std::vector<ArgDef>&& kwargs,
        ArgDef&& var_args,
        ArgDef&& var_kwargs,
        Pos src
    ) noexcept:
    Node(src),
    args(std::move(args)),
    kwargs(std::move(kwargs)),
    var_args(std::move(var_args)),
    var_kwargs(std::move(var_kwargs)) {}

    bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const ArgSpec&>(that);
        return
            args == casted.args &&
            kwargs == casted.kwargs &&
            var_args == casted.var_args &&
            var_kwargs == casted.var_kwargs;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ARG_SPEC;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os <<
            OutContainerManip(args) << ", " <<
            OutContainerManip(kwargs) << ", " <<
            var_args << ", " <<
            var_kwargs;
    }
};

}
