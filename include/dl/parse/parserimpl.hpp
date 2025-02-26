#pragma once

#include <cstdint>

#include <deque>
#include <ostream>
#include <utility>
#include <variant>
#include <vector>

#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/parse/context.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/orientation.hpp"
#include "dl/parse/parser.hpp"
#include "dl/parse/tokeninfo.hpp"
#include "dl/parse/tokenkind.hpp"
#include "dl/pos.hpp"

namespace dl {

// Indicates that an indent was expected but not found.
struct ExpectedIndentErr final: SourcedErr {
    ExpectedIndentErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIndentErr";
    }
};

// Indicates that an indent which was not a multiple of four spaces was found.
struct IncompleteIndentErr final: SourcedErr {
    IncompleteIndentErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "IncompleteIndentErr";
    }
};

// Indicates that more space than the current block's indentation have been
// found in a context that does not permit an indent.
struct OverIndentErr final: SourcedErr {
    OverIndentErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "OverIndentErr";
    }
};

// Indicates that the file has ended while a left bracket remains unclosed.
struct UnclosedBracketErr final: SourcedErr {
    UnclosedBracketErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedBracketErr";
    }
};

// Indicates that less space than the current block's indentation have been
// found in context that does not permit a dedent.
struct UnderIndentErr final: SourcedErr {
    UnderIndentErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnderIndentErr";
    }
};

// Indicates that a particular token was unexpected.
struct UnexpectedTokenErr final: SourcedErr {
    TokenID token;

    UnexpectedTokenErr(TokenID token, Pos src) noexcept:
    token(token), SourcedErr(src) {}

    bool equals(const Err& that) const noexcept override {
        return SourcedErr::equals(that) &&
        token == dynamic_cast<const UnexpectedTokenErr&>(that).token;
    }

    std::ostream& out_data(std::ostream& os) const override {
        return os << token << ", " << src;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedTokenErr";
    }
};

struct ParserImpl: Parser {
    // Depth of indentation.
    std::uint32_t depth;

    // Indicates whether we are at the start of a line.
    bool line_start;

    // Orientation ("state") of the parser.
    Orientation orientation;

    // Queue containing the parsed operations.
    std::deque<Op> queue;

    // Stack containing contexts to be popped when appropriate conditions are
    // met.
    // Contexts include blocks, brackets, and constructs.
    std::vector<Context> contexts;

    // When true, we are parsing a predicate and the colon operator has a
    // different meaning when used outside of brackets.
    bool parsing_pred;

    // When true, the previous token was a construct component which expects
    // no predicate (try, else, etc.)
    bool prev_was_no_pred_construct;

    // When true, we are parsing loop vars and should use a higher-precedence
    // version of separation (comma) operator.
    bool parsing_loop_vars;

    // ID of previous token. Useful when corresponding operator is currently
    // ambiguous.
    TokenID prev;

    // Pos for the previous token.
    Pos prev_src;

    ParserImpl():
    depth(0),
    line_start(true),
    orientation(Orientation::START),
    queue(),
    contexts(),
    parsing_pred(false),
    prev_was_no_pred_construct(false),
    parsing_loop_vars(false),
    // TokenID::SPACE is just a meaningless placeholder until prev is set to
    // a token.
    // Note that this is the only case in which prev will be set to space.
    prev(TokenID::SPACE),
    prev_src() {}

    // Determines whether a token continues the parsing of a construct in the
    // outermost block in which that construct is being parsed, such as `elif`
    // or `else`, but not `if`, which would initiate a new construct.
    static bool continues_construct(TokenID token) noexcept {
        using enum TokenKind;
        switch(tokeninfo(token).kind) {
        case CONSTRUCT_MIDDLE:
        case CONSTRUCT_LAST:
        case CONSTRUCT_LAST_OR_BINARY:
            return true;
        default:
            return false;
        }
    }

    // Convenience functions for pushing operators.
    // Return values are to make code more concise.

    ErrPtr pushop(OpID id, Pos src) {
        queue.push_back(Op(id, src));
        return nullptr;
    }
    
