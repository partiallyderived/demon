#pragma once

#include <cstdint>

#include <memory>
#include <ostream>

#include "dl/interpret/nodekind.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

struct Node {
    Span src;

    Node(const Node& that) = delete;

    Node(Node&& that) = default;

    Node(Span src) noexcept: src(src) {}

    virtual ~Node() noexcept {}

    Node& operator=(Node&& that) = default;

    virtual bool operator==(const Node& that) const noexcept {
        return kind() == that.kind() && src == that.src && equals(that);
    }

    virtual bool equals(const Node& that) const noexcept=0;

    virtual NodeKind kind() const noexcept=0;

    virtual std::ostream& out_data(std::ostream& os) const=0;

    virtual Span span() const noexcept=0;

    virtual std::unique_ptr<Node> copy_ptr() const=0;
};

template<typename ThisType>
struct Node_: Node {
    Node_(Span src) noexcept: Node(src) {}

    virtual ThisType copy() const=0;

    virtual std::unique_ptr<Node> copy_ptr() const override {
        return std::unique_ptr<Node>(new ThisType(copy()));
    }
};

using NodePtr = std::unique_ptr<Node>;
using NodeRes = Res<NodePtr>;
using Nodes = std::vector<NodePtr>;

NodePtr copy_np(const NodePtr& x) {
    if (x)
        return x->copy_ptr();
    return nullptr;
}

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
