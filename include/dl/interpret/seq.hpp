#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct Seq final: Node {
    Nodes nodes;

    Seq(Nodes&& nodes, Pos src) noexcept: 
    Node(src), nodes(std::move(nodes)) {}

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(nodes, dynamic_cast<const Seq&>(that).nodes);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << OutContainerManip(nodes);
    }
};

using List = Seq<NodeKind::LIST>;
using Map = Seq<NodeKind::MAP>;
using MatchList = Seq<NodeKind::MATCH_LIST>;
using MatchMap = Seq<NodeKind::MATCH_MAP>;
using MatchOptions = Seq<NodeKind::MATCH_OPTIONS>;
using MatchTuple = Seq<NodeKind::MATCH_TUPLE>;
using Set = Seq<NodeKind::SET>;
using Tuple = Seq<NodeKind::TUPLE>;

}
