#pragma once

#include <iostream>
#include <memory>
#include <ostream>

#include "dl/err.hpp"
#include "dl/parse/state.hpp"
#include "dl/syntax/action.hpp"
#include "dl/syntax/err.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/tokenforward.hpp"

namespace dl::syntax {

// Indicates that a token was not expected, like when a binary operator is found
// where a value should be.
struct UnexpectedTokenErr final: SyntaxErr {
    // The unexpected token.
    const Token* token;

    UnexpectedTokenErr(const Token* token, Pos loc) noexcept:
    SyntaxErr(loc), token(token) {}

    inline bool equals(const Err& that) const noexcept override;

    inline std::ostream& out_data(std::ostream& os) const override;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedTokenErr";
    }
};

struct Token {
    virtual Kind kind() const noexcept = 0;

    // Tells What should be done if this operator is found when state.ctx is
    // LEFT.
    virtual ActionRes onleft(parse::State& state) = 0;

    // Tells what should be done if this operator is found when state.ctx is
    // RIGHT.
    virtual ActionRes onright(parse::State& state) = 0;

    virtual bool operator==(const Token& that) const noexcept {
        return this->kind() == that.kind();
    }

    bool operator!=(const Token& that) const noexcept = default;

    // Tells whether this has a state. Used by TokenPtr to determine whether a
    // token needed to be dynamically allocated and therefore deleted.
    virtual bool has_state() const noexcept {
        return false;
    }

    // Tells what should be done if this operator is found when state.ctx is
    // START.
    virtual ActionRes onstart(parse::State& state) {
        // Typically logic for `onstart` is the same as for `onleft`.
        return this->onleft(state);
    }

    // Tells what should be done if this operator is found when state.ctx is
    // END.
    virtual ActionRes onend(parse::State& state) {
        // Only whitespace and comments are allowed on end, so return an error
        // can be used as a common default.
        return ErrPtr(new UnexpectedTokenErr(this, state.next_token.loc));
    }

    // Tells what should be done if this operator is found when `state.ctx` is
    // OPTIONAL.
    virtual ActionRes onoptional(parse::State& state) {
        // Typically logic for `onoptional` is the same for `onleft`.
        return this->onleft(state);
    }

    virtual ActionRes onlex(parse::State& state) {
        switch (state.ctx) {
        case parse::Context::START:
            return this->onstart(state);
        case parse::Context::LEFT:
            return this->onleft(state);
        case parse::Context::RIGHT:
            return this->onright(state);
        case parse::Context::OPTIONAL:
            return this->onoptional(state);
        case parse::Context::END:
            return this->onend(state);
        }
    }

    virtual std::ostream& out(std::ostream& os) const {
        return os << "Token[" << this->kind() << "]";
    }

    virtual ~Token() noexcept {}
};

// Convenience class with token kind as a template parameter.
template<Kind KIND>
struct Token_: Token {
    Kind kind() const noexcept final override {
        return KIND;
    }
};

std::ostream& operator<<(std::ostream& os, const Token& token) {
    return token.out(os);
}

// As is natural for a virtual class, tokens with state must be dynamically
// allocated.
// However, most tokens do not have a state, and a singleton is used rather than
// dynamic allocation.
// The custom deleter given here will delete a token only if it is stateful.
void TokenDeleter::operator()(Token* x) const noexcept {
    if (x && x->has_state())
        delete x;
};

bool Located::operator==(const Located& that) const noexcept {
    return loc == that.loc && token == that.token || (
        token && that.token && *token == *that.token
    );
}

std::ostream& operator<<(std::ostream& os, const Located& x) {
    os << "Located(";
    if (x.token == nullptr)
        os << "nullptr";
    else
        os << "&" << *x.token;
    return os << ", " << x.loc << ")";
}

using TokenRes = Res<Located, ErrPtr>;

bool UnexpectedTokenErr::equals(const Err& that) const noexcept {
    return
        LocatedErr::equals(that) &&
        *token == (*dynamic_cast<const UnexpectedTokenErr&>(that).token);
}

std::ostream& UnexpectedTokenErr::out_data(std::ostream& os) const {
    return LocatedErr::out_data(os) << ", &" << *token;
}

}
