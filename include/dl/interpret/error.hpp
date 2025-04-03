#pragma once

#include <ostream>
#include <utility>

#include "dl/compose/comp.hpp"
#include "dl/err.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

struct ErrorNode final: Node_<ErrorNode> {
    ErrPtr err;
    Comp comp;

    ErrorNode(ErrPtr&& err, Comp&& comp) noexcept:
    Node_(comp.span()), err(std::move(err)), comp(std::move(comp)) {}

    ErrorNode(ErrPtr&& err, Comp&& comp, Span src) noexcept:
    Node_(src), err(std::move(err)), comp(std::move(comp)) {}

    virtual ErrorNode copy() const override {
        return ErrorNode(err->copy(), comp.copy(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return *err == *dynamic_cast<const ErrorNode&>(that).err;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ERROR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, *err, comp);
    }

    virtual Span span() const noexcept override {
        return comp.span();
    }
};

}
