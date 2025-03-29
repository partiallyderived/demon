#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct KeywordArgNode final: Node_<KeywordArgNode<KIND>> {
    ID id;
    NodePtr arg;

    KeywordArgNode(ID&& id, NodePtr&& arg, Span src) noexcept:
    Node_<KeywordArgNode<KIND>>(src), id(std::move(id)), arg(std::move(arg)) {}

    virtual KeywordArgNode copy() const override {
        return KeywordArgNode(id.copy(), arg->copy_ptr(), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const KeywordArgNode&>(that);
        return id == casted.id && npeq(arg, casted.arg);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, id, arg);
    }

    virtual Span span() const noexcept override {
        return Span(id.src, arg->span());
    }
};

using KeywordArg = KeywordArgNode<NodeKind::KWARG>;
using MatchKeywordArg = KeywordArgNode<NodeKind::MATCH_KWARG>;

}