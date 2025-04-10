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
#include "dl/span.hpp"

namespace dl {

// Indicates that an indent was expected but not found.
struct ExpectedIndentErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedIndentErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIndentErr";
    }
};

struct ExpectedValueErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedValueErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedValueErr";
    }
};

// Indicates that an indent which was not a multiple of four spaces was found.
struct IncompleteIndentErr final: SourcedErr {
    IncompleteIndentErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new IncompleteIndentErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "IncompleteIndentErr";
    }
};

// Indicates that more space than the current block's indentation have been
// found in a context that does not permit an indent.
struct OverIndentErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new OverIndentErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "OverIndentErr";
    }
};

// Indicates that the file has ended while a left bracket remains unclosed.
struct UnclosedBracketErr final: SourcedErr {
    UnclosedBracketErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new UnclosedBracketErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedBracketErr";
    }
};

// Indicates that less space than the current block's indentation have been
// found in context that does not permit a dedent.
struct UnderIndentErr final: SourcedErr {
    UnderIndentErr(Span src) noexcept: SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new UnderIndentErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnderIndentErr";
    }
};

// Indicates that a particular token was unexpected.
struct UnexpectedTokenErr final: SourcedErr {
    TokenID token;

    UnexpectedTokenErr(TokenID token, Span src) noexcept:
    token(token), SourcedErr(src) {}

    virtual ErrPtr copy() const override {
        return ErrPtr(new UnexpectedTokenErr(*this));
    }

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

    // Queue containing the parsed operations or errors if they occurred.
    std::deque<Res<Op>> queue;

    // Stack containing contexts to be popped when appropriate conditions are
    // met.
    // Contexts include blocks, brackets, and constructs.
    std::vector<Context> contexts;

    // When true, we are parsing loop vars and should use a higher-precedence
    // version of separation (comma) operator.
    bool parsing_loop_vars;

    // ID of previous token. Useful when corresponding operator is currently
    // ambiguous.
    TokenID prev;

    // Source for the previous token.
    Span prev_src;

    ParserImpl():
    depth(0),
    line_start(true),
    orientation(Orientation::START),
    queue(),
    contexts(),
    parsing_loop_vars(false),
    // TokenID::SPACE is just a meaningless placeholder until prev is set to
    // a token.
    // Note that this is the only case in which prev will be set to space.
    prev(TokenID::SPACE),
    prev_src(Span(Pos())) {}

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
    void pushop(OpID id, Span src) {
        queue.push_back(Op(id, src));
    }
    
    void pushop1(TokenID token, Span src) {
    	pushop(tokeninfo(token).op1, src);
    }

    void pushop2(TokenID token, Span src) {
        pushop(tokeninfo(token).op2, src);
    }

    Span prev_end() {
        return Span(prev_src.end_line, prev_src.end_col);
    }

    void close_ctx() {
        contexts.pop_back();

        // End the context, then treat as single statement.
        queue.push_back(Op(OpID::END, prev_end()));
        queue.push_back(Op(OpID::STMT, prev_end()));
    }

    void push_stmt(Span src) {
        orientation = Orientation::START;
        queue.push_back(Op(OpID::STMT, src));
        if (ctx_is(Context::CONSTRUCT_END))
            // `CONSTRUCT_END` indicates that we should close a construct if
            // we are in `START` and it resides on top of `contexts`.
            close_ctx();
    }

    // Check whether there is at least one context and that the top context is
    // `ctx`.
    bool ctx_is(Context ctx) const noexcept {
        return !contexts.empty() && contexts.back() == ctx;
    }

    // Check if the incoming token implies that we are finished parsing a
    // construct.
    void check_if_construct_ends(const Token& token) {
        if (
            orientation == Orientation::START &&
            ctx_is(Context::CONSTRUCT) &&
            !continues_construct(token.id)
        )
            close_ctx();
    }

    // Determine whether we are currently between brackets.
    bool in_brackets() const noexcept {
        return
            ctx_is(Context::CURLY) ||
            ctx_is(Context::CURVED) ||
            ctx_is(Context::SQUARE);
    }

