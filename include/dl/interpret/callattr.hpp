#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"

namespace dl {

template<NodeKind KIND>
struct CallAttrNode final: Node {
    NodePtr object;
    NodePtr attr;
    Args args;

    CallAttrNode(NodePtr&& object, NodePtr&& attr, Args&& args, Pos src)
    noexcept:
    Node(src),
    object(std::move(object)),
    attr(std::move(attr)),
    args(std::move(args))
    {}

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const CallAttrNode&>(that);
        return
            npeq(object, casted.object) &&
            npeq(attr, casted.attr) &&
            args == casted.args;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, object, attr, args);
    }
};

using CallAttr = CallAttrNode<NodeKind::CALL_ATTR>;
using CachedCallAttr = CallAttrNode<NodeKind::CACHED_CALL_ATTR>;

}