    ErrPtr pushop1(TokenID token, Pos src) {
    	return pushop(tokeninfo(token).op1, src);
    }

    ErrPtr pushop2(TokenID token, Pos src) {
        return pushop(tokeninfo(token).op2, src);
    }

    // Check whether there is at least one context and that the top context is
    // `ctx`.
    bool ctx_is(Context ctx) const noexcept {
        return !contexts.empty() && contexts.back() == ctx;
    }

    // Check if the incoming token implies that we are finished parsing a
    // construct.
    void check_if_construct_ends(const Token& token, Pos src) {
        if (
            orientation == Orientation::START &&
            ctx_is(Context::CONSTRUCT) &&
            !continues_construct(token.id)
        ) {
            contexts.pop_back();
            queue.push_back(Op(OpID::END, src));
            queue.push_back(Op(OpID::STMT, src));
        }
    }

    // Determine whether we are currently between brackets.
    bool in_brackets() const noexcept {
        return ctx_is(Context::CURVED) || ctx_is(Context::SQUARE);
    }

    // Handle dedenting the specified number of times.
    ErrPtr handle_dedents(std::uint32_t dedents, Pos src) {
        for (std::uint32_t i = 0; i < dedents; i++) {
            Context ctx = contexts.back();
            if (ctx == Context::CONSTRUCT || ctx == Context::CONSTRUCT_END) {
                // Dedent implicitly closes a construct.
                contexts.pop_back();
                queue.push_back(Op(OpID::END, src));

                // Construct considered a single statement.
                queue.push_back(Op(OpID::STMT, src));
                ctx = contexts.back();
            }
            // There can be at most one unpushed construct at the end of
            // each block so there is no need to check for another.
            if (ctx != Context::BLOCK)
                // Can't be in brackets because we are in START orientation.
                // Therefore, something is amiss.
                return ErrPtr(new AssertionFailedErr("Context was not BLOCK"));
            contexts.pop_back();
            depth--;

            // Push an `END` to the queue to signal end of a block.
            queue.push_back(Op(OpID::END, src));
            
            // Additionally, every block is implicitly treated as a statement
            // in the enclosing block.
            queue.push_back(Op(OpID::STMT, src));
        }

        // Additionally, if CONSTRUCT_END is on the context stack after
        // dedenting, pop that context too.
        if (ctx_is(Context::CONSTRUCT_END)) {
            contexts.pop_back();
            queue.push_back(Op(OpID::END, src));

            // Construct considered a single statement.
            queue.push_back(Op(OpID::STMT, src));
        }
        return nullptr;
    }

    ErrPtr on_leading_space_start(std::uint32_t count, Pos src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (leftover)
            // Additional space before the start of a statement is not allowed.
            return ErrPtr(new IncompleteIndentErr(src));
        if (indents > depth)
            return ErrPtr(new OverIndentErr(src));
        if (indents == depth)
            // No indent/dedent, just return and keep orientation.
            return nullptr;
        // We now know `indents < depth`, so we're dedenting.
        return handle_dedents(depth - indents, src);
    }

    ErrPtr on_leading_space_stmt(std::uint32_t count, Pos src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (leftover)
            return ErrPtr(new IncompleteIndentErr(src));
        if (indents < depth)
            return ErrPtr(new UnderIndentErr(src));
        if (indents > depth + 1)
            return ErrPtr(new OverIndentErr(src));
        if (indents == depth)
            // An indent is expected when in STMT orientation with leading space.
            return ErrPtr(new ExpectedIndentErr(src));
        // We now know indents == depth + 1
        // When the number of indents is exactly one more than depth
        // without extra space and the state is in the STMT orientation,
        // an indent occurs.
        depth++;
        orientation = Orientation::START;
        contexts.push_back(Context::BLOCK);
        queue.push_back(Op(OpID::BLOCK, src));
        return nullptr;
    }

    ErrPtr on_leading_space_before(std::uint32_t count, Pos src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (indents < depth)
            // Can only dedent in `START`.
            return ErrPtr(new UnderIndentErr(src));
        return nullptr;
    }

