#pragma once

#include <ostream>
#include <vector>

#include "dl/interpret/id.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodeid.hpp"
#include "dl/interpret/varargdef.hpp"
#include "dl/util.hpp"

namespace dl {

struct ArgSpec final: Node_<NodeID::ARG_SPEC> {
    std::vector<ArgDef> args;
    std::vector<ArgDef> kwargs;
    VarArgsDef var_args;
    VarKwargsDef var_kwargs;

    virtual bool equals(const Node& that) const noexcept override {
        auto casted = dynamic_cast<const ArgSpec&>(that);
        return
            args == casted.args &&
            kwargs == casted.kwargs &&
            var_args == casted.var_args &&
            var_kwargs == casted.var_kwargs;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os
            << out_container(args) << ", "
            << out_container(kwargs) << ", "
            << var_args << ", "
            << var_kwargs;
    }
};

}
