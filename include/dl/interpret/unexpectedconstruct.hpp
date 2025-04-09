#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/parse/opid.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct UnexpectedConstruct final: Node_<UnexpectedConstruct> {
    OpID op;
    NodePtr content;

    UnexpectedConstruct(OpID op, NodePtr&& content, Span src) noexcept:
    Node_(src), op(op), content(std::move(content)) {}

    virtual UnexpectedConstruct copy() const override {
        return UnexpectedConstruct(op, copy_np(content), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const UnexpectedConstruct&>(that);
        return op == casted.op && npeq(content, casted.content);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::UNEXPECTED_CONSTRUCT;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, op, content);
    }

    virtual Span span() const noexcept override {
        return Span(src, content->span());
    }
};

}