    ErrPtr on_leading_space_after(std::uint32_t count, Pos src) {
        std::uint32_t indents = count / 4;
        if (indents < depth)
            // Can only dedent in `START` orientation.
            return ErrPtr(new UnderIndentErr(src));
        return nullptr;
    }

    ErrPtr on_leading_space(std::uint32_t count, Pos src) {
        // Note: since this is the start of a newline, we cannot be in the
        // `OPTIONAL` or `END` orientations. This is because we must either
        // be at the start of the file, or directly after a newline. We
        // start the file in the `START` orientation, and a newline
        // reorients `OPTIONAL` and `END`.
        // Check indents/dedents.
        line_start = false;

        using enum Orientation;
        switch (orientation) {
        case START:
            return on_leading_space_start(count, src);
        case STMT:
            return on_leading_space_stmt(count, src);
        case BEFORE:
            return on_leading_space_before(count, src);
        case AFTER:
        case AFTER_STAR:
            return on_leading_space_after(count, src);
        default:
            return ErrPtr(new AssertionFailedErr("Impossible orientation"));
        }
    }

    void infer_expansion() {
        // In `AFTER_STAR` orientation, infer as unary expansion operator.
        orientation = Orientation::BEFORE;
        pushop(OpID::UNPACK_ARGS, prev_src);
    }

    void infer_arg_sep() {
        // In `AFTER_STAR` orientation, infer as separator between positional
        // and keyword arguments.
        orientation = Orientation::AFTER;
        pushop(OpID::POS_KW_SEP, prev_src);
    }

    ErrPtr on_nullary(TokenID token, Pos src) {
        orientation = Orientation::END;
        return pushop1(token, src);
    }

    ErrPtr on_unary(TokenID token, Pos src) {
        if (token == TokenID::COLON && prev_was_no_pred_construct) {
            // Special case: colon just after a construct which has no
            // predicate moves to statement orientation.
            orientation = Orientation::STMT;
            parsing_pred = false;
            return pushop(OpID::BODY, src);
        }
        orientation = Orientation::BEFORE;
        return pushop1(token, src);
    }

    ErrPtr on_binary(TokenID token, Pos src) {
        // Expecting a value now.
        orientation = Orientation::BEFORE;

        if (token == TokenID::COLON && parsing_pred && !in_brackets()) {
            // Special case: colon as binary operator in a predicate moves to
            // statement orientation.
            orientation = Orientation::STMT;
            parsing_pred = false;
            return pushop(OpID::LABEL, src);
        }

        if (token == TokenID::COMMA && parsing_loop_vars)
            // Special case: comma has higher precedence when parsing loop
            // variables.
            return pushop(OpID::LOOP_VAR_SEP, src);

        if (token == TokenID::EQUALS && in_brackets())
            // Special case: equals changes meaning and precedence inside
            // brackets.
            return pushop(OpID::BIND, src);
        return pushop2(token, src);
    }

    ErrPtr on_postfix(TokenID token, Pos src) {
        orientation = Orientation::AFTER;
        return pushop2(token, src);
    }

    ErrPtr on_value_before(Token&& token, Pos src) {
        // op1 is for values and unary operators.
        orientation = Orientation::AFTER;
        queue.push_back(
            Op(tokeninfo(token.id).op1, std::move(token.data), src)
        );
        return nullptr;
    }

    ErrPtr on_value_after(Token&& token, Pos src) {
        // When an immediate appears directly after another value, this case is
        // treated as though there is an "invisible" operator between them.
        pushop2(token.id, src);
        // Push the value in either case.
        return on_value_before(std::move(token), src);
    }

    ErrPtr on_optional_stmt(TokenID token, Pos src) {
        orientation = Orientation::OPTIONAL;
        return pushop1(token, src);
    }

    ErrPtr on_left_before(TokenID token, Pos src) {
        contexts.push_back(tokeninfo(token).match);
        // A parenthetical or similar is "value-like", so we push `op1`.
        pushop1(token, src);
        orientation = Orientation::BEFORE;
        return nullptr;
    }

    ErrPtr on_left_after(TokenID token, Pos src) {
        // When a left bracket directly follows a value, this case is
        // treated as though there is an "invisible" binary operator between
        // them.
        pushop2(token, src);

        // In any case, the value operator is still pushed.
        return on_left_before(token, src);
    }

