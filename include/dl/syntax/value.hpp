#pragma once

#include <ostream>
#include <string>

#include "dl/syntax/binary.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/node.hpp"
#include "dl/syntax/nullary.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

// "Values" correspond to elements which are treated as though they are values,
// like "true", "this", literals, and identifiers.
// `OPERATOR` is in reference to the operator that should be used in case this
// token is concatenated to another value.
template<Kind KIND, auto OPERATOR>
struct ValueToken: Token_<KIND> {
    virtual ActionRes onleft(parse::State& state) noexcept override {
        return Action(parse::Context::RIGHT, nullptr);
    }

    virtual ActionRes onright(parse::State& state) noexcept override {
        return Action(parse::Context::RIGHT, OPERATOR);
    }
};

// false, true, this, and vars are singleton tokens, i.e. stateless tokens that
// refer to a value.
template<Kind KIND, auto OPERATOR, typename NodeType>
struct SingletonToken: ValueToken<KIND, OPERATOR> {
    virtual ActionRes onlex(parse::State& state) override {
        state.push(new NodeType());
        return Token::onlex(state);
    }
};

template<Kind KIND, auto OPERATOR>
struct SingletonPack {
    struct Node final: NullaryNode<KIND> {};

    struct Token final: SingletonToken<KIND, OPERATOR, Node> {};
    static Token TOKEN;
};

template<Kind KIND, auto OPERATOR>
typename SingletonPack<KIND, OPERATOR>::Token
SingletonPack<KIND, OPERATOR>::TOKEN = Token();

using False = SingletonPack<Kind::FALSE, &Suffix::OPERATOR>;
using This  = SingletonPack<Kind::THIS , &Suffix::OPERATOR>;
using True  = SingletonPack<Kind::TRUE , &Suffix::OPERATOR>;
using Vars  = SingletonPack<Kind::VARS , &Suffix::OPERATOR>;

template<Kind KIND>
struct WordNode: Node_<KIND> {
    std::string word;

    WordNode(std::string&& word) noexcept: word(std::move(word)) {}

    virtual bool operator==(const Node& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        return word == dynamic_cast<const WordNode*>(&that)->word;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << KIND << "(" << word << ")";
    }
};

// "Words" correspond to identifiers and literals, which is to say, tokens of
// variable length.
template<Kind KIND, auto OPERATOR, typename NodeType>
struct WordToken: ValueToken<KIND, OPERATOR> {
    std::string word;

    WordToken(std::string&& word) noexcept: word(std::move(word)) {}

    virtual bool operator==(const Token& that) const noexcept override {
        if (this->kind() != that.kind())
            return false;
        return word == dynamic_cast<const WordToken*>(&that)->word;
    }

    virtual ActionRes onlex(parse::State& state) override {
        // Push a node containing this token's contents upon being lexed.
        // After onlex is executed, this token is no longer valid and should not
        // be used.
        state.push(new NodeType(std::move(word)));
        return Token::onlex(state);
    }

    bool has_state() const noexcept final override {
        return true;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << this->kind() << "(" << word << ")";
    }
};

template<Kind KIND, auto OPERATOR>
struct WordPack {
    struct Node final: WordNode<KIND> {
        using WordNode<KIND>::WordNode;
    };

    struct Token final: WordToken<KIND, OPERATOR, Node> {
        using WordToken<KIND, OPERATOR, Node>::WordToken;
    };
};

using String = WordPack<Kind::STRING, &Concat::OPERATOR>;
using ID     = WordPack<Kind::ID    , &Suffix::OPERATOR>;
using Int    = WordPack<Kind::INT   , &Suffix::OPERATOR>;
using Mixed  = WordPack<Kind::MIXED , &Suffix::OPERATOR>;

}
