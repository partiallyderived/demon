#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct CallNode final: Node_<CallNode<KIND>> {
    NodePtr callee;
    Args args;

    CallNode(NodePtr&& callee, Args&& args, Span src) noexcept: 
    Node_<CallNode<KIND>>(src), callee(std::move(callee)), args(std::move(args))
    {}

    virtual CallNode copy() const override {
        return CallNode(callee->copy_ptr(), args.copy(), this->src);
    }

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

    virtual Span span() const noexcept override {
        return Span(callee->span(), args.span());
    }
};

using Call = CallNode<NodeKind::CALL>;
using CachedCall = CallNode<NodeKind::CACHED_CALL>;

}
