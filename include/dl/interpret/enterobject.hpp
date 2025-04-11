#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct EnterObject final: Node_<EnterObject> {
    NodePtr predicate;
    Nodes code;

    EnterObject(NodePtr&& predicate, Nodes&& code, Span src) noexcept:
    Node_(src), predicate(std::move(predicate)), code(std::move(code)) {}

    virtual EnterObject copy() const override {
        return EnterObject(copy_np(predicate), deep_copy_ptr(code), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const EnterObject&>(that);
        return npeq(predicate, casted.predicate) && nodes_eq(code, casted.code);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ENTER_OBJECT;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, code);
    }

    virtual Span span() const noexcept override {
        return Span(predicate->span(), code.back()->span());
    }
};

}
