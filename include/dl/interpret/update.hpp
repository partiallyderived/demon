#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Update final: Node {
    NodePtr object;
    Args args;
    NodePtr value;

    Update(NodePtr&& object, Args&& args, NodePtr&& value, Pos src) noexcept:
    Node(src),
    object(std::move(object)),
    args(std::move(args)),
    value(std::move(value)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Update&>(that);
        return
            npeq(object, casted.object) &&
            args == casted.args &&
            npeq(value, casted.value);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::UPDATE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, object, args, value);
    }
};

}
