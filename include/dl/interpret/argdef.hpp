#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct ArgDef final: Node {
    ID id;
    NodePtr label;
    NodePtr dflt;
    bool match;

    ArgDef() noexcept:
    Node(Pos(0, 0)),
    id("", Pos(0, 0)),
    label(nullptr),
    dflt(nullptr),
    match(false) {}

    ArgDef(
        ID&& id, NodePtr&& label, NodePtr&& dflt, bool match, Pos src
    ) noexcept:
    Node(src),
    id(std::move(id)),
    label(std::move(label)),
    dflt(std::move(dflt)),
    match(match) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const ArgDef&>(that);
        return
            id == casted.id &&
            npeq(label, casted.label) &&
            npeq(dflt, casted.dflt) &&
            match == casted.match;
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::ARG_DEF;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, id, label, dflt, match);
    }
};

}
