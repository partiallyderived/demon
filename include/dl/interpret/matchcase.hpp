#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct MatchCase final: Node {
    NodePtr matcher;
    NodePtr guard;
    Nodes body;

    MatchCase(NodePtr&& matcher, NodePtr&& guard, Nodes&& body, Pos src)
    noexcept:
    Node(src),
    matcher(std::move(matcher)),
    guard(std::move(guard)),
    body(std::move(body)) {}

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
};

}
