#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/callkind.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Call final: Node {
    CallKind kind;
    NodePtr callee;
    Args args;

    Call(CallKind kind, NodePtr&& callee, Args&& args, Pos src) noexcept: 
    Node(src), kind(kind), callee(std::move(callee)), args(std::move(args)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::CALL;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Call&>(that);
        return
            kind == casted.kind &&
            npeq(callee, casted.callee) &&
            args == casted.args;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << kind << "(" << callee << ", " << args << ")";
    }
};

}
