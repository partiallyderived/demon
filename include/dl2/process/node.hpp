#pragma once

#include <cstdint>

#include <string>
#include <utility>
#include <vector>

#include "dl/parse/opid.hpp"
#include "dl/process/opinfo.hpp"

namespace dl {

struct BinaryData;

struct Node {
    OpID op;
    union {
        Node* node;
        BinaryData* bin;
        std::string str;
        std::vector<Node> nodes;
    };
    std::uint64_t src_id;

    Node(const Node& that) = delete;

    Node(Node&& that) noexcept: op(that.op), src_id(that.src_id) {
        switch(opinfo(that.op).kind) {
        case UNARY:
            new(&node)(that.node);
            that.node = nullptr;
            return;
        case BINARY:
            new(&bin)(that.bin);
            that.bin = nullptr;
            return;
        case WORD:
            new(&str)(std::move(that.str));
            return;
        case BLOCK:
            new(&nodes)(std::move(that.nodes));
            return;
        }
    }

    // Nullary constructor
    Node(OpID op, std::uint64_t src_id) noexcept: op(op), src_id(src_id) {}

    // Unary constructor
    Node(OpID op, Node&& node, std::uint64_t src_id):
    op(op), node(new Node(std::move(node))), src_id(src_id) {}

    // Binary constructor
    Node(OpID op, Node&& lhs, Node&& rhs, std::uint64_t src_id);

    // String constructor
    Node(OpID op, std::string&& str, std::uint64_t src_id):
    op(op), str(std::move(str)), src_id(src_id) {}

    // Block constructor
    Node(OpID op, std::vector<Node>&& nodes, std::uint64_t src_id):
    op(op), nodes(std::move(nodes)), src_id(src_id) {}

    ~Node() noexcept {
        switch(opinfo(op)) {
        case UNARY:
            delete node;
            return;
        case BINARY:
            delete bin;
            return;
        case WORD:
            word.~string();
            return;
        case BLOCK:
            nodes.~vector();
            return;
    }
};

struct BinaryData {
    Node lhs;
    Node rhs;

    BinaryData(Node&& lhs, Node&& rhs) noexcept:
    lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

Node::Node(OpID op, Node&& lhs, Node&& rhs, std::uint64_t src_id):
op(op), bin(new BinaryData(std::move(lhs), std::move(rhs))), src_id(src_id) {}

}
