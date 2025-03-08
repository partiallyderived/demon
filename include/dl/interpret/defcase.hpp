#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/argspec.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct DefCase final: Node {
    ArgSpec spec;
    NodePtr returns;
    Nodes body;

    DefCase(ArgSpec&& spec, NodePtr&& returns, Nodes&& body, Pos src) noexcept:
    Node(src),
    spec(std::move(spec)),
    returns(std::move(returns)),
    body(std::move(body)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const DefCase&>(that);
        return
            spec == casted.spec &&
            npeq(returns, casted.returns) &&
            nodes_eq(body, casted.body);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::DEF_CASE;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, spec, returns, body);
    }
};

}