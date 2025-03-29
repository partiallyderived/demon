#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct DefCase final: Node_<DefCase> {
    MatchArgs spec;
    NodePtr guard;
    NodePtr returns;
    Nodes body;

    DefCase(
        MatchArgs&& spec,
        NodePtr&& guard,
        NodePtr&& returns,
        Nodes&& body,
        Span src
    ) noexcept:
    Node_<DefCase>(src),
    spec(std::move(spec)),
    guard(std::move(guard)),
    returns(std::move(returns)),
    body(std::move(body)) {}

    virtual DefCase copy() const override {
        return DefCase(
            spec.copy(),
            copy_np(guard),
            copy_np(returns),
            deep_copy_ptr(body),
            this->src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const DefCase&>(that);
        return
            spec == casted.spec &&
            npeq(guard, casted.guard) &&
            npeq(returns, casted.returns) &&
            nodes_eq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::DEF_CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, spec, guard, returns, body);
    }

    virtual Span span() const noexcept override {
        return Span(src, body.back()->span());
    }
};

}