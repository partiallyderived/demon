#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct MatchCase final: Node_<MatchCase> {
    NodePtr matcher;
    NodePtr guard;
    Nodes body;

    MatchCase(NodePtr&& matcher, NodePtr&& guard, Nodes&& body, Span src)
    noexcept:
    Node_<MatchCase>(src),
    matcher(std::move(matcher)),
    guard(std::move(guard)),
    body(std::move(body)) {}

    MatchCase copy() const override {
        return MatchCase(
            matcher->copy_ptr(),
            copy_np(guard),
            deep_copy_ptr(body),
            this->src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const MatchCase&>(that);
        return
            npeq(matcher, casted.matcher) &&
            npeq(guard, casted.guard) &&
            nodes_eq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH_CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, matcher, guard, body);
    }

    virtual Span span() const noexcept override {
        return Span(this->src, body.back()->span());
    }
};

}
