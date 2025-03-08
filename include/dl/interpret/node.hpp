#pragma once

#include <cstdint>

#include <memory>
#include <ostream>

#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"
#include "dl/util.hpp"

namespace dl {

struct Node {
    Pos src;

    Node(Pos src) noexcept: src(src) {}

    virtual ~Node() noexcept {}

    virtual bool equals(const Node& that) const noexcept=0;

    virtual NodeKind kind() const noexcept=0;

    virtual std::ostream& out_data(std::ostream& os) const=0;

    bool operator==(const Node& that) const noexcept {
        return kind() == that.kind() && src == that.src && equals(that);
    }
};

using NodePtr = std::unique_ptr<Node>;
using Nodes = std::vector<NodePtr>;
using NodeRes = Res<NodePtr>;

// npeq means "Node Pointer Equals"
bool npeq(const NodePtr& lhs, const NodePtr& rhs) noexcept {
    if (lhs)
        return rhs && *lhs == *rhs;
    return !rhs;
}

// Compare vectors of NodePtr
bool nodes_eq(const Nodes& lhs, const Nodes& rhs) noexcept {
    if (lhs.size() != rhs.size())
        return false;
    for (std::uint32_t i = 0; i < lhs.size(); i++) {
        if (!npeq(lhs[i], rhs[i]))
            return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << node.kind() << "(";
    return node.out_data(os) << ")@" << node.src;
}

std::ostream& operator<<(std::ostream& os, const NodePtr& node_ptr) {
    if (node_ptr)
        return os << *node_ptr;
    return os << "null";
}

std::ostream& operator<<(std::ostream& os, const Nodes& nodes) {
    return os << OutContainerManip(nodes);
}

}
