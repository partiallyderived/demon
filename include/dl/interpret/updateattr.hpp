#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct UpdateAttr final: Node {
    NodePtr object;
    NodePtr attr;
    Args args;
    NodePtr value;

    UpdateAttr(
        NodePtr&& object, NodePtr&& attr, Args&& args, NodePtr&& value, Pos src
    ) noexcept:
    Node(src),
    object(std::move(object)),
    attr(std::move(attr)),
    args(std::move(args)),
    value(std::move(value)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const UpdateAttr&>(that);
        return
            npeq(object, casted.object) &&
            npeq(attr, casted.attr) &&
            args == casted.args &&
            npeq(value, casted.value);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::UPDATE_ATTR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, object, attr, args, value);
    }
};

}
