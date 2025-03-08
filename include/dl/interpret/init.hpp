#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Init final: Node {
    NodePtr attr;
    NodePtr type;
    NodePtr val;

    Init(NodePtr&& attr, NodePtr&& type, NodePtr&& val, Pos src) noexcept:
    Node(src),
    attr(std::move(attr)),
    type(std::move(type)),
    val(std::move(val)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Init&>(that);
        return
            npeq(attr, casted.attr) &&
            npeq(type, casted.type) &&
            npeq(val, casted.val);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::INIT;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, attr, type, val);
    }
};

}
