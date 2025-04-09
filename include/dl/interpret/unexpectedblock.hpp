#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct UnexpectedBlock final: Node_<UnexpectedBlock> {
    Nodes code;

    UnexpectedBlock(Nodes&& code, Span src) noexcept:
    Node_(src), code(std::move(code)) {}

    virtual UnexpectedBlock copy() const override {
        return UnexpectedBlock(deep_copy_ptr(code), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(code, dynamic_cast<const UnexpectedBlock&>(that).code);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::UNEXPECTED_BLOCK;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << code;
    }

    virtual Span span() const noexcept override {
        return Span(code.front()->span(), code.back()->span());
    }
};

}
