#pragma once

#include <cstdint>

#include <ostream>

#include "dl/err.hpp"
#include "dl/parse/state.hpp"
#include "dl/syntax/bracket.hpp"
#include "dl/syntax/err.hpp"
#include "dl/syntax/kind.hpp"
#include "dl/syntax/statement.hpp"
#include "dl/syntax/token.hpp"

namespace dl::syntax {

// Indicates that an indent which was not a multiple of four spaces was found.
struct IncompleteIndentErr final: SyntaxErr {
    using SyntaxErr::SyntaxErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "IncompleteIndentErr";
    }
};

// Indicates that more space than the current block's indentation have been
// found in a context that does not permit an indent.
struct OverIndentErr final: SyntaxErr {
    using SyntaxErr::SyntaxErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "OverIndentErr";
    }
};

// Indicates that the file has ended while a left bracket remains unclosed.
struct UnclosedBracketErr final: SyntaxErr {
    using SyntaxErr::SyntaxErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedBracketErr";
    }
};

// Indicates that less space than the current block's indentation have been
// found in context that does not permit a dedent.
struct UnderIndentErr final: SyntaxErr {
    using SyntaxErr::SyntaxErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnderIndentErr";
    }
};

struct UnexpectedEOFErr final: SyntaxErr {
    using SyntaxErr::SyntaxErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedEOFErr";
    }
};

// Space not at the beginning of a line.
// Separate from `LeadingSpace` to avoid unnecessary dynamic memory allocation,
// since space is quite common.

struct Space {
    struct Token final: Token_<Kind::SPACE> {
        ActionRes onstart(parse::State& state) override {
            // Should have been lexxed as `LeadingSpace` instead.
            return ErrPtr(new AssertionFailedErr());
        }

        ActionRes onleft(parse::State& state) override {
            return Action(parse::Context::LEFT, nullptr);
        }

        ActionRes onright(parse::State& state) override {
            return Action(parse::Context::RIGHT, nullptr);
        }

        ActionRes onoptional(parse::State& state) noexcept override {
            return Action(parse::Context::OPTIONAL, nullptr);
        }

        ActionRes onend(parse::State& state) noexcept override {
            return Action(parse::Context::END, nullptr);
        }
    };

    static Token TOKEN;
};

Space::Token Space::TOKEN = Token();

struct LeadingSpace {
    struct Token final: Token_<Kind::LEADING_SPACE> {
        std::uint32_t n;

        constexpr Token(std::uint32_t n) noexcept: n(n) {}

        bool operator==(const syntax::Token& that) const noexcept override {
            return this->kind() == that.kind() &&
            this->n == dynamic_cast<const Token&>(that).n;
        }

        bool has_state() const noexcept override {
            return true;
        }

        std::ostream& out(std::ostream& os) const override {
            return os << "LeadingSpace(" << n << ")";
        }

        ActionRes onstart(parse::State& state) override {
            std::uint32_t indents = n / 4;
            std::uint32_t leftover = n % 4;
            if (leftover)
                // Incomplete indent occurs just after all previous indents,
                // i.e. at indents * 4 + 1.
                return ErrPtr(
                    new IncompleteIndentErr(
                        Pos(state.next_token.loc.line, indents * 4 + 1)
                    )
                );
            if (indents > state.depth)
                // Deepening indentation only permitted for `LEFT`.
                return ErrPtr(new OverIndentErr(
                    Pos(state.next_token.loc.line, state.depth * 4 + 1)
                ));
            if (indents == state.depth)
                return Action(parse::Context::START, nullptr);
            // We now know `state.depth >= indents`

            // Action only permits returning one operator, the remaining dedents
            // must be processed later.
            state.end_blocks_to_process = state.depth - indents - 1;
            state.depth = indents;
            return Action(parse::Context::START, &EndBlock::OPERATOR);
        }

        ActionRes onleft(parse::State& state) override {
            std::uint32_t indents = n / 4;
            std::uint32_t leftover = n % 4;
            if (indents < state.depth)
                // Can only dedent in `onstart`.
                return ErrPtr(new UnderIndentErr(
                    Pos(state.next_token.loc.line, n + 1)
                ));
            if (
                indents == state.depth + 1 &&
                !leftover &&
                !state.in_brackets() && (
                    state.op()->kind() == Kind::EXPR ||
                    state.op()->kind() == Kind::LABEL
                )
            ) {
                // 1. State is in the left context.
                // 2. # Indents is exactly one more without any extra space
                // 3. Indent was not found between brackets.
                // 4. Operator on stack is a colon operator.
                // These are the conditions for increasing the depth of
                // indentation.
                state.depth++;
                return Action(parse::Context::START, &Block::OPERATOR);
            }
            return Action(parse::Context::LEFT, nullptr);
        }

