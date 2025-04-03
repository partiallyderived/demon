#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Block final: Node_<Block> {
    Nodes code;

    Block(Nodes&& code, Span src) noexcept:
    Node_<Block>(src), code(std::move(code)) {}

    virtual Block copy() const override {
        return Block(deep_copy_ptr(code), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Block&>(that);
        return nodes_eq(code, casted.code);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::BLOCK;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << code;
    }

    virtual Span span() const noexcept override {
        return Span(this->src);
    }
};

}
