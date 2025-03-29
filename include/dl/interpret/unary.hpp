#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

template<NodeKind KIND, bool PREFIX=true>
struct Unary final: Node_<Unary<KIND, PREFIX>> {
    NodePtr arg;

    Unary(NodePtr&& arg, Span src) noexcept:
    Node_<Unary<KIND, PREFIX>>(src), arg(std::move(arg)) {}

    virtual Unary copy() const override {
        return Unary(copy_np(arg), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return npeq(arg, dynamic_cast<const Unary&>(that).arg);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << arg;
    }

    virtual Span span() const noexcept override {
        if (arg == nullptr)
            return this->src;
        if constexpr (PREFIX)
            return Span(this->src, arg->span());
        return Span(arg->span(), this->src);
    }
};

using Addr = Unary<NodeKind::ADDR>;
using AddrType = Unary<NodeKind::ADDR_TYPE, false>;
using BitNot = Unary<NodeKind::BNOT>;
using Expansion = Unary<NodeKind::EXPANSION>;
using Lambda = Unary<NodeKind::LAMBDA>;
using Negate = Unary<NodeKind::NEG>;
using Not = Unary<NodeKind::NOT>;
using Raise = Unary<NodeKind::RAISE>;
using Return = Unary<NodeKind::RETURN>;
using Symbol = Unary<NodeKind::SYMBOL>;

}
