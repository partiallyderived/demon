#pragma once

#include <cstdint>

#include <ostream>
#include <queue>
#include <stack>
#include <utility>

#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/parse/context.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/orientation.hpp"
#include "dl/parse/tokenkind.hpp"

namespace dl {

// Indicates that an indent which was not a multiple of four spaces was found.
struct IncompleteIndentErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "IncompleteIndentErr";
    }
};

// Indicates that more space than the current block's indentation have been
// found in a context that does not permit an indent.
struct OverIndentErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "OverIndentErr";
    }
};

// Indicates that the file has ended while a left bracket remains unclosed.
struct UnclosedBracketErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnclosedBracketErr";
    }
};

// Indicates that less space than the current block's indentation have been
// found in context that does not permit a dedent.
struct UnderIndentErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnderIndentErr";
    }
};

// Indicates that EOF was found unexpectedly.
struct UnexpectedEOFErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedEOFErr";
    }
};

// Indicates that an particular token was unexpected.
struct UnexpectedTokenErr final: Err {
    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedTokenErr";
    }
}

struct ParserImpl {
    // Depth of indentation.
    std::uint32_t depth;

    // Indicates whether we are at the start of a line.
    bool line_start;

    // Orientation ("state") of the parser.
    Orientation orientation;

    // Queue containing the parsed operations.
    std::queue<Op> queue;

    // Stack containing contexts to be popped when appropriate conditions are
    // met.
    // Contexts include blocks, brackets, and constructs.
    std::stack<Context> contexts;

    // Previous token. Used when corresponding operator is currently
    // ambiguous.
    Token prev;

    // Source ID for the previous token.
    std::uint64_t prev_src;

    ParserImpl():
    depth(0),
    line_start(true),
    orientation(Orientation::START),
    queue(),
    contexts(),
    // TokenID::SPACE is just a meaningless placeholder until prev is set to
    // a token.
    // Note that this is the only case in which prev will be set to space.
    prev(TokenID::SPACE, 0),
    prev_src(-1) {}

    // Determines whether a token continues the parsing of a construct in the
    // outermost block in which that construct is being parsed, such as `elif`
    // or `else`, but not `if`, which would initiate a new construct.
    static bool continues_construct(TokenID id) noexcept {
        switch(tokeninfo(id).kind) {
        case TokenKind::CONSTRUCT_MIDDLE:
        case TokenKind::CONSTRUCT_END:
            return true;
        default:
            return false;
        }
    }

    // Convenience functions for pushing operators.
    // Return values are to make code more concise.
    ErrPtr pushop(Token& token, OpID id, std::uint64_t src_id) {
    	queue.push(Op(id, std::move(token.content), src_id));
    	return nullptr;
    }
    
    ErrPtr pushop1(Token& token, std::uint64_t src_id) {
    	return pushop(token, tokeninfo(token.id).op1, src_id);
    }

    ErrPtr pushop2(Token& token, std::uint64_t src_id) {
        return pushop(token, tokeninfo(token.id).op2, src_id);
    }

    ErrPtr pushop3(Token& token, std::uint64_t src_id) {
        return pushop(token, tokeninfo(token.id).op3, src_id);
    }

    // Check whether there is at least one context and that the top context is
    // `ctx`.
    bool ctx_is(Context ctx) const noexcept {
        return !contexts.empty() && contexts.top() == ctx;
    }

    // Check if the incoming token implies that we are finished parsing a
    // construct.
    void check_if_construct_ends(const Token& token, std::uint64_t src_id) {
        if (
        	ctx_is(Context::CONSTRUCT) &&
        	!continues_construct(token.id) &&
        	token.id != TokenID::NEWLINE
        ) {
            contexts.pop();
            queue.push(Op(OpID::END, src_id));
            queue.push(Op(OpID::STMT, src_id));
        }
    }

    // Determine whether we are currently between brackets.
    bool in_brackets() const noexcept {
        return ctx_is(Context::CURVED) || ctx_is(Context::SQUARE);
    }

    // Handle dedenting the specified number of times.
    ErrPtr handle_dedents(std::uint32_t num_dedents, std::uint64_t src_id) {
        for (std::uint32_t i = 0; i < dedents; i++) {
            Context ctx = contexts.top();
            if (ctx == Context::CONSTRUCT || ctx == Context::CONSTRUCT_END) {
                // Dedent implicitly closes a construct.
                contexts.pop();
                queue.push(Op(OpID::END, src_id));

                // Construct considered a single statement.
                queue.push(Op(OpID::STMT, src_id));
                ctx = contexts.top();
            }
            // There can be at most one unpushed construct at the end of
            // each block so there is no need to check for another.
            if (ctx != Context::BLOCK)
                // Dedent while contents of bracket is being populated,
                // therefore the bracket is unclosed.
                return ErrPtr(new UnclosedBracketErr());
            contexts.pop();
            depth--;

            // Push an `END` to the queue to signal end of a block.
            queue.push(Op(OpID::END, src_id));
            
            // Additionally, every block is implicitly treated as a statement
            // in the enclosing block.
            queue.push(Op(OpID::STMT, src_id));
        }
        return nullptr;
    }

