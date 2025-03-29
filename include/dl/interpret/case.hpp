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
    Nodes body;

    Case(NodePtr&& predicate, Nodes&& body, Span src) noexcept:
    Node_<Case>(src),
    predicate(std::move(predicate)),
    body(std::move(body)) {}

    virtual Case copy() const override {
        return Case(predicate->copy_ptr(), deep_copy_ptr(body), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Case&>(that);
        return 
            npeq(predicate, casted.predicate) &&
            nodes_eq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, body);
    }

    virtual Span span() const noexcept override {
        return Span(this->src, body.back()->span());
    }
};

}