    ErrPtr on_right_before(TokenID token, Pos src) {
        // A RIGHT is only valid in BEFORE orientation if the previous token was
        // it's matching left token (i.e. empty parentheses).
        if (
            tokeninfo(prev).kind != TokenKind::LEFT ||
            tokeninfo(prev).match != tokeninfo(token).match
        )
            return ErrPtr(new UnexpectedTokenErr(token, src));
        contexts.pop_back();
        // Push OpID::NOTHING to indicate lack of a value.
        queue.push_back(Op(OpID::NOTHING, src));
        queue.push_back(Op(OpID::END, src));
        orientation = Orientation::AFTER;
        return nullptr;
    }

    ErrPtr on_right_after(TokenID token, Pos src) {
        if (!ctx_is(tokeninfo(token).match))
            return ErrPtr(new UnexpectedTokenErr(token, src));
        // Orientation stays `AFTER` after finding a right bracket.
        contexts.pop_back();
        queue.push_back(Op(OpID::END, src));
        return nullptr;
    }

    ErrPtr on_construct_first(TokenID token, Pos src) {
        // `CONSTRUCT_FIRST` is for the first token in a construct, like `if` or
        // `for`, but not `elif` or `else`.
        // Constructs are handled by imagining there is a hidden "construct"
        // operator which groups different blocks in a construct like `if`, 
        // `elif`, and `else`.
        // Consider the following code:
        //
        // if (cond1):
        //    body1
        // elif (cond2):
        //    body2
        // else:
        //    body3
        //
        // At the AST level, we want to read this code as the following:
        //
        // construct(if(cond1, body1), elif(cond2, body2), else(body3))
        //
        // This hidden construct has a presence on `contexts` using
        // `Context::CONSTRUCT` to indicate that we are parsing a construct.
        contexts.push_back(Context::CONSTRUCT);
        // Expecting a value now.
        orientation = Orientation::BEFORE;
        queue.push_back(Op(OpID::CONSTRUCT, src));

        // Just after parsing a "for", need to signal that loop variables are
        // about to be parsed.
        if (token == TokenID::FOR)
            parsing_loop_vars = true;
        parsing_pred = true;
        prev_was_no_pred_construct = token == TokenID::TRY;
        return pushop1(token, src);
    }

    ErrPtr on_construct_middle(TokenID token, Pos src) {
        // Here's a question: when do we know we're finished parsing a
        // construct?
        // Well, if we just parsed an `if`, we could already be done, or there
        // could be an `elif` or an `else.`
        // The only way to know is to keep reading.
        // Here, `elif` satisfies the "grammatical" role as a
        // "construct middle", meaning it indicates code that is part of a
        // construct, but does not initiate the construct, and it's presence
        // does not imply we are done parsing the construct afterwords as in the
        // case of `else`.
        // It is treated as valid in the parsing stage if and only if we are in
        // the `START` orientation with `Context::CONSTRUCT` on top of
        // `contexts`.
        // This is true even if it is not actually valid within the context of
        // the particular construct being parsed, like a `case` label in an `if`
        // construct or `except` in a `for` construct.
        // This choice was made to simplify the parser by reducing tokens to
        // grammatical roles, where they are treated as valid even if
        // nonsensical, like "but" in the sentence "I had a good day, but I was
        // happy."
        // The responsibility of checking whether it is valid in the particular
        // construct is passed onto the executor.
        if (!ctx_is(Context::CONSTRUCT))
            return ErrPtr(new UnexpectedTokenErr(token, src));

        // Expecting a value now.
        orientation = Orientation::BEFORE;
        parsing_pred = true;
        return pushop1(token, src);
    }

