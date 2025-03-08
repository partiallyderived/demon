#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Args final: Node {
    Nodes args;
    Nodes kwargs;

    Args(Nodes&& args, Nodes&& kwargs, Pos src) noexcept:
    Node(src), args(std::move(args)), kwargs(std::move(kwargs)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Args&>(that);
        return nodes_eq(args, casted.args) && nodes_eq(kwargs, casted.kwargs);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ARGS;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, args, kwargs);
    }
};

}