        ActionRes onright(parse::State& state) override {
            std::uint32_t indents = n / 4;
            if (indents < state.depth)
                // Can only dedent in `onstart`.
                return ErrPtr(new UnderIndentErr(
                    Pos(state.next_token.loc.line, n + 1)
                ));
            // Over-indenting is fine when not at start of statement.
            return Action(parse::Context::RIGHT, nullptr);
        }

        ActionRes onoptional(parse::State& state) override {
            // A newline in an optional context transitions to `START`, so this
            // is impossible.
            return ErrPtr(new AssertionFailedErr());
        }

        ActionRes onend(parse::State& state) override {
            // Can't be leading space at the end.
            return ErrPtr(new AssertionFailedErr());
        }
    };
};


template<Kind KIND>
struct NewlineBase: Token {
    Kind kind() const noexcept final override {
        return KIND;
    }

    virtual ActionRes onstart(parse::State& state) noexcept override {
        return Action(parse::Context::START, nullptr);
    }

    // No `noexcept` here so `NewlineThenGlobal` can allocate an error.
    virtual ActionRes onleft(parse::State& state) override {
        return Action(parse::Context::LEFT, nullptr);
    }

    virtual ActionRes onright(parse::State& state) noexcept override {
        if (state.in_brackets())
            // If we're enclosed between brackets, just proceed.
            return Action(parse::Context::RIGHT, nullptr);
        // Otherwise, newline acts as a statement separator.
        return Action(parse::Context::START, &Statements::OPERATOR);
    }

    virtual ActionRes onoptional(parse::State& state) noexcept override {
        if (state.in_brackets())
            // If we're enclosed between brackets, just proceed.
            return Action(parse::Context::OPTIONAL, nullptr);
        return Action(parse::Context::START, &Statements::OPERATOR);
    }

    virtual ActionRes onend(parse::State& state) noexcept override {
        return Action(parse::Context::START, &Statements::OPERATOR);
    }
};

struct Newline {
    struct Token final: NewlineBase<Kind::NEWLINE> {};

    static Token TOKEN;
};

Newline::Token Newline::TOKEN = Token();

// `NewlineThenGlobal` is a newline that is not followed by space, EOF, or a
// newline.
// This token is necessary in order to process dedents not indicated by leading
// space, which is absent in this case.
struct NewlineThenGlobal {
    struct Token final: NewlineBase<Kind::NEWLINE_THEN_GLOBAL> {
        ActionRes onlex(parse::State& state) override {
            // On contexts other than `LEFT`, a full dedent additionally occurs.
            if (state.ctx != parse::Context::LEFT) {
                // Note that the following two lines do nothing if `state.depth`
                // is 0, assuming new tokens are not processed while
                // `end_blocks_to_process` is nonzero.
                state.end_blocks_to_process = state.depth;
                state.depth = 0;
            }
            return syntax::Token::onlex(state);
        }

        ActionRes onleft(parse::State& state) override {
            if (state.depth != 0) {
                // Unexpected dedent when a value was expected.
                return ErrPtr(
                    // The error occurs on the next line.
                    new UnderIndentErr(Pos(state.next_token.loc.line + 1, 1))
                );
            }
            return Action(parse::Context::LEFT, nullptr);
        }
    };

    static Token TOKEN;
};

NewlineThenGlobal::Token NewlineThenGlobal::TOKEN = Token();

struct EndOfFile {
    // End of file is similar enough to a newline to warrant being in the space
    // file.
    struct Token final: Token_<Kind::END_OF_FILE> {
        ActionRes onstart(parse::State& state) noexcept override {
            return Action(parse::Context::START, nullptr);
        }

        ActionRes onleft(parse::State& state) override {
            return ErrPtr(new UnexpectedEOFErr(state.next_token.loc));
        }

        ActionRes onright(parse::State& state) override {
            // Very similar to newline in this case.
            if (state.in_brackets())
                return ErrPtr(new UnclosedBracketErr(state.next_token.loc));
            return Action(parse::Context::START, &Statements::OPERATOR);
        }

        ActionRes onoptional(parse::State& state) override {
            return onright(state);
        }

        ActionRes onend(parse::State& state) override {
            return onright(state);
        }
    };

    static Token TOKEN;
};

EndOfFile::Token EndOfFile::TOKEN = Token();

}
