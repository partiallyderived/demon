#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct MatchKeywordArg final: Node {
    ID id;
    NodePtr arg;

    MatchKeywordArg(ID&& id, NodePtr&& arg, Pos src) noexcept:
    Node(src), id(std::move(id)), arg(std::move(arg)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const MatchKeywordArg&>(that);
        return id == casted.id && npeq(arg, casted.arg);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH_KWARG;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, id, arg);
    }
};

}