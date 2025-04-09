#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct MatchObject final: Node_<MatchObject> {
    NodePtr type;
    NodePtr args;

    MatchObject(NodePtr&& type, NodePtr&& args, Span src) noexcept:
    Node_(src), type(std::move(type)), args(std::move(args)) {}

    virtual MatchObject copy() const override {
        return MatchObject(copy_np(type), copy_np(args), src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const MatchObject&>(that);
        return npeq(type, casted.type) && npeq(args, casted.args);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH_OBJECT;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, type, args);
    }

    virtual Span span() const noexcept override {
        return Span(type->span(), args->span());
    }
};

}
