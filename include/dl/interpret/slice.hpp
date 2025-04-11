#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Slice final: Node_<Slice> {
    NodePtr start;
    NodePtr end;
    NodePtr step;

    Slice(
        NodePtr&& start, NodePtr&& end, NodePtr&& step, Span src
    ) noexcept:
    Node_(src),
    start(std::move(start)),
    end(std::move(end)),
    step(std::move(step)) {}

    virtual Slice copy() const override {
        return Slice(
            copy_np(start),
            copy_np(end),
            copy_np(step),
            src
        );
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Slice&>(that);
        return
            npeq(start, casted.start) &&
            npeq(end, casted.end) &&
            npeq(step, casted.step);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::SLICE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, start, end, step);
    }

    virtual Span span() const noexcept override {
        return src;
    }
};

}
