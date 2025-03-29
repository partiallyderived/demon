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
    std::vector<Case> cases;
    Block orelse;

    If(std::vector<Case>&& cases, Block&& orelse, Span src) noexcept:
    Node_<If>(src), cases(std::move(cases)), orelse(std::move(orelse)) {}

    virtual If copy() const override {
        return If(deep_copy(cases), orelse.copy(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const If&>(that);
        return cases == casted.cases && orelse == casted.orelse;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::IF;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << OutContainerManip(cases) << ", " << orelse;
    }

    virtual Span span() const noexcept override {
        Span end = orelse.code.size() > 0 ?
            orelse.code.back()->span(): cases.back().span();
        return Span(cases.front().span(), end);
    }
};

}
