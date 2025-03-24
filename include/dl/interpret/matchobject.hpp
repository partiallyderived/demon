#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct MatchObject final: Node {
    NodePtr type;
    MatchArgs args;

    MatchObject(NodePtr&& type, MatchArgs&& args, Pos src) noexcept:
    Node(src), type(std::move(type)), args(std::move(args)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const MatchObject&>(that);
        return npeq(type, casted.type) && args == casted.args;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::MATCH_OBJECT;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, type, args);
    }
};

}
