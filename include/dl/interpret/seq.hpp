#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct Seq final: Node_<Seq<KIND>> {
    Nodes nodes;

    Seq(Nodes&& nodes, Span src) noexcept:
    Node_<Seq<KIND>>(src), nodes(std::move(nodes)) {}

    virtual Seq copy() const override {
        return Seq(deep_copy_ptr(nodes), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return nodes_eq(nodes, dynamic_cast<const Seq&>(that).nodes);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << nodes;
    }

    virtual Span span() const noexcept override {
        return this->src;
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
