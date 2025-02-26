#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/interpret/seqkind.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Seq final: Node {
    SeqKind kind;
    Nodes nodes;

    Seq(SeqKind kind, Nodes&& nodes, Pos src) noexcept: 
    Node(src), kind(kind), nodes(std::move(nodes)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::SEQ;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Seq&>(that);
        return kind == casted.kind && nodes_eq(nodes, casted.nodes);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << kind << OutContainerManip(nodes);
    }
};

}
