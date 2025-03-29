#pragma once

#include <ostream>
#include <utility>
#include <vector>

#include "dl/interpret/data.hpp"
#include "dl/interpret/defcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Def final: Node_<Def> {
    NodePtr id;
    std::vector<DefCase> cases;

    Def(NodePtr&& id, std::vector<DefCase>&& cases, Span src) noexcept:
    Node_<Def>(src),
    id(std::move(id)),
    cases(std::move(cases)) {}

    virtual Def copy() const override {
        return Def(id->copy_ptr(), deep_copy(cases), this->src);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::DEF;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Def&>(that);
        return npeq(id, casted.id) && cases == casted.cases;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << id << ", " << OutContainerManip(cases);
    }

    virtual Span span() const noexcept override {
        return Span(cases.front().span(), cases.back().span());
    }
};

}