    ErrPtr on_construct_last(TokenID token, Pos src) {
        // The `CONSTRUCT_LAST` token kind is for tokens that signify the last
        // construct component, like `else` or `finally`.
        // Functionally, they allow the parsing of a construct to be completed
        // without reading more tokens once this component is finished being
        // parsed, and therefore the construct may be executed earlier as well.
        if (!ctx_is(Context::CONSTRUCT))
            return ErrPtr(new UnexpectedTokenErr(token, src));

        // Replace `CONSTRUCT` with `CONSTRUCT_END` to signify that no more
        // construct components may be parsed for this construct after this one.
        contexts.pop_back();
        contexts.push_back(Context::CONSTRUCT_END);

        // It may seem strange to "expect" a value after tokens like `else` and
        // `finally`, but this is fine since the colon is a multiary token.
        // Allowing for this case is more flexible as nothing intrinsic to being
        // a terminal construct token implies that no predicate or similar value
        // may be parsed in general.
        orientation = Orientation::BEFORE;
        parsing_pred = true;
        prev_was_no_pred_construct =
            token == TokenID::ELSE || token == TokenID::FINALLY;
        return pushop1(token, src);
    }

    ErrPtr on_not_before() {
        orientation = Orientation::AFTER_NOT;
        return nullptr;
    }

    ErrPtr on_star_before() {
        orientation = Orientation::AFTER_STAR;
        return nullptr;
    }

