#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

#include <queue>
#include <stack>
#include <string>
#include <utility>

#include "dl/parser/opid.hpp"
#include "dl/parser/opkind.hpp"
#include "dl/process/opinfo.hpp"
#include "dl/process/node.hpp"
#include "dl/process/processor.hpp"

namespace dl {

struct ProcessorImpl: Processor {
    std::stack<Op> ops;
    std::stack<Node> nodes;
    std::queue<Node> queue;

    ProcessorImpl(): ops(), nodes(), queue() {}

    void acquire_precedence(OpID op) {
        while (!ops.empty() && has_precedence(ops.top().id, op))
            make();
    }

    Node pop_node() {
        Node node = nodes.top();
        nodes.pop();
        return node;
    }

    Op pop_op() {
        Op op = ops.top();
        ops.pop();
        return op;
    }

    void make() {
        Op op = pop_op();
        const OpInfo& info = opinfo(op.id);
        switch(info.kind) {
        case NULLARY:
            nodes.push(Node(op.id, op.src_id));
            return;
        case UNARY:
            nodes.push(Node(op.id, pop_node(), op.src_id));
            return;
        case BINARY:
            Node rhs = pop_node();
            Node lhs = pop_node();
            nodes.push(Node(op.id, std::move(lhs), std::move(rhs), op.src_id));
            return;
        case default:
            // Other operators should not be found here.
            assert(false);
        }
    }

    void feed(Op op) override {
        using enum OpKind;
        const OpInfo& info = opinfo(op.id);
        if (info.kind == OpKind::SINGLETON) {
            // Singleton values should be pushed immediately without using the
            // op stack.
            nodes.push(Node(op.id, op.src_id));
            return;
        }
        if (info.kind == OpKind::STRING) {
            // Same for string operations (string literals and alphanumerics).
            nodes.push(Node(op.id, std::move(op.content), op.src_id));
            return;
        }
        // For non-singletons, the op stack is used.
        // Keep processing operators with higher precedence until either this op
        // has higher precedence, or there are no more to pop.
        acquire_precedence(op);
        if (op.id == OpID::END) {
            // OpID::END signals the end of a low precedence "start" operator.
            // "Start" operators are like left brackets, while OpID::END is like
            // the corresponding right bracket.
            // Despite there being multiple right brackets, there is only one
            // end operator because the parser should already have verified that
            // the brackets (or other similar elements) match.
            Op start = pop_op();
            if (start.id == OpID::BLOCK) {
                // Block is special in that it aggregates its contents rather
                // than taking its contents as a unary operand.
                // To do so, we must find the node marking the start of the
                // block, which has OpID::WAITING in nodes.
                std::uint32_t i = nodes.size() - 1;
                while (nodes[i].op != OpID::WAITING)
                    i--;
                i++;
                std::vector<Node> dest;
                for (;i < nodes.size(); i++)
                    dest.push_back(std::move(nodes[i]));
                // Pop all the nodes we just pushed, including the mark node,
                // which is why `<=` rather than `<` is used.
                for (i = 0; i <= dest.size(); i++)
                    nodes.pop();
                nodes.push(Node(OpID::BLOCK, dest, start.src_id));
            } else
                // Start operators are essentially unary, taking their entire
                // contents as a single argument.
                nodes.push(Node(start.id, pop_node(), start.src_id));
        }
        else if (op.id == OpID::STMT) {
            // Statement does not get pushed to the operator stack, because it
            // is redundant to wrap a node in a statement.
            if (ops.empty())
                // When we've just parsed a statement separator and the op stack
                // is empty, the statement we just parsed may be executed.
                queue.push(pop_node());
        }
        else {
            if (op.id == OpID::BLOCK)
                // Push a node to mark the start of a block.
                nodes.push(Node(OpID::WAITING, Pos()));
            // Barring singletons and special cases, push the operator to the
            // operator stack.
            // Note that ops with ID OpID::WAITING or OpID::DONE should never be
            // fed to the processor.
            ops.push(std::move(op));
        }
    }

    // Feed a word node, processing it's string representation.
    void feed_word(Located<OpID> op, std::string&& word) override {
        nodes.push(Node(op.obj, std::move(std::move(word)), op.pos));
    }

    Node next() override {
        if (queue.empty())
            return Node(OpID::WAITING, Pos());
        Node node = queue.front();
        queue.pop();
        return node;
    }
};

}
