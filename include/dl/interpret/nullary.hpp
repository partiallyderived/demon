#pragma once

#include <ostream>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

template<NodeKind KIND>
struct Nullary final: Node_<Nullary<KIND>> {
    Nullary(Span src) noexcept: Node_<Nullary<KIND>>(src) {}

    virtual Nullary copy() const override {
        return Nullary(this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return true;
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os;
    }

    virtual Span span() const noexcept override {
        return this->src;
    }
};

using Break = Nullary<NodeKind::BREAK>;
using Continue = Nullary<NodeKind::CONTINUE>;
using None = Nullary<NodeKind::NONE>;
using Null = Nullary<NodeKind::NULL_>;
using Placeholder = Nullary<NodeKind::PLACEHOLDER>;
using This = Nullary<NodeKind::THIS>;

}