    ErrPtr on_before(Token&& token, Pos src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case UNARY:
        case MULTIARY:
        case DUAL_AFFIX:
            if (in_brackets() && token.id == TokenID::STAR)
                return on_star_before();
            if (token.id == TokenID::NOT)
                return on_not_before();
            return on_unary(token.id, src);
        case VALUE:
            return on_value_before(std::move(token), src);
        case LEFT:
            return on_left_before(token.id, src);
        case RIGHT:
            return on_right_before(token.id, src);
        default:
            return ErrPtr(new UnexpectedTokenErr(token.id, src));
        }
    }

    ErrPtr on_stmt(Token&& token, Pos src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NULLARY:
            return on_nullary(token.id, src);
        case OPTIONAL_STMT:
            return on_optional_stmt(token.id, src);
        default:
            return on_before(std::move(token), src);
        }
    }

    ErrPtr on_start(Token&& token, Pos src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case CONSTRUCT_FIRST:
        case CONSTRUCT_FIRST_OR_BINARY:
            return on_construct_first(token.id, src);
        case CONSTRUCT_MIDDLE:
            return on_construct_middle(token.id, src);
        case CONSTRUCT_LAST:
        case CONSTRUCT_LAST_OR_BINARY:
            return on_construct_last(token.id, src);
        default:
            return on_stmt(std::move(token), src);
        }
    }

    ErrPtr on_after(Token&& token, Pos src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case VALUE:
            return on_value_after(std::move(token), src);
        case BINARY:
        case MULTIARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
            return on_binary(token.id, src);
        case DUAL_AFFIX:
            return on_postfix(token.id, src);
        case LEFT:
            return on_left_after(token.id, src);
        case RIGHT:
            return on_right_after(token.id, src);
        default:
            return ErrPtr(new UnexpectedTokenErr(token.id, src));
        }
    }

    ErrPtr on_after_not(Token&& token, Pos src) {
        if (token.id == TokenID::IN) {
            // "not" followed by "in" is a special case.
            orientation = Orientation::BEFORE;
            return pushop(OpID::NOT_IN, prev_src);
        }
        // Otherwise, push the not and default to "before" orientation
        // behavior.
        pushop(OpID::NOT, prev_src);
        return on_before(std::move(token), src);
    }

    ErrPtr on_after_star(Token&& token, Pos src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case UNARY:
        case MULTIARY:
            infer_expansion();
            return on_unary(token.id, src);
        case BINARY:
            if (token.id != TokenID::COMMA)
                return ErrPtr(new UnexpectedTokenErr(token.id, src));
            infer_arg_sep();
            return on_binary(token.id, src);
        case VALUE:
            infer_expansion();
            return on_value_before(std::move(token), src);
        case LEFT:
            infer_expansion();
            return on_left_before(token.id, src);
        case RIGHT:
            infer_arg_sep();
            return on_right_after(token.id, src);
        default:
            return ErrPtr(new UnexpectedTokenErr(token.id, src));
        }
    }

    ErrPtr on_hash(Pos src) {
        if (line_start)
            // Still may dedent if comment found at start of line.
            return on_leading_space(0, src);
        // Ignore comments.
        return nullptr;
    }

    ErrPtr on_end_of_file(Pos src) {
        // Treat EOF as though it is preceded by a newline.
        ErrPtr err = on_newline(TokenID::END_OF_FILE, src);
        if (err)
            return err;

        if (in_brackets())
            return ErrPtr(new UnclosedBracketErr(src));

        // Also, EOF fully dedents.
        err = on_leading_space(0, src);
        if (err)
            return err;

        // EOF closes all constructs.
        while (ctx_is(Context::CONSTRUCT)) {
            contexts.pop_back();
            queue.push_back(Op(OpID::END, src));
            queue.push_back(Op(OpID::STMT, src));
        }

        return nullptr;
    }

    ErrPtr on_newline(TokenID token, Pos src) {
        // The `TokenID` argument disambiguates between newline and EOF in case
        // an UnexpectedTokenErr is returned.
        line_start = true;
        if (in_brackets())
            // Ignore newlines inside of brackets.
            return nullptr;

        using enum Orientation;
        switch(orientation) {
        case BEFORE:
            return ErrPtr(new UnexpectedTokenErr(token, src));
        case OPTIONAL:
            // Missing optional value denoted as `NOTHING`.
            queue.push_back(Op(OpID::NOTHING, src));
            [[fallthrough]];
        case AFTER:
        case END: {
            parsing_pred = false;
            orientation = Orientation::START;
            queue.push_back(Op(OpID::STMT, src));
            if (ctx_is(Context::CONSTRUCT_END)) {
                // `CONSTRUCT_END` indicates that we should close a construct if
                // we are in `START` and it resides on top of `contexts`.
                contexts.pop_back();
                queue.push_back(Op(OpID::END, src));

                // Also, treat construct as a single statement.
                queue.push_back(Op(OpID::STMT, src));
            }
            return nullptr;
        }
        default:
            return nullptr;
        }
    }

    ErrPtr on_space(std::uint32_t count, Pos src) {
        if (line_start)
            return on_leading_space(count, src);
        // Ignore non-leading space.
        return nullptr;
    }

    ErrPtr feed(Token&& token, Pos src) override {
        // First, check for specific tokens that need special handling.
        switch(token.id) {
        case TokenID::HASH:
            return on_hash(src);
        case TokenID::NEWLINE:
            return on_newline(TokenID::NEWLINE, src);
        case TokenID::SPACE:
            return on_space(std::get<std::uint32_t>(token.data), src);
        case TokenID::END_OF_FILE:
            return on_end_of_file(src);
        default:;
        }

        bool prev_was_no_pred_construct_val = prev_was_no_pred_construct;

        parsing_loop_vars =
            parsing_loop_vars &&
            (token.id == TokenID::ID || token.id == TokenID::COMMA);
        check_if_construct_ends(token, src);
        ErrPtr res;
        if (line_start) {
            // Need to account for dedents when starting a new line without
            // leading spaces.
            res = on_leading_space(0, src);
            if (res)
                return res;
        }

        using enum Orientation;
        switch(orientation) {
        case START:
            res = on_start(std::move(token), src);
            break;
        case STMT:
            res = on_stmt(std::move(token), src);
            break;
        case BEFORE:
        case OPTIONAL:
            res = on_before(std::move(token), src);
            break;
        case AFTER:
            res = on_after(std::move(token), src);
            break;
        case AFTER_NOT:
            res = on_after_not(std::move(token), src);
            break;
        case AFTER_STAR:
            res = on_after_star(std::move(token), src);
            break;
        case END:
            return ErrPtr(new UnexpectedTokenErr(token.id, src));
        }
        // Newlines, comments, spaces, and EOF are not stored to prev.
        prev = token.id;
        prev_src = src;

        // Can't have two constructs in a row.
        prev_was_no_pred_construct =
            prev_was_no_pred_construct && !prev_was_no_pred_construct_val;
        return res;
    }
    
    Op next() override {
        if (queue.empty())
            return Op(OpID::WAITING, Pos());
        Op op = std::move(queue.front());
        queue.pop_back();
        return op;
    }
};

}
