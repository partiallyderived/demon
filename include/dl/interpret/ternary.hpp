#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Ternary final: Node {
    NodePtr predicate;
    NodePtr if_case;
    NodePtr else_case;

    Ternary(
        NodePtr&& predicate, NodePtr&& if_case, NodePtr&& else_case, Pos src
    )
    noexcept:
    Node(src),
    predicate(std::move(predicate)),
    if_case(std::move(if_case)),
    else_case(std::move(else_case)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::TERNARY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Ternary&>(that);
        return
            npeq(predicate, casted.predicate) &&
            npeq(if_case, casted.if_case) &&
            npeq(else_case, casted.else_case);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            predicate << ", " << if_case << ", " << else_case <<
        ")";
    }
};

}
