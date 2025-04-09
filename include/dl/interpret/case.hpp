#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Case final: Node_<Case> {
    NodePtr predicate;
    NodePtr body;

    Case(NodePtr&& predicate, NodePtr&& body, Span src) noexcept:
    Node_(src),
    predicate(std::move(predicate)),
    body(std::move(body)) {}

    virtual Case copy() const override {
        return Case(copy_np(predicate), copy_np(body), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Case&>(that);
        return npeq(predicate, casted.predicate) && npeq(body, casted.body);
    }

    bool exists() const noexcept {
        return predicate != nullptr || body != nullptr;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, body);
    }

    virtual Span span() const noexcept override {
        if (body)
            return Span(src, body->span());
        if (predicate)
            return Span(src, predicate->span());
        return src;
    }
};

}
