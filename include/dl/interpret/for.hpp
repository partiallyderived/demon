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
    NodePtr iterable;
    Nodes cases;
    NodePtr orelse;

    For(
        NodePtr&& iterable,
        Nodes&& cases,
        NodePtr&& orelse,
        Span src
    ) noexcept:
    Node_(src),
    iterable(std::move(iterable)),
    cases(std::move(cases)),
    orelse(std::move(orelse)) {}

    virtual For copy() const override {
        return For(
            copy_np(iterable),
            deep_copy_ptr(cases),
            copy_np(orelse),
            src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const For&>(that);
        return
            npeq(iterable, casted.iterable) &&
            nodes_eq(cases, casted.cases) &&
            npeq(orelse, casted.orelse);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::FOR;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, iterable, cases, orelse);
    }

    virtual Span span() const noexcept override {
        Span end = orelse != nullptr ? orelse->span(): cases.back()->span();
        return Span(src, end);
    }
};

}
