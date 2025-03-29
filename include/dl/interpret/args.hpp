#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct ArgsNode final: Node_<ArgsNode<KIND>> {
    Nodes args;
    Nodes kwargs;

    ArgsNode(Nodes&& args, Nodes&& kwargs, Span src) noexcept:
    Node_<ArgsNode<KIND>>(src),
    args(std::move(args)),
    kwargs(std::move(kwargs)) {}

    virtual ArgsNode copy() const override {
        return ArgsNode(deep_copy_ptr(args), deep_copy_ptr(kwargs), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const ArgsNode&>(that);
        return nodes_eq(args, casted.args) && nodes_eq(kwargs, casted.kwargs);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, args, kwargs);
    }

    virtual Span span() const noexcept override {
        return this->src;
    }
};

using Args = ArgsNode<NodeKind::ARGS>;
using MatchArgs = ArgsNode<NodeKind::MATCH_ARGS>;

}
