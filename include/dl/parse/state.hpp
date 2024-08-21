#pragma once

#include <memory>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "dl/file.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/parse/opinfo.hpp"
#include "dl/pos.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/tokenforward.hpp"

namespace dl::syntax {
    struct Operator;
}

namespace dl::parse {

// Context for where an operator was parsed within a statement.
// START is "start of a statement"
// END is "End of a statement". This usually means we have just parsed a token
//   which expects a newline afterwards, such as "break".
// LEFT is "left of value", or more precisely, we are in a part of a statement
//   where it is valid to parse an unconcatenated value.
// RIGHT is "right of value", or more precisely, we are in a part of a statement
//   where it is valid to parse a typical binary operator.
// OPTIONAL is for the uncommon situation that an operator has a optional
//   argument, like "return"

enum class Context {
    START, LEFT, RIGHT, END, OPTIONAL
};

std::ostream& operator<<(std::ostream& os, Context ctx) {
    using enum Context;
    switch(ctx) {
    case START:
        return os << "Start";
    case LEFT:
        return os << "Left";
    case RIGHT:
        return os << "Right";
    case END:
        return os << "End";
    case OPTIONAL:
        return os << "Optional";
    }
}

// Contains all the relevant information for parsing a dl file.
struct State {
    // Cursor used to track our location within a file being lexxed.
    lex::Cursor cursor;

    // Current parsing context.
    Context ctx;

    // Depth of current indentation.
    std::uint32_t depth;

    // Number of end blocks to process (unprocessed dedents).
    std::uint32_t end_blocks_to_process;

    // The operator to process next in case an operator could not immediately be
    // processed.
    const syntax::Operator* next_op;

    // `Located` containing the next token to be processed or `nullptr` if a new
    // token needs to be read.
    syntax::Located next_token;

    // Mapping from `Node` pointers to their corresponding locations within the
    // file being processed.
    std::unordered_map<syntax::Node*, Pos> locations;

    // Stack containing pointers to parsed nodes.
    // Often referred to in comments as "the node stack" or simply "the stack".
    syntax::Nodes stack;

    // Stack containing all pushed operators ("the operator stack").
    std::vector<OpInfo> opstack;

    // Get the info for the operator on top of the operator stack.
    OpInfo& opinfo() noexcept {
        return opstack.back();
    }

    // Get `OpInfo` as a constant reference.
    const OpInfo& opinfo() const noexcept {
        return opstack.back();
    }

    // Get the operator on top of the operator stack.
    const syntax::Pushed* op() noexcept {
        return opinfo().op;
    }

    // Get the corresponding file location for the operator on top of the
    // operator stack.
    Pos loc() const noexcept {
        return opinfo().loc;
    }

    // Determine whether the operator on top of the operator stack is enclosed
    // between brackets.
    bool in_brackets() const noexcept {
        return opinfo().in_brackets;
    }

    // Push an `OpInfo` on top of the operator stack.
    void pushop(OpInfo ctx) {
        opstack.push_back(ctx);
    }

    // Pop an `OpInfo` from the operator stack.
    OpInfo popop() noexcept {
        OpInfo info = opstack.back();
        opstack.pop_back();
        return info;
    }

    // Look up the location of the syntax element corresponding to `node` in the
    // source file.
    Pos loc(syntax::Node* node) const noexcept {
        return locations.at(node);
    }

    // Determines whether the node stack is empty.
    bool empty() const noexcept {
        return stack.empty();
    }

    // Node on top of the stack, which corresponds to the back of a vector.
    syntax::Node* top() const noexcept {
        return stack.back().get();
    }

    syntax::Node* at(std::size_t idx) const noexcept {
        return stack[stack.size() - idx - 1].get();
    }

    // Push a node onto the node stack.
    // Location is the line/column number the corresponding syntax element was
    // parsed at.
    void push(syntax::NodePtr&& node, Pos location) {
        locations[node.get()] = location;
        stack.emplace_back(std::move(node));
    }

    void push(syntax::Node* node, Pos location) {
        push(syntax::NodePtr(node), location);
    }

    void push(syntax::NodePtr&& node) {
        // Use loc as a default location for the node, which works out for the
        // majority of cases.
        push(std::move(node), loc());
    }

    void push(syntax::Node* node) {
        push(syntax::NodePtr(node));
    }

    // Pop a node from the node stack and return it.
    syntax::NodePtr pop() noexcept {
        syntax::NodePtr node = std::move(stack.back());
        stack.pop_back();
        return node;
    }

    State(File* file) noexcept:
    cursor(file),
    ctx(Context::START),
    depth(0),
    end_blocks_to_process(0),
    next_op(nullptr),
    next_token(nullptr),
    locations(),
    stack(),
    opstack() {}
};

}
