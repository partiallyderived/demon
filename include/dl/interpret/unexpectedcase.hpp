#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct UnexpectedCase final: Node_<UnexpectedCase> {
    NodePtr predicate;
    Nodes code;

    UnexpectedCase(NodePtr&& predicate, Nodes&& code, Span src) noexcept:
    Node_(src), predicate(std::move(predicate)), code(std::move(code)) {}

    virtual UnexpectedCase copy() const override {
        return UnexpectedCase(copy_np(predicate), deep_copy_ptr(code), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(code, dynamic_cast<const UnexpectedCase&>(that).code);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::UNEXPECTED_CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, predicate, code);
    }

    virtual Span span() const noexcept override {
        return Span(predicate->span(), code.back()->span());
    }
};

}
