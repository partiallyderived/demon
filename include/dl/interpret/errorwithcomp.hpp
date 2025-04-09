#pragma once

#include <ostream>
#include <utility>

#include "dl/compose/comp.hpp"
#include "dl/err.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

struct ErrorWithComp final: Node_<ErrorWithComp> {
    ErrPtr err;
    Comp comp;

    ErrorWithComp(ErrPtr&& err, Comp&& comp) noexcept:
    Node_(comp.span()), err(std::move(err)), comp(std::move(comp)) {}

    ErrorWithComp(ErrPtr&& err, Comp&& comp, Span src) noexcept:
    Node_(src), err(std::move(err)), comp(std::move(comp)) {}

    virtual ErrorWithComp copy() const override {
        return ErrorWithComp(err->copy(), comp.copy(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const ErrorWithComp&>(that);
        return *err == *casted.err && comp == casted.comp;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ERROR_WITH_COMP;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, *err, comp);
    }

    virtual Span span() const noexcept override {
        return comp.span();
    }
};

}
