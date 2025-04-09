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
    Nodes cases;

    Def(NodePtr&& id, Nodes&& cases, Span src) noexcept:
    Node_(src),
    id(std::move(id)),
    cases(std::move(cases)) {}

    virtual Def copy() const override {
        return Def(copy_np(id), deep_copy_ptr(cases), this->src);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::DEF;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Def&>(that);
        return npeq(id, casted.id) && nodes_eq(cases, casted.cases);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, id, cases);
    }

    virtual Span span() const noexcept override {
        return Span(cases.front()->span(), cases.back()->span());
    }
};

}
