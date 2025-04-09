#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Match final: Node_<Match> {
    NodePtr matchee;
    Nodes cases;

    Match(NodePtr&& matchee, Nodes&& cases, Span src) noexcept:
    Node_(src),
    matchee(std::move(matchee)),
    cases(std::move(cases)) {}

    virtual Match copy() const override {
        return Match(copy_np(matchee), deep_copy_ptr(cases), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Match&>(that);
        return npeq(matchee, casted.matchee) && nodes_eq(cases, casted.cases);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, matchee, cases);
    }

    virtual Span span() const noexcept override {
        return Span(src, cases.back()->span());
    }
};

}
