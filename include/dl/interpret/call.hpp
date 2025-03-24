#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct CallNode final: Node {
    NodePtr callee;
    Args args;

    CallNode(NodePtr&& callee, Args&& args, Pos src) noexcept: 
    Node(src), callee(std::move(callee)), args(std::move(args)) {}

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const CallNode&>(that);
        return
            npeq(callee, casted.callee) &&
            args == casted.args;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, callee, args);
    }
};

using Call = CallNode<NodeKind::CALL>;
using CachedCall = CallNode<NodeKind::CACHED_CALL>;

}
