#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Ternary final: Node_<Ternary> {
    NodePtr predicate;
    NodePtr if_case;
    NodePtr else_case;

    Ternary(
        NodePtr&& predicate, NodePtr&& if_case, NodePtr&& else_case, Span src
    ) noexcept:
    Node_(src),
    predicate(std::move(predicate)),
    if_case(std::move(if_case)),
    else_case(std::move(else_case)) {}

    virtual Ternary copy() const override {
        return Ternary(
            copy_np(predicate),
            copy_np(if_case),
            copy_np(else_case),
            src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Ternary&>(that);
        return
            npeq(predicate, casted.predicate) &&
            npeq(if_case, casted.if_case) &&
            npeq(else_case, casted.else_case);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::TERNARY;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, if_case, else_case);
    }

    virtual Span span() const noexcept override {
        return Span(if_case->span(), else_case->span());
    }
};

}
