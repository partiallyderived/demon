#pragma once

#include <cstdint>

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct Counted final: Node {

    std::uint32_t count;
    NodePtr arg;

    Counted(std::uint32_t count, NodePtr&& arg, Pos src) noexcept:
    Node(src), count(count), arg(std::move(arg)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Counted&>(that);
        return count == casted.count && npeq(arg, casted.arg);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, count, arg);
    }
};

using LambdaVar = Counted<NodeKind::LAMBDA_VAR>;
using Var = Counted<NodeKind::VAR>;

}
