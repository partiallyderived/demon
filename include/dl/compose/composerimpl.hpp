#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "dl/compose/opinfo.hpp"
#include "dl/compose/opkind.hpp"
#include "dl/compose/composer.hpp"
#include "dl/compose/comp.hpp"
#include "dl/parse/opid.hpp"
#include "dl/pos.hpp"
#include "dl/span.hpp"

namespace dl {

struct ComposerImpl: Composer {
    std::vector<Op> ops;
    std::vector<Comp> comps;
    std::queue<Comp> queue;

    ComposerImpl(): ops(), comps(), queue() {}

    void acquire_precedence(OpID op) {
        while (!ops.empty() && has_precedence(ops.back().id, op))
            make();
    }

    Comp pop_comp() {
        Comp comp = std::move(comps.back());
        comps.pop_back();
        return comp;
    }

    Op pop_op() {
        Op op = std::move(ops.back());
        ops.pop_back();
        return op;
    }

    void make() {
        Op op = pop_op();
        const OpInfo& info = opinfo(op.id);

        using enum OpKind;
        switch(info.kind) {
        case NULLARY:
            comps.push_back(Comp(op.id, op.src));
            return;
        case UNARY:
            comps.push_back(Comp(op.id, pop_comp(), op.src));
            return;
        case BINARY: {
            Comp rhs = pop_comp();
            Comp lhs = pop_comp();
            comps.push_back(
                Comp(op.id, std::move(lhs), std::move(rhs), op.src)
            );
            return;
        }
        default:
            // Other operators should not be found here.
            assert(false);
        }
    }

    void feed(Op&& op) override {
        using enum OpKind;
        const OpInfo& info = opinfo(op.id);
        if (info.kind == OpKind::SINGLETON) {
            // Singleton values should be pushed immediately without using the
            // op stack.
            comps.push_back(Comp(op.id, op.src));
            return;
        }
        if (info.kind == OpKind::DATA) {
            // Same for literals and identifiers.
            comps.push_back(Comp(op.id, std::move(op.data), op.src));
            return;
        }
        // For non-singletons, the op stack is used.
        // Keep processing operators with higher precedence until either this op
        // has higher precedence, or there are no more to pop.
        acquire_precedence(op.id);
        if (op.id == OpID::END) {
            // OpID::END signals the end of a low precedence "start" operator.
            // "Start" operators are like left brackets, while OpID::END is like
            // the corresponding right bracket.
            // Despite there being multiple right brackets, there is only one
            // end operator because the parser should already have verified that
            // the brackets (or other similar elements) match.
            Op start = pop_op();
            if (start.id == OpID::BLOCK || start.id == OpID::CONSTRUCT) {
                // Block and construct are special in that they aggregate their
                // contents rather than taking their contents as a unary
                // operand.
                // To do so, we must find the comp marking the start of the
                // block or construct, indicated by OpID::WAITING.
                std::uint32_t i = comps.size() - 1;
                while (comps[i].op != OpID::WAITING)
                    i--;
                i++;
                std::vector<Comp> dest;
                for (;i < comps.size(); i++)
                    dest.push_back(std::move(comps[i]));
                // Pop all the comps we just pushed, including the mark comp,
                // which is why <= rather than < is used.
                for (i = 0; i <= dest.size(); i++)
                    comps.pop_back();
                comps.push_back(
                    Comp(start.id, std::move(dest), Span(start.src, op.src))
                );
            } else
                // Start operators are essentially unary, taking their entire
                // contents as a single argument.
                comps.push_back(
                    Comp(start.id, pop_comp(), Span(start.src, op.src))
                );
        }
        else if (op.id == OpID::STMT) {
            // Statement does not get pushed to the operator stack, because it
            // is redundant to wrap a comp in a statement.
            if (ops.empty())
                // When we've just parsed a statement separator and the op stack
                // is empty, the statement we just parsed may be executed.
                queue.push(pop_comp());
        }
        else {
            if (op.id == OpID::BLOCK || op.id == OpID::CONSTRUCT)
                // Push a comp to mark the start of a block or construct.
                comps.push_back(Comp(OpID::WAITING, op.src));
            // Barring singletons and special cases, push the operator to the
            // operator stack.
            // Note that ops with ID OpID::WAITING or OpID::DONE should never be
            // fed to the composer.
            ops.push_back(std::move(op));
        }
    }

    Comp next() override {
        if (queue.empty())
            return Comp(OpID::WAITING, Span(Pos()));
        Comp comp = std::move(queue.front());
        queue.pop();
        return comp;
    }
};

}
