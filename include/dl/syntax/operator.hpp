#pragma once

#include <ostream>

#include "dl/err.hpp"
#include "dl/parse/state.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/precedence.hpp"

namespace dl::syntax {

struct Operator {
    virtual Kind kind() const noexcept = 0;

    // Called when the operator is parsed to modify the parse state accordingly.
    virtual ErrPtr onparse(parse::State& state) const = 0;

    // All `Operator` subclasses are stateless.
    bool operator==(const Operator& that) const noexcept {
        return kind() == that.kind();
    }

    bool operator!=(const Operator& that) const noexcept = default;
};

std::ostream& operator<<(std::ostream& os, const Operator& op) {
    return os << op.kind();
}

struct Pushed;

// "Precedented" operators may or may not "precede" an operator which is on top
// of the operator stack.
// Note that just because an operator is precedented does not mean it may be
// found on the stack itself, since it may be fully handled immediately upon
// having precedence.
struct Precedented: Operator {
    // Precedence of this operator when it appears right of the other operator.
    // Note that a Precedented operator may be handled immediately instead of
    // using the stack and therefore might not appear to the left of another
    // operator for the purposes of a precedence check, so only
    // `right_precedence` is defined for `Precedented`.
    virtual Precedence right_precedence() const noexcept = 0;

    static inline bool precedes(const Pushed* left, const Precedented* right)
    noexcept;

    // Process an operator on top of the operator stack.
    static inline ErrPtr process(parse::State& state);

    // Handles logic for when `this` precedes the operator on top of the
    // operator stack.
    virtual ErrPtr onprecedes(parse::State& state) const {
        return nullptr;
    }

    virtual ErrPtr onparse(parse::State& state) const override {
        while (precedes(state.op(), this)) {
            // The operator on top of the operator stack precedes `this`.
            // Therefore, go ahead and process the operator on top and check
            // again.
            ErrPtr e = process(state);
            if (e)
                return std::move(e);
        }
        // We precede the operator on the stack, so push us on top so we're
        // handled first.
        return onprecedes(state);
    }
};

// "Pushed" operators are pushed to the operator stack when they precede the
// previous top of the operator stack.
struct Pushed: Precedented {
    // Determines what happens when `this` is popped from the operator stack.
    virtual ErrPtr onpop(parse::State& state) const = 0;

    // Precedence of this operator when it appears to the left of the other
    // operator, or in other words, when `this` is on the stack.
    virtual Precedence left_precedence() const noexcept = 0;

    // Get the operator to be associated with `this` when pushed to the operator
    // stack.
    virtual parse::OpInfo info(parse::State& state) const noexcept {
        return parse::OpInfo(
            this,
            state.next_token.loc,
            // If our parent operator was enclosed, we are still enclosed.
            left_bracket() || state.in_brackets()
        );
    }

    // Determines whether this operator is a left bracket operator.
    // Such operators cause whitespace to be handled differently.
    virtual bool left_bracket() const noexcept {
        return false;
    }

    // When `this` precedes the operator on top of the operator stack, it should
    // carry out the `Precedented` logic as well as push itself onto the
    // operator stack.
    virtual ErrPtr onprecedes(parse::State& state) const override {
        // Should push `this` when it precedes operator on top of the node
        // stack.
        state.pushop(info(state));
        return nullptr;
    }
};

bool Precedented::precedes(const Pushed* left, const Precedented* right)
noexcept {
    return left->left_precedence() >= right->right_precedence();
}

ErrPtr Precedented::process(parse::State& state) {
    // Just trigger the `onpop` logic and then pop the top operator.
    ErrPtr e = state.op()->onpop(state);
    state.popop();
    return e;
}

// Convenience class implementing most of Precedented using template parameters.
template<Kind KIND, Precedence RIGHT_PRECEDENCE>
struct Precedented_: Precedented {
    Kind kind() const noexcept final override {
        return KIND;
    }

    Precedence right_precedence() const noexcept final override {
        return RIGHT_PRECEDENCE;
    }
};

// Convenience class implementing most of Pushed using template parameters.
template<
    Kind KIND,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
struct Pushed_: Pushed {
    Kind kind() const noexcept final override {
        return KIND;
    }

    Precedence left_precedence() const noexcept final override {
        return LEFT_PRECEDENCE;
    }

    Precedence right_precedence() const noexcept final override {
        return RIGHT_PRECEDENCE;
    }
};

// Chained operators keep track of the number of duplicates of a particular
// operator which have been pushed rather than handling them as a series of
// applications of a binary operator.
template<
    Kind KIND,
    int INITIAL_COUNT,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
struct ChainedOp: Pushed_<KIND, LEFT_PRECEDENCE, RIGHT_PRECEDENCE> {
    // Modify operator so that it also keeps track of a count of the number of
    // chained nodes.
    virtual parse::OpInfo info(parse::State& state) const noexcept override {
        parse::OpInfo c = Pushed::info(state);
        c.count = INITIAL_COUNT;
        return c;
    }

    virtual ErrPtr onparse(parse::State& state) const override {
        // If the operator on the stack is not the same as `this`, just call
        // the `Precedented` logic.
        if (this->kind() != state.op()->kind())
            return Precedented::onparse(state);
        // Otherwise, increment the operator count and do not push this to the
        // operator stack.
        state.opinfo().count++;
        return nullptr;
    }
};

// Chained collected operator, which means there are other arguments between
// duplicates that need to be aggregated into the resulting node.
template<
    Kind KIND,
    typename NodeType,
    int INITIAL_COUNT,
    Precedence LEFT_PRECEDENCE,
    Precedence RIGHT_PRECEDENCE = LEFT_PRECEDENCE
>
struct CollectedChainedOp:
ChainedOp<KIND, INITIAL_COUNT, LEFT_PRECEDENCE, RIGHT_PRECEDENCE> {
    // Construct and push the resulting node from beginning and end pointers to
    // the chain on the stack.
    virtual ErrPtr onpop(parse::State& state) const override {
        std::size_t beginidx = state.stack.size() - state.opinfo().count;
        syntax::NodePtr* begin = &state.stack[beginidx];
        syntax::NodePtr* end = begin + state.opinfo().count;
        auto res = new NodeType(begin, end);
        state.stack.resize(beginidx);
        state.push(res, state.opinfo().loc);
        return nullptr;
    }
};

// Begin is a special operator marking the beginning of the file.
struct Begin {
    struct Operator final: Pushed_<Kind::BEGIN, Precedence::LEFT_LEFT> {
        // Begin should never be popped.
        ErrPtr onpop(parse::State& state) const override {
            return ErrPtr(new AssertionFailedErr());
        }
    };
    static constexpr auto OPERATOR = Operator();
};

}