    void handle_stmt_sep(std::uint64_t src_id) {
        if (orientation == Orientation::OPTIONAL)
            // For now, a missing optional value is inferred to be `None`.
            queue.push(Op(OpID::NONE, src_id));
        orientation = Orientation::START;
        queue.push(Op(OpID::STMT, src_id));
        if (ctx_is(Context::CONSTRUCT_END)) {
            // `CONSTRUCT_END` indicates that we should close a construct if we
            // are in `START` and it resides on top of `contexts`.
            contexts.pop();
            queue.push(Op(OpID::END, src_id));

            // Also, treat construct as a single statement.
            queue.push(Op(OpID::STMT, src_id));
        }
    }

    ErrPtr on_newline(std::uint64_t src_id) {
        // Inside brackets, newline does not act as a statement separator.
        if (!in_brackets())
            handle_stmt_sep(src_id);
        return nullptr;
    }

    ErrPtr on_leading_space_start(std::uint32_t count, std::uint64_t src_id) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (leftover)
            // Additional space before the start of a statement is not
            // allowed.
            return ErrPtr(new IncompleteIndentErr());
        if (indents > depth)
            return ErrPtr(new OverIndentErr());
        if (indents == depth)
            // No indent/dedent, just return and keep orientation.
            return nullptr;
        // We now know `indents < depth`, so we're dedenting.
        return handle_dedents(depth - indents, src_id);
    }

    ErrPtr on_leading_space_before(std::uint32_t count, std::uint64_t src_id) {
        std::uint32_t indents = count / 4;
        std::uint32_t leftover = count % 4;
        if (indents < depth)
            // Can only dedent in `START`.
            return ErrPtr(new UnderIndentErr());
        if (
            indents == depth + 1 &&
            !leftover &&
            !in_brackets() &&
            prev.obj == TokenID::COLON
        ) {
            // 1. State is in the left orientation.
            // 2. # Indents is exactly one more without any extra space
            // 3. Indent was not found between brackets.
            // 4. Operator on stack is a colon operator.
            // These are the conditions for increasing the depth of
            // indentation.
            depth++;
            orientation = ORIENTATION::START;
            contexts.push(Context::BLOCK);
            queue.push(Op(OpID::BLOCK, std::uint64_t));
        }
        return nullptr;
    }

    ErrPtr on_leading_space_after(std::uint32_t count, std::uint64_t src_id) {
        std::uint32_t indents = count / 4;
        if (indents < depth)
            // Can only dedent in `START` orientation.
            return ErrPtr(new UnderIndentErr());
        return nullptr;
    }

    ErrPtr on_leading_space(std::uint32_t count, std::uint64_t src_id) {
        // Note: since this is the start of a newline, we cannot be in the
        // `OPTIONAL` or `END` orientations. This is because we must either
        // be at the start of the file, or directly after a newline. We
        // start the file in the `START` orientation, and a newline
        // reorients `OPTIONAL` and `END`.
        // Check indents/dedents.
        switch (orientation) {
        case Orientation::START:
            return on_leading_space_start(count, src_id);
        case Orientation::BEFORE:
            return on_leading_space_before(count, src_id);
        case Orientation::AFTER:
            return on_leading_space_after(count, src_id);
        default:
            return ErrPtr(new AssertionFailedErr("Impossible orientation"));
        }
    }

    void infer_binary() {
        // In `AFTER_BINARY_OR_POSTFIX` orientation, infer binary.
        orientation = Orientation::BEFORE;
        pushop2(prev, prev_src);
    }

    void infer_postfix() {
        // In `AFTER_BINARY_OR_POSTFIX` orientation, infer postfix.
        orientation = Orientation::AFTER;
        pushop3(prev, prev_src);
    }

    void infer_unary() {
        // In `AFTER_UNARY_OR_VALUE` orientation, infer unary.
        orientation = Orientation::BEFORE;
        pushop1(prev, prev_src);
    }

    void infer_value() {
        // In `AFTER_UNARY_OR_VALUE` orientation, infer value.
        orientation = Orientation::AFTER;
        pushop3(prev, prev_src);
    }

    ErrPtr on_nullary(Token& token, std::uint64_t src_id) {
        orientation = Orientation::END;
        return pushop1(token, src_id);
    }

    ErrPtr on_unary(Token& token, std::uint64_t src_id) {
        orientation = Orientation::BEFORE;
        return pushop1(token, src_id);
    }

    ErrPtr on_binary(Token& token, std::uint64_t src_id) {
        // Expecting a value now.
        orientation = Orientation::BEFORE;
        return pushop2(token, src_id);
    }

    ErrPtr on_value_before(Token& token, std::uint64_t src_id) {
        // op3 is the "value" operator.
        orientation = Orientation::AFTER;
        return pushop3(token, src_id);
    }

    ErrPtr on_value_after(Token& token, std::uint64_t src_id) {
        // When an immediate appears directly after another value, this
        // case is treated as though there is an "invisible" operator
        // between them.
        // This operator is `op2`.
        pushop2(token, src_id);
        // Push the value in either case.
        return on_value_before(token, src_id);
    }

    ErrPtr on_multiary_value_before(Token& token, std::uint64_t src_id) {
        // Ambiguous whether `token` is meant to be interpreted as a unary
        // operator or a value. Await more tokens to disambiguate.
        orientation = Orientation::AFTER_UNARY_OR_VALUE;
        return nullptr;
    }

    ErrPtr on_return(Token& token, std::uint64_t src_id) {
        orientation = Orientation::OPTIONAL;
        return pushop1(token, src_id);
    }

    ErrPtr on_left_before(Token& token, std::uint64_t src_id) {
        contexts.push(tokeninfo(token.id).match);
        // A parenthetical or similar is "value-like", so we push `op3`.
        return pushop3(token, src_id);
    }

    ErrPtr on_left_after(Token& token, std::uint64_t src_id) {
        contexts.push(tokeninfo(token.obj).match);
        // When a left bracket directly follows a value, this case is
        // treated as though there is an "invisible" binary operator between
        // them.
        pushop2(token, src_id);
        orientation = Orientation::BEFORE;

        // In any case, the value operator is still pushed.
        return pushop3(token, src_id);
    }

    ErrPtr on_right(Token& token, std::uint64_t src_id) {
        if (!ctx_is(tokeninfo(token.id).match))
            return ErrPtr(new UnexpectedTokenErr());
        // Orientation stays `AFTER` after finding a right bracket.
        contexts.pop();
        queue.push(Op(OpID::END, src_id));
        return nullptr;
    }

    ErrPtr on_construct_first(Token& token, std::uint64_t src_id) {
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
        contexts.push(Context::CONSTRUCT);
        // Expecting a value now.
        orientation = Orientation::BEFORE;
        queue.push(Op(OpID::CONSTRUCT, src_id));
        return pushop1(token, src_id);
    }

    ErrPtr on_construct_middle(Token& token, std::uint64_t src_id) {
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
            return ErrPtr(new UnexpectedTokenErr());
        // Expecting a value now.
        orientation = Orientation::BEFORE;
        return pushop1(token, src_id);
    }

    ErrPtr on_construct_last(Token& token, std::uint64_t src_id) {
        // The `CONSTRUCT_LAST` token kind is for tokens that signify the last
        // construct component, like `else` or `finally`.
        // Functionally, they allow the parsing of a construct to be completed
        // without reading more tokens once this component is finished being
        // parsed, and therefore the construct may be executed earlier as well.
        if (!ctx_is(Context::CONSTRUCT))
            return ErrPtr(new UnexpectedTokenErr());
        // Replace `CONSTRUCT` with `CONSTRUCT_END` to signify that no more
        // construct components may be parsed for this construct after this one.
        contexts.pop();
        contexts.push(Context::CONSTRUCT_END);
        // It may seem strange to "expect" a value after tokens like `else` and
        // `finally`, but this is fine since the colon is a multiary operator.
        // Allowing for this case is more flexible as nothing intrinsic to being
        // a terminal construct token implies that no predicate or similar value
        // may be parsed in general.
        orientation = Orientation::BEFORE;
        return pushop1(token, src_id);
    }

    ErrPtr on_end_of_file(std::uint64_t src_id) {
        if (in_brackets())
            return ErrPtr(new UnclosedBracketErr());
        // EOF acts as a statement separator like newline here.
        handle_stmt_sep(src_id);
        queue.push(Op(OpID::DONE, src_id));
        return nullptr;
    }

    ErrPtr on_before(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            return nullptr;
        case UNARY:
        case MULTIARY:
        case MULTIARY_POSTFIX:
            return on_unary(token, src_id);
        case VALUE:
            return on_value_before(token, src_id);
        case MULTIARY_VALUE:
            return on_multiary_value_before(token, src_id);
        case LEFT:
            return on_left_before(token, src_id);
        default:
            return new UnexpectedTokenErr();
        }
    }

    ErrPtr on_start(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
        case END_OF_FILE:
            return nullptr;
        case NULLARY:
            return on_nullary(token, src_id);
        case CONSTRUCT_FIRST:
        case CONSTRUCT_FIRST_OR_BINARY:
            return on_construct_first(token, src_id);
        case CONSTRUCT_MIDDLE:
            return on_construct_middle(token, src_id);
        case CONSTRUCT_LAST:
        case CONSTRUCT_LAST_OR_BINARY:
            return on_construct_last(token, src_id);
        case RETURN:
            return on_return(token, src_id);
        case END_OF_FILE:
            return on_end_of_file(token, src_id);
        default:
            return on_before(token, src_id);
        }
    }

    ErrPtr on_after(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            return on_newline(token, src_id);
        case BINARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
        case MULTIARY:
        case MULTIARY_VALUE:
            return on_binary(token, src_id);
        case LEFT:
            return on_left_after(token, src_id);
        case RIGHT:
            return on_right(token, src_id);
        case END_OF_FILE:
            return on_end_of_file(token, src_id);
        default:
            return new UnexpectedTokenErr();
        }
    }

    ErrPtr on_after_binary_or_postfix(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            if (!in_brackets()) {
                infer_postfix();
                return on_newline(token, src_id);
            }
            return nullptr;
        case UNARY:
            infer_binary();
            return on_unary(token, src_id);
        case BINARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
        case MULTIARY:
        case MULTIARY_VALUE:
            infer_postfix();
            return on_binary(token, src_id);
        case VALUE:
            infer_binary();
            return on_value_before(token, src_id);
        case LEFT:
            infer_binary();
            return on_left_before(token, src_id);
        case RIGHT:
            infer_postfix();
            return on_right(token, src_id);
        case END_OF_FILE:
            infer_postfix();
            return on_end_of_file(token, src_id);
        default:
            return new UnexpectedTokenErr();
        }
    }

    ErrPtr on_after_unary_or_value(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            if (!in_brackets())
                infer_unary();
            return nullptr;
        case UNARY:
        case MULTIARY:
            infer_unary();
            return on_unary(token, src_id);
        case BINARY:
        case CONSTRUCT_FIRST_OR_BINARY:
        case CONSTRUCT_LAST_OR_BINARY:
            infer_value();
            return on_binary(token, src_id);
        case VALUE:
            infer_unary();
            return on_value_before(token, src_id);
        case MULTIARY:
            infer_unary();
            return on_unary(token, src_id);
        case MULTIARY_VALUE:
            infer_unary();
            return on_multiary_value_before(token, src_id);
        case LEFT:
            infer_unary();
            return on_left_before(token, src_id);
        case RIGHT:
            infer_value();
            return on_right(token, src_id);
        case END_OF_FILE:
            infer_unary();
            return on_end_of_file(token, src_id);
        default:
            return new UnexpectedTokenErr();
        }
    }

    ErrPtr on_optional(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            return on_newline(token, src_id);
        case END_OF_FILE:
            return on_end_of_file(token, src_id);
        default:
            return on_before(token, src_id);
        }
    }

    ErrPtr on_end(Token& token, std::uint64_t src_id) {
        using enum TokenKind;

        switch(tokeninfo(token.id).kind) {
        case NEWLINE:
            return on_newline(token, src_id);
        case END_OF_FILE:
            return on_end_of_file(token, src_id);
        default:
            return new UnexpectedTokenErr();
        }
    }

    ErrPtr feed(Token& token, std::uint64_t src_id) override {
        switch(token.id) {
        case TokenID::HASH:
            // Ignore comments.
            return nullptr;
        case TokenID::SPACE:
            if (line_start) {
                line_start = false;
                return handle_leading_space(token.count, src_id);
            }
            // Ignore non-leading space.
            return nullptr;
        }

        ErrPtr res;
        if (line_start && token.id != TokenID::NEWLINE) {
            // Need to account for dedents when starting a new line without
            // leading spaces.
            res = handle_leading_space(0, src_id);
            if (res)
                return res;
        }

        using enum Orientation;
        switch(orientation) {
        case START:
            res = on_start(token, src_id);
        case BEFORE:
            res = on_before(token, src_id);
        case AFTER:
            res = on_after(token, src_id);
        case AFTER_BINARY_OR_POSTFIX:
            res = on_after_binary_or_postfix(token, src_id);
        case AFTER_UNARY_OR_VALUE:
            res = on_after_unary_or_value(token, src_id);
        case OPTIONAL:
            res = on_optional(token, src_id);
        case END:
            res = on_end(token, src_id);
        }
        if (token.id != TokenID::NEWLINE) {
            // Newlines, comments, and spaces are not stored to prev.
            // Since we return on comments and spaces above, just check for
            // newlines here.
            prev = std::move(token);
            prev_src = src_id;
            line_start = false;
        } else
            line_start = true;
        return res;
    }
    
    Op next() override {
        if (queue.empty())
            return Op(OpID::WAITING);
        Op op = queue.front();
        queue.pop();
        return op;
    }
};

}
