#pragma once

#include <ostream>
#include <utility>

#include "dl/err.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

struct ErrorNode final: Node_<ErrorNode> {
    ErrPtr err;

    ErrorNode(ErrPtr&& err, Span src) noexcept:
    Node_(src), err(std::move(err)) {}

    virtual ErrorNode copy() const override {
        return ErrorNode(err->copy(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return *err == *dynamic_cast<const ErrorNode&>(that).err;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ERROR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << *err;
    }

    virtual Span span() const noexcept override {
        return src;
    }
};

}
