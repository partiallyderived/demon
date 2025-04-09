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
    NodePtr body;

    MatchCase(NodePtr&& matcher, NodePtr&& guard, NodePtr&& body, Span src)
    noexcept:
    Node_(src),
    matcher(std::move(matcher)),
    guard(std::move(guard)),
    body(std::move(body)) {}

    MatchCase copy() const override {
        return MatchCase(
            copy_np(matcher),
            copy_np(guard),
            copy_np(body),
            src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const MatchCase&>(that);
        return
            npeq(matcher, casted.matcher) &&
            npeq(guard, casted.guard) &&
            npeq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH_CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, matcher, guard, body);
    }

    virtual Span span() const noexcept override {
        return Span(src, body->span());
    }
};

}
