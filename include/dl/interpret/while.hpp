#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/block.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct While final: Node_<While> {
    NodePtr predicate;
    Nodes body;
    Block orelse;

    While(
        NodePtr&& predicate, Nodes&& body, Block&& orelse, Span src
    ) noexcept:
    Node_<While>(src),
    predicate(std::move(predicate)),
    body(std::move(body)),
    orelse(std::move(orelse)) {}

    virtual While copy() const override {
        return While(
            predicate->copy_ptr(), deep_copy_ptr(body), orelse.copy(), this->src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const While&>(that);
        return
            npeq(predicate, casted.predicate) &&
            nodes_eq(body, casted.body) &&
            orelse == casted.orelse;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::WHILE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, body, orelse);
    }

    virtual Span span() const noexcept override {
        Span end = orelse.code.size() > 0 ?
            orelse.code.back()->span(): body.back()->span();
        return Span(this->src, end);
    }
};

}
