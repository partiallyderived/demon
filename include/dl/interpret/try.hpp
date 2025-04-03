#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/block.hpp"
#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Try final: Node_<Try> {
    NodePtr body;
    Nodes excepts;
    NodePtr finally;

    Try(
        NodePtr&& body,
        Nodes&& excepts,
        NodePtr&& finally,
        Span src
    )
    noexcept:
    Node_<Try>(src),
    body(std::move(body)),
    excepts(std::move(excepts)),
    finally(std::move(finally)) {}

    virtual Try copy() const override {
        return Try(
            copy_np(body), deep_copy_ptr(excepts), copy_np(finally), this->src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Try&>(that);
        return
            npeq(body, casted.body) &&
            nodes_eq(excepts, casted.excepts) &&
            npeq(finally, casted.finally);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::TRY;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, body, excepts, finally);
    }

    virtual Span span() const noexcept override {
        if (finally != nullptr)
            return Span(this->src, finally->span());
        if (excepts.size() > 0)
            return Span(this->src, excepts.back()->span());
        return body->span();
    }
};

}
