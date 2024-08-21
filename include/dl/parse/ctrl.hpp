#pragma once

#include <typeinfo>
#include <utility>

#include "dl/err.hpp"
#include "dl/file.hpp"
#include "dl/lex/lex.hpp"
#include "dl/parse/state.hpp"
#include "dl/pos.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/operator.hpp"
#include "dl/syntax/statement.hpp"
#include "dl/syntax/token.hpp"

namespace dl::parse {

// Determine whether the top node of `state` may be executed based on what the
// following token is.
bool can_execute(State& state) {
    if (state.stack.size() != 1 || state.ctx != Context::START)
        return false;
    if (state.next_token) {
        switch (state.next_token.token->kind()) {
        case syntax::Kind::HASH:
        case syntax::Kind::LEADING_SPACE:
        case syntax::Kind::SPACE:
        case syntax::Kind::NEWLINE:
        case syntax::Kind::NEWLINE_THEN_GLOBAL:
            // These tokens simply proceed on start, we want to check the next
            // token that would actually result in an operator.
            return false;
        default:
            return state.top()->ready(state.next_token.token->kind());
        }
    }
    // Use nullary ready when a token is unavailable.
    return state.top()->ready();
}

void offer(State& state) {
    // If there are two or more nodes, offer top node to second-to-top node.
    if (
        state.size() >= 2 &&
        state.stack[state.stack.size() - 2].offer(std::move(state.top()))
    )
        // Offer accepted, pop node from stack.
        state.stack.pop_back();
}

syntax::NodeRes advance_from_op(State& state, const syntax::Operator* op) {
    offer(state);
    if (can_execute(state)) {

    }
}

// Advance the parsing state by processing the next operator or executing the
// top node of the stack if we are in an appropriate global context.
syntax::NodeRes advance(State& state) {
    if (state.end_blocks_to_process > 0) {
        state.end_blocks_to_process--;
        // Blocks are not accepted as offers to other nodes, so can just return
        // here.
        return syntax::EndBlock::OPERATOR.onparse(state);
    }
    if (state.next_op != nullptr) {
        ErrPtr e = state.next_op->onparse(state);
        state.next_op = nullptr;
        offer(state);
        if (!state.empty() && state.top()-)
        return e;
    }
    if (state.next_token.token == nullptr) {
        syntax::TokenRes res = lex::next(state.cursor);
        if (res.is_err)
            return std::move(res.err);
        state.next_token = std::move(res.res);
        if (can_execute(state)) {
            // Defer processing of token to a later call and pop the top node,
            // which is ready for execution.
            return state.pop();
        }
    }
    syntax::ActionRes res = state.next_token.token->onlex(state);
    if (res.is_err) 
        return std::move(res.err);
    state.ctx = res.res.transition;
    state.next_token.token = nullptr;
    ErrPtr e = res.res.op->onparse(state);
    
    
    return e;
}

// Given a file, construct the initial state with a single operator,
// `syntax::BEGIN`, on the operator stack.
State initial_state(File* file) {
    auto state = State(file);
    state.pushop(OpInfo(&syntax::Begin::OPERATOR, Pos(1, 1), false));
    return state;
}

}
