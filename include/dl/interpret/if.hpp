#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/case.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct If final: Node_<If> {
    Nodes cases;
    NodePtr orelse;

    If(Nodes&& cases, NodePtr&& orelse, Span src) noexcept:
    Node_(src), cases(std::move(cases)), orelse(std::move(orelse)) {}

    virtual If copy() const override {
        return If(deep_copy_ptr(cases), copy_np(orelse), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const If&>(that);
        return nodes_eq(cases, casted.cases) && npeq(orelse, casted.orelse);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::IF;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << OutContainerManip(cases) << ", " << orelse;
    }

    virtual Span span() const noexcept override {
        Span end = orelse != nullptr ? orelse->span(): cases.back()->span();
        return Span(cases.front()->span(), end);
    }
};

}
