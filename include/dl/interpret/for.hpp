#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct For final: Node_<For> {
    NodePtr vars;
    NodePtr iterable;
    NodePtr body;
    NodePtr orelse;

    For(
        NodePtr&& vars,
        NodePtr&& iterable,
        NodePtr&& body,
        NodePtr&& orelse,
        Span src
    ) noexcept:
    Node_(src),
    vars(std::move(vars)),
    iterable(std::move(iterable)),
    body(std::move(body)),
    orelse(std::move(orelse)) {}

    virtual For copy() const override {
        return For(
            copy_np(vars),
            copy_np(iterable),
            copy_np(body),
            copy_np(orelse),
            src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const For&>(that);
        return
            npeq(vars, casted.vars) &&
            npeq(iterable, casted.iterable) &&
            npeq(body, casted.body) &&
            npeq(orelse, casted.orelse);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::FOR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, vars, iterable, body, orelse);
    }

    virtual Span span() const noexcept override {
        Span end = orelse != nullptr ? orelse->span(): body->span();
        return Span(src, end);
    }
};

}