    // Perform an indentation.
    void indent(Span src) {
        orientation = Orientation::START;
        depth++;
        contexts.push_back(Context::BLOCK);

        queue.push_back(Op(
            OpID::BLOCK,
            // Need span to be the start of the block.
            Span(
                src.start_line,
                src.end_col + 1,
                src.start_line,
                src.end_col + 1
            )
        ));
    }

    // Force indents when they are unexpected.  While it is an error to indent
    // more than once at a time, the parser does so in order to continue parsing
    // when errors arise.
    void force_indents(std::uint32_t n, Span src) {
        for (std::uint32_t i = 0; i < n; i++) {
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new OverIndentErr()), src)
            );
            // Push the LABEL operator so that we're basically parsing
            // error:
            //     <block>
            queue.push_back(Op(OpID::LABEL, src));
            indent(src);
        }
    }

    // Handle dedenting the specified number of times.
    void handle_dedents(std::uint32_t dedents) {
        for (std::uint32_t i = 0; i < dedents; i++) {
            Context ctx = contexts.back();
            if (ctx == Context::CONSTRUCT || ctx == Context::CONSTRUCT_END) {
                // Dedent implicitly closes a construct.
                close_ctx();
                ctx = contexts.back();
            }
            // There can be at most one unpushed construct at the end of
            // each block so there is no need to check for another.
            close_ctx();
            depth--;
        }

        // Additionally, if CONSTRUCT_END is on the context stack after
        // dedenting, pop that context too.
        if (ctx_is(Context::CONSTRUCT_END))
            close_ctx();
    }

    void on_leading_space_start(std::uint32_t count, Span src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (leftover)
            // Additional space before the start of a statement is not allowed.
            queue.push_back(ErrPtr(new IncompleteIndentErr(src)));

        if (indents > depth)
            // Force indents in order to continue parsing.
            force_indents(indents - depth, src);
        
        else if (indents < depth)
            handle_dedents(depth - indents);
    }

    void on_leading_space_indenting(std::uint32_t count, Span src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (leftover)
            queue.push_back(ErrPtr(new IncompleteIndentErr(src)));

        // No matter what happens, we're going to indent once.
        // Note that this increments depth.
        indent(src);
        if (indents < depth - 1) {
            // Propagate error as a statement to ensure a non-empty block.
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new UnderIndentErr(src)), src)
            );
            // Force dedents to continue parsing.
            handle_dedents(depth - indents);
        }
        else if (indents > depth)
            // Force the rest of the indents to continue parsing.
            force_indents(indents - depth, src);
        else if (indents == depth - 1) {
            // An indent is expected when in INDENTING orientation with leading
            // space.
            // Indent, propagate the error to have a non-empty block, and then
            // dedent.
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new ExpectedIndentErr()), src)
            );
            handle_dedents(1);
        }
        // We now know indents == depth
        // When the number of indents is exactly one more than depth
        // without extra space and the state is in the INDENTING
        // orientation a *proper* indent has occurred.
    }

    void on_leading_space_before(std::uint32_t count, Span src) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (indents < depth)
            // Can only dedent in `START`.
            // Note that we must be in brackets to achieve this state in the
            // first place. Rather than closing them all to force a dedent,
            // just proceed as though under indenting inside brackets is
            // allowed.
            queue.push_back(ErrPtr(new UnderIndentErr(src)));
    }

    void on_leading_space_after(std::uint32_t count, Span src) {
        std::uint32_t indents = count / 4;
        if (indents < depth)
            // Can only dedent in `START` orientation.
            // Like for leading space in before orientation, we must be in
            // brackets. Do not force a dedent.
            queue.push_back(ErrPtr(new UnderIndentErr(src)));
    }

    void on_leading_space(std::uint32_t count, Span src) {
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
            on_leading_space_start(count, src);
            return;
        case INDENTING:
            on_leading_space_indenting(count, src);
            return;
        case BEFORE:
            on_leading_space_before(count, src);
            return;
        case AFTER:
        case AFTER_STAR:
            on_leading_space_after(count, src);
            return;
        default:
            queue.push_back(
                ErrPtr(new AssertionFailedErr("Impossible orientation"))
            );
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

    void on_nullary(TokenID token, Span src) {
        orientation = Orientation::END;
        pushop1(token, src);
    }

    void on_unary(TokenID token, Span src) {
        orientation = Orientation::BEFORE;
        pushop1(token, src);
    }

    void on_binary(TokenID token, Span src) {
        // Expecting a value now.
        orientation = Orientation::BEFORE;
        if (token == TokenID::COMMA && parsing_loop_vars)
            // Special case: comma has higher precedence when parsing loop
            // variables.
            pushop(OpID::LOOP_VAR_SEP, src);

        else if (token == TokenID::EQUALS && in_brackets())
            // Special case: equals changes meaning and precedence inside
            // brackets.
            pushop(OpID::BIND, src);
        else
            pushop2(token, src);
    }

    void on_postfix(TokenID token, Span src) {
        orientation = Orientation::AFTER;
        pushop2(token, src);
    }

    void on_value_before(Token&& token, Span src) {
        // op1 is for values and unary operators.
        orientation = Orientation::AFTER;
        queue.push_back(
            Op(tokeninfo(token.id).op1, std::move(token.data), src)
        );
    }

    void on_value_after(Token&& token, Span src) {
        // When an immediate appears directly after another value, this case is
        // treated as though there is an "invisible" operator between them.
        pushop2(token.id, src);
        // Push the value in either case.
        on_value_before(std::move(token), src);
    }

    void on_optional_stmt(TokenID token, Span src) {
        orientation = Orientation::OPTIONAL;
        pushop1(token, src);
    }

    void on_left_before(TokenID token, Span src) {
        contexts.push_back(tokeninfo(token).match);
        // A parenthetical or similar is "value-like", so we push `op1`.
        pushop1(token, src);
        orientation = Orientation::BEFORE;
    }

    void on_left_after(TokenID token, Span src) {
        // When a left bracket directly follows a value, this case is
        // treated as though there is an "invisible" binary operator between
        // them.
        pushop2(token, src);

        // In any case, the value operator is still pushed.
        on_left_before(token, src);
    }

    void on_right_before(TokenID token, Span src) {
        // A RIGHT is only valid in BEFORE orientation if the previous token was
        // it's matching left token (i.e. empty parentheses).
        if (tokeninfo(prev).kind != TokenKind::LEFT) {
            // In this case, we got a right bracket where we expected a value.
            // Insert a value via the ERROR operator to continue parsing.
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new ExpectedValueErr()), src)
            );
            orientation = Orientation::AFTER;
            on_right_after(token, src);
            return;
        }
        else if (tokeninfo(prev).match != tokeninfo(token).match) {
            // Mismatched brackets. See below for comments on how to handle
            // this.
            queue.push_back(ErrPtr(new UnexpectedTokenErr(token, src)));
            return;
        }
        contexts.pop_back();
        // Push OpID::NOTHING to indicate lack of a value.
        queue.push_back(Op(OpID::NOTHING, src));
        queue.push_back(Op(OpID::END, src));
        orientation = Orientation::AFTER;
        
    }

    void on_right_after(TokenID token, Span src) {
        if (!ctx_is(tokeninfo(token).match)) {
            // Mismatched brackets. To continue parsing, there are a few
            // options:
            // 1: Continue as if ignoring the token.
            // 2: Treat the token as if it were the correct bracket.
            // 3: Pop contexts until it matches this right bracket.
            // 4: Insert the correct brackets until the context matches this
            //    right bracket.
            // (3) is not a great solution because that context may not exist,
            // and a simple typo will trigger possibly many more mismatched
            // bracket errors down the line. Same with (4).
            // (2) is a somewhat nice solution, but it is also surprising.
            // (1) seems to be the simplest and best solution.
            queue.push_back(ErrPtr(new UnexpectedTokenErr(token, src)));
            return;
        }
        // Orientation stays `AFTER` after finding a right bracket.
        contexts.pop_back();
        queue.push_back(Op(OpID::END, src));
    }

    void on_construct_first(TokenID token, Span src) {
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
        pushop1(token, src);
    }

    void on_construct_middle(TokenID token, Span src) {
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
            // We're not in a construct.
            // To continue parsing anyway, just propagate the operator as if we
            // were: it will be treated as a low precedence unary operator.

            queue.push_back(ErrPtr(new UnexpectedTokenErr(token, src)));

        // Expecting a value now.
        orientation = Orientation::BEFORE;
        pushop1(token, src);
    }

    void on_construct_last(TokenID token, Span src) {
        // The `CONSTRUCT_LAST` token kind is for tokens that signify the last
        // construct component, like `else` or `finally`.
        // Functionally, they allow the parsing of a construct to be completed
        // without reading more tokens once this component is finished being
        // parsed, and therefore the construct may be executed earlier as well.
        if (!ctx_is(Context::CONSTRUCT))
            // Like for CONSTRUCT_MIDDLE, propagate the operator as a low
            // precedence unary operator. Do not push a CONSTRUCT_END context,
            // since we are not actually in a construct.
            queue.push_back(ErrPtr(new UnexpectedTokenErr(token, src)));
        else {
            // Replace `CONSTRUCT` with `CONSTRUCT_END` to signify that no more
            // construct components may be parsed for this construct after this
            // one.
            contexts.pop_back();
            contexts.push_back(Context::CONSTRUCT_END);
        }

        // It may seem strange to "expect" a value after tokens like `else` and
        // `finally`, but this is fine since the colon is a multiary token.
        // Allowing for this case is more flexible as nothing intrinsic to being
        // a terminal construct token implies that no predicate or similar value
        // may be parsed in general.
        orientation = Orientation::BEFORE;
        pushop1(token, src);
    }

    void on_not_before() {
        orientation = Orientation::AFTER_NOT;
    }

    void on_star_before() {
        orientation = Orientation::AFTER_STAR;
    }

    void on_before0(Token&& token, Span src) {
        if (token.id == TokenID::COLON && !in_brackets()) {
            // Could be start of indent, don't push anything yet as different
            // operators need to be pushed depending on whether or not we are
            // indenting.
            // Set Orientation explicitly to BEFORE in case it was START before,
            // which would cause the prev colon checks in on_before and on_after
            // to fail.
            orientation = Orientation::BEFORE;
            return;
        }

        using enum TokenKind;
        switch(tokeninfo(token.id).kind) {
        case UNARY:
        case MULTIARY:
        case DUAL_AFFIX:
            if (in_brackets() && token.id == TokenID::STAR)
                on_star_before();
            else if (token.id == TokenID::NOT)
                on_not_before();
            else
                on_unary(token.id, src);
            return;
        case VALUE:
            on_value_before(std::move(token), src);
            return;
        case LEFT:
            on_left_before(token.id, src);
            return;
        case RIGHT:
            on_right_before(token.id, src);
            return;
        default:
            // A value was expected. Propagate this information and try on_after
            // instead.
            queue.push_back(Op(
                OpID::ERROR, ErrPtr(new ExpectedValueErr()), src)
            );
            orientation = Orientation::AFTER;
            on_after(std::move(token), src);
        }
    }

    void on_before(Token&& token, Span src) {
        using enum TokenKind;

        // on_before0 exists because we need a version of this function without
        // the following block to avoid double pushing operators.
        if (
            prev == TokenID::COLON &&
            !in_brackets() &&
            orientation != Orientation::START
        )
            // No indent after last colon, push its unary operator.
            on_unary(TokenID::COLON, prev_src);
        on_before0(std::move(token), src);
    }

    void on_start(Token&& token, Span src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case CONSTRUCT_FIRST:
        case CONSTRUCT_FIRST_OR_BINARY:
            on_construct_first(token.id, src);
            return;
        case CONSTRUCT_MIDDLE:
            on_construct_middle(token.id, src);
            return;
        case CONSTRUCT_LAST:
        case CONSTRUCT_LAST_OR_BINARY:
            on_construct_last(token.id, src);
            return;
        case NULLARY:
            on_nullary(token.id, src);
            return;
        case OPTIONAL_STMT:
            on_optional_stmt(token.id, src);
            return;
        default:
            on_before(std::move(token), src);
        }
    }

    void on_after(Token&& token, Span src) {
        using enum TokenKind;

        if (
            prev == TokenID::COLON &&
            !in_brackets() &&
            orientation != Orientation::START
        ) {
            // No indent after last colon, push its binary operator.
            on_binary(TokenID::COLON, prev_src);
            if (token.id != TokenID::COLON)
                // Need to call on_before0 to avoid pushing colon operator
                // again, since prev is still TokenID::COLON.
                on_before0(std::move(token), src);
            return;
        }

        if (token.id == TokenID::COLON) {
            if (ctx_is(Context::CURLY)) {
                // Special case: Colon directly inside curly braces is the ENTRY
                // operator.
                pushop(OpID::ENTRY, src);
                orientation = Orientation::BEFORE;
                return;
            }
            else if (!in_brackets())
                // Could be start of indent, don't push anything yet as
                // different operators need to be pushed depending on whether or
                // not we are indenting.
                return;
        }

        if (token.id == TokenID::COLON && !in_brackets())
            
            return;

        switch(tokeninfo(token.id).kind) {
        case VALUE:
            on_value_after(std::move(token), src);
            return;
        case BINARY:
        case MULTIARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
            on_binary(token.id, src);
            return;
        case DUAL_AFFIX:
            on_postfix(token.id, src);
            return;
        case LEFT:
            on_left_after(token.id, src);
            return;
        case RIGHT:
            on_right_after(token.id, src);
            return;
        default:
            // To continue parsing, just ignore this token: while inserting a
            // statement separator is *almost* a good solution, we may be in
            // brackets in general. So just ignore it.
            queue.push_back(ErrPtr(new UnexpectedTokenErr(token.id, src)));
        }
    }

    void on_after_not(Token&& token, Span src) {
        if (token.id == TokenID::IN) {
            // "not" followed by "in" is a special case.
            orientation = Orientation::BEFORE;
            pushop(OpID::NOT_IN, Span(prev_src, src));
        } else {
            // Otherwise, push the not and default to "before" orientation
            // behavior.
            pushop(OpID::NOT, prev_src);
            on_before(std::move(token), src);
        }
    }

    void on_after_star(Token&& token, Span src) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case UNARY:
        case MULTIARY:
            infer_expansion();
            on_unary(token.id, src);
            return;
        case BINARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
            if (token.id != TokenID::COMMA) {
                // Insert a value to make things right.
                infer_expansion();
                queue.push_back(
                    Op(OpID::ERROR, ErrPtr(new ExpectedValueErr()), src)
                );
                on_binary(token.id, src);
            }
            else {
                infer_arg_sep();
                on_binary(token.id, src);
            }
            return;
        case VALUE:
            infer_expansion();
            on_value_before(std::move(token), src);
            return;
        case LEFT:
            infer_expansion();
            on_left_before(token.id, src);
            return;
        case RIGHT:
            infer_arg_sep();
            on_right_after(token.id, src);
            return;
        default:
            // Insert a value to make things right and try "on_after".
            infer_expansion();
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new ExpectedValueErr()), src)
            );
            on_after(std::move(token), src);
        }
    }

    void on_hash(Span src) {
        if (line_start)
            // Still may dedent if comment found at start of line.
            on_leading_space(0, src);
    }

    void on_end_of_file(Span src) {
        while (in_brackets()) {
            // Encountering EOF while in brackets is an error, but to finish
            // parsing gracefully, we need to close them.
            // Simplest way is to feed them into the parser, which always
            // results in closing a bracket (perhaps after emitting errors)
            queue.push_back(ErrPtr(new UnclosedBracketErr(src)));

            using enum Context;
            switch(contexts.back()) {
            case CURVED:
                feed(Token(TokenID::RIGHT_CURVED), src);
                break;
            case SQUARE:
                feed(Token(TokenID::RIGHT_SQUARE), src);
                break;
            case CURLY:
                feed(Token(TokenID::RIGHT_CURLY), src);
                break;
            default:
                queue.push_back(ErrPtr(new AssertionFailedErr(
                    "Not in brackets."
                )));
            }
        }

        // Treat EOF as though it is preceded by a newline.
        on_newline(TokenID::END_OF_FILE, src);

        // Also, EOF fully dedents.
        on_leading_space(0, src);

        // EOF closes all constructs.
        while (ctx_is(Context::CONSTRUCT))
            close_ctx();
    }

    void on_newline(TokenID token, Span src) {
        // The `TokenID` argument disambiguates between newline and EOF in case
        // an error needs to be propagated.
        line_start = true;
        if (in_brackets())
            // Ignore newlines inside of brackets.
            return;

        using enum Orientation;
        if (prev == TokenID::COLON) {
            // Special case: newline following a colon outside of brackets means
            // to expect an indent.
            switch(orientation) {
            case AFTER:
                pushop(OpID::LABEL, prev_src);
                break;
            case BEFORE:
            case OPTIONAL:
            case START:
                pushop(OpID::BODY, prev_src);
                break;
            default:;
            }
            orientation = Orientation::INDENTING;
            return;
        }

        switch(orientation) {
        case BEFORE:
            // Expected a value.
            queue.push_back(
                Op(OpID::ERROR, ErrPtr(new ExpectedValueErr()), src)
            );
            push_stmt(src);
            return;
        case OPTIONAL:
            // Missing optional value denoted as `NOTHING`.
            queue.push_back(Op(OpID::NOTHING, src));
            [[fallthrough]];
        case AFTER:
        case END:
            push_stmt(src);
        default:;
        }
    }

    void on_space(std::uint32_t count, Span src) {
        if (line_start)
            on_leading_space(count, src);
    }

    void feed(Token&& token, Span src) override {
        // First, check for specific tokens that need special handling.
        switch(token.id) {
        case TokenID::HASH:
            on_hash(src);
            return;
        case TokenID::NEWLINE:
            on_newline(TokenID::NEWLINE, src);
            return;
        case TokenID::SPACE:
            on_space(std::get<std::uint32_t>(token.data), src);
            return;
        case TokenID::END_OF_FILE:
            on_end_of_file(src);
            return;
        default:;
        }

        parsing_loop_vars =
            parsing_loop_vars &&
            (token.id == TokenID::ID || token.id == TokenID::COMMA);
        check_if_construct_ends(token);
        if (line_start) {
            // Need to account for dedents when starting a new line without
            // leading spaces.
            on_leading_space(
                0, Span(src.start_line, 1, src.start_line, 1)
            );
        }

        using enum Orientation;
        switch(orientation) {
        case START:
            on_start(std::move(token), src);
            break;
        case BEFORE:
        case OPTIONAL:
            on_before(std::move(token), src);
            break;
        case AFTER:
            on_after(std::move(token), src);
            break;
        case AFTER_NOT:
            on_after_not(std::move(token), src);
            break;
        case AFTER_STAR:
            on_after_star(std::move(token), src);
            break;
        case INDENTING:
            // Can't be in indenting, because line_start would have been true
            // and on_leading_space would have transitioned us to another
            // orientation.
            queue.push_back(ErrPtr(new AssertionFailedErr(
                "INDENTING orientation is impossible here"
            )));
            break;
        case END:
            // Encountering a non-special token in END orientation is an error.
            // To continue parsing, just ignore it. Further tokens found on this
            // line will continue to yield UnexpectedTokenErr.
            queue.push_back(
                ErrPtr(new UnexpectedTokenErr(token.id, src))
            );
            break;
        }
        // Newlines, comments, spaces, and EOF are not stored to prev.
        prev = token.id;
        prev_src = src;
    }
    
    Res<Op> next() override {
        if (queue.empty())
            return Op(OpID::WAITING, Span(1, 1));
        Res<Op> op = std::move(queue.front());
        queue.pop_front();
        return op;
    }
};

}
