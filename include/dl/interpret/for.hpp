#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct For final: Node_<For> {
    Nodes vars;
    NodePtr iterable;
    Nodes body;
    Block orelse;

    For(
        Nodes&& vars,
        NodePtr&& iterable,
        Nodes&& body,
        Block&& orelse,
        Span src
    ) noexcept:
    Node_<For>(src),
    vars(std::move(vars)),
    iterable(std::move(iterable)),
    body(std::move(body)),
    orelse(std::move(orelse)) {}

    virtual For copy() const override {
        return For(
            deep_copy_ptr(vars),
            iterable->copy_ptr(),
            deep_copy_ptr(body),
            orelse.copy(),
            this->src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const For&>(that);
        return
            nodes_eq(vars, casted.vars) &&
            npeq(iterable, casted.iterable) &&
            nodes_eq(body, casted.body) &&
            orelse == casted.orelse;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::FOR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, vars, iterable, body, orelse);
    }

    virtual Span span() const noexcept override {
        Span end = orelse.code.size() > 0 ?
            orelse.code.back()->span(): body.back()->span();
        return Span(src, end);
    }
};

}
