#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include <iostream>
#include <queue>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

#include "dl/data.hpp"
#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/parse/context.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/orientation.hpp"
#include "dl/parse/parserimpl.hpp"
#include "dl/parse/tokeninfo.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

#include "util.hpp"

using namespace dl;

using SourcedToken = std::pair<Token, Pos>;

TEST_CASE("Parser Core", "[parse]") {
    auto parser = ParserImpl();
    auto s1 = Pos(1, 1);
    auto s2 = Pos(2, 2);

    auto on_orientation = [&](auto token, Orientation orientation, Pos src) {
        parser.orientation = orientation;
        if constexpr (std::is_same_v<decltype(token), Token>)
            return parser.feed(std::move(token), src);
        else
            return parser.feed(Token(token), src);
    };

    auto on_orientation_op = [&](
        auto token, Orientation orientation, Pos src
    ) {
        ErrPtr err = on_orientation(token, orientation, src);
        if (err)
            return Res<Op>(std::move(err));
        if (parser.queue.empty())
            return Res<Op>(ErrPtr(new AssertionFailedErr("No ops pushed.")));
        if (parser.queue.size() > 1)
            return Res<Op>(ErrPtr(
                new AssertionFailedErr("Multiple ops pushed.")
            ));
        return Res<Op>(parser.next());
    };

    auto on_orientation_ops = [&](
        auto token, Orientation orientation, Pos src
    ) {
        ErrPtr err = on_orientation(token, orientation, src);
        if (err)
            return Res<std::deque<Op>>(std::move(err));
        std::deque<Op> res = parser.queue;
        parser.queue.clear();
        return Res<std::deque<Op>>(std::move(res));
    };

    auto on_start = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::START, src);
    };

    auto on_stmt = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::STMT, src);
    };

    auto on_before = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::BEFORE, src);
    };

    auto on_after = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::AFTER, src);
    };

    auto on_after_star = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::AFTER_STAR, src);
    };

    auto on_optional = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::OPTIONAL, src);
    };

    auto on_end = [&](auto token, Pos src) {
        return on_orientation(token, Orientation::END, src);
    };

    auto on_start_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::START, src);
    };

    auto on_stmt_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::STMT, src);
    };

    auto on_before_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::BEFORE, src);
    };

    auto on_after_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::AFTER, src);
    };

    auto on_after_star_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::AFTER_STAR, src);
    };

    auto on_optional_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::OPTIONAL, src);
    };

    auto on_end_op = [&](auto token, Pos src) {
        return on_orientation_op(token, Orientation::END, src);
    };

    auto on_start_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::START, src);
    };

    auto on_stmt_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::STMT, src);
    };

    auto on_before_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::BEFORE, src);
    };

    auto on_after_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::AFTER, src);
    };

    auto on_after_star_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::AFTER_STAR, src);
    };

    auto on_optional_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::OPTIONAL, src);
    };

    auto on_end_ops = [&](auto token, Pos src) {
        return on_orientation_ops(token, Orientation::END, src);
    };

    SECTION("ParserImpl Init") {
        REQUIRE(parser.depth == 0);
        REQUIRE(parser.line_start);
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.queue.empty());
        REQUIRE(parser.contexts.empty());
        REQUIRE_FALSE(parser.parsing_pred);
        REQUIRE_FALSE(parser.prev_was_no_pred_construct);
        REQUIRE_FALSE(parser.parsing_loop_vars);
    }

    SECTION("Hash") {
        REQUIRE(on_start(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_stmt(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_before(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_after(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_after_star(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_optional(TokenID::HASH, s1) == nullptr);
        REQUIRE(on_end(TokenID::HASH, s1) == nullptr);
        REQUIRE(parser.queue.empty());

        // Check that hash dedents if found at the beginning of a line in START
        // orientation.
        parser.depth = 2;
        parser.line_start = true;
        parser.contexts.push_back(Context::BLOCK);
        parser.contexts.push_back(Context::BLOCK);
        REQUIRE(
            on_start_ops(TokenID::HASH, s1) ==
            std::deque{
                Op(OpID::END, s1),
                Op(OpID::STMT, s1),
                Op(OpID::END, s1),
                Op(OpID::STMT, s1)
            }
        );
        REQUIRE(parser.orientation == Orientation::START);
    }

    SECTION("Newline") {
        parser.line_start = false;
        REQUIRE(on_start_ops(TokenID::NEWLINE, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.line_start);

        REQUIRE(on_stmt_ops(TokenID::NEWLINE, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::STMT);
        REQUIRE(parser.line_start);

        ErrPtr err = on_before(TokenID::NEWLINE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::NEWLINE, s1));

        parser.line_start = false;
        REQUIRE(on_after_op(TokenID::NEWLINE, s1) == Op(OpID::STMT, s1));
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.line_start);

        // AFTER_STAR orientation is impossible unless we are in brackets, which
        // is tested later.

        parser.line_start = false;
        REQUIRE(on_optional_ops(TokenID::NEWLINE, s1) ==
            std::deque{Op(OpID::NOTHING, s1), Op(OpID::STMT, s1)}
        );
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.line_start);

        parser.line_start = false;
        REQUIRE(on_end_op(TokenID::NEWLINE, s1) == Op(OpID::STMT, s1));
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.line_start);

        SECTION("In Brackets") {
            parser.contexts.push_back(Context::CURVED);
            // Note that START, OPTIONAL, and END orientations are impossible
            // inside brackets.
            parser.line_start = false;
            REQUIRE(on_before_ops(TokenID::NEWLINE, s1) == std::deque<Op>{});
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.line_start);

            parser.line_start = false;
            REQUIRE(on_after_ops(TokenID::NEWLINE, s1) == std::deque<Op>{});
            REQUIRE(parser.orientation == Orientation::AFTER);
            REQUIRE(parser.line_start);

            parser.line_start = false;
            REQUIRE(
                on_after_star_ops(TokenID::NEWLINE, s1) == std::deque<Op>{}
            );
            REQUIRE(parser.orientation == Orientation::AFTER_STAR);
            REQUIRE(parser.line_start);
        }

        SECTION("After Construct End") {
            // This section tests that a construct is ended in the AFTER,
            // OPTIONAL, and END orientations when a newline is found when in
            // the CONSTRUCT_END context.
            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_after_ops(TokenID::NEWLINE, s1) ==
                std::deque<Op>{
                    Op(OpID::STMT, s1), Op(OpID::END, s1), Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_optional_ops(TokenID::NEWLINE, s1) ==
                std::deque<Op>{
                    Op(OpID::NOTHING, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_end_ops(TokenID::NEWLINE, s1) ==
                std::deque<Op>{
                    Op(OpID::STMT, s1), Op(OpID::END, s1), Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());
        }
    }

    SECTION("Space") {
        SECTION("Non-leading") {
            // Non-leading space should be ignored.
            parser.line_start = false;
            REQUIRE(
                on_start(Token(TokenID::SPACE, std::uint32_t(1)), s1) == nullptr
            );
            REQUIRE(
                on_stmt(Token(TokenID::SPACE, std::uint32_t(2)), s1) == nullptr
            );
            REQUIRE(
                on_before(
                    Token(TokenID::SPACE, std::uint32_t(3)), s1
                ) == nullptr
            );
            REQUIRE(
                on_after(
                    Token(TokenID::SPACE, std::uint32_t(4)), s1
                ) == nullptr
            );
            REQUIRE(
                on_after_star(Token(TokenID::SPACE, std::uint32_t(5)), s1) ==
                nullptr
            );
            REQUIRE(on_optional(
                Token(TokenID::SPACE, std::uint32_t(6)), s1
            ) == nullptr);
            REQUIRE(
                on_end(Token(TokenID::SPACE, std::uint32_t(7)), s1) == nullptr
            );
            REQUIRE(parser.queue.empty());
        }

        SECTION("Leading, Start Orientation") {
            // First, try a number of spaces which is not a multiple of 4 in
            // start orientation.
            ErrPtr err = on_start(Token(TokenID::SPACE, std::uint32_t(3)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == IncompleteIndentErr(s1));

            // Now try with 4 spaces (over-indent)
            parser.line_start = true;
            err = on_start(Token(TokenID::SPACE, std::uint32_t(4)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == OverIndentErr(s1));

            // Set depth to 1 and try again with 4 spaces.
            parser.line_start = true;
            parser.depth = 1;
            parser.contexts.push_back(Context::BLOCK);
            REQUIRE(
                on_start(Token(TokenID::SPACE, std::uint32_t(4)), s1) == nullptr
            );
            REQUIRE(parser.depth == 1);
            REQUIRE(parser.contexts == std::vector{Context::BLOCK});
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.orientation == Orientation::START);

            // Set depth to 2 and try again with 4 spaces (dedent).
            parser.line_start = true;
            parser.depth = 2;
            parser.contexts.push_back(Context::BLOCK);
            REQUIRE(
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(4)), s1)
                == std::deque{Op(OpID::END, s1), Op(OpID::STMT, s1)}
            );
            REQUIRE(parser.depth == 1);
            REQUIRE(parser.contexts == std::vector{Context::BLOCK});
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.orientation == Orientation::START);

            // 12 spaces with a depth of 6 (triple dedent).
            parser.line_start = true;
            parser.depth = 6;
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            REQUIRE(
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(12)), s1)
                == std::deque{
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                }
            );
            REQUIRE(parser.depth == 3);
            REQUIRE(
                parser.contexts ==
                std::vector{Context::BLOCK, Context::BLOCK, Context::BLOCK}
            );
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.orientation == Orientation::START);

            // Again, but mix-in some construct contexts to make sure dedent
            // closes them too.
            parser.line_start = true;
            parser.depth = 6;
            parser.contexts.push_back(Context::CONSTRUCT_END);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::CONSTRUCT);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(12)), s1)
                == std::deque{
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                }
            );
            REQUIRE(parser.depth == 3);
            REQUIRE(
                parser.contexts ==
                std::vector{Context::BLOCK, Context::BLOCK, Context::BLOCK}
            );
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.orientation == Orientation::START);

            // Full dedent: check that parsing a valid start-orientation token
            // which is not newline or space results in a full dedent.
            parser.line_start = true;
            REQUIRE(
                on_start_ops(TokenID::TRUE, s1) ==
                std::deque{
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::TRUE, s1)
                }
            );
            REQUIRE(parser.depth == 0);
            REQUIRE(parser.contexts.empty());
        }

        SECTION("Leading, Statement Orientation") {
            // When encountering leading space in the STMT orientation, the
            // parser expects an indent to occur.

            parser.line_start = true;
            REQUIRE(
                on_stmt_op(Token(TokenID::SPACE, std::uint32_t(4)), s1)
                == Op(OpID::BLOCK, s1)
            );
            REQUIRE(parser.depth == 1);
            REQUIRE(parser.orientation == Orientation::START);
            REQUIRE(
                parser.contexts == std::vector{Context::BLOCK}
            );
            REQUIRE_FALSE(parser.line_start);

            // Try indenting again.
            parser.line_start = true;
            REQUIRE(
                on_stmt_op(Token(TokenID::SPACE, std::uint32_t(8)), s1)
                == Op(OpID::BLOCK, s1)
            );
            REQUIRE(parser.depth == 2);
            REQUIRE(parser.orientation == Orientation::START);
            REQUIRE(
                parser.contexts == std::vector{Context::BLOCK, Context::BLOCK}
            );
            REQUIRE_FALSE(parser.line_start);

            // Now try to dedent once, which is an err in this situation.
            parser.line_start = true;
            parser.line_start = true;
            ErrPtr err = on_stmt(Token(TokenID::SPACE, std::uint32_t(4)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnderIndentErr(s1));

            // Now try with exact space (note that an indent is expected, not
            // a continuation of the same indentation).
            parser.line_start = true;
            err = on_stmt(Token(TokenID::SPACE, std::uint32_t(8)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == ExpectedIndentErr(s1));

            // Now try with less space than another indent.
            parser.line_start = true;
            err = on_stmt(Token(TokenID::SPACE, std::uint32_t(10)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == IncompleteIndentErr(s1));

            // Now try to indent twice, which is an error.
            parser.line_start = true;
            err = on_stmt(Token(TokenID::SPACE, std::uint32_t(16)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == OverIndentErr(s1));
        }

        SECTION("Leading, Other Orientations") {
            // Note that leading spaces cannot occur for OPTIONAL or END
            // orientations.

            // First, check that under-indenting results in an error, as we may
            // only dedent in START orientation.
            parser.depth = 2;
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);

            ErrPtr err = on_before(Token(TokenID::SPACE, std::uint32_t(2)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnderIndentErr(s1));

            parser.line_start = true;
            err = on_after(Token(TokenID::SPACE, std::uint32_t(4)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnderIndentErr(s1));

            parser.line_start = true;
            err = on_after_star(Token(TokenID::SPACE, std::uint32_t(6)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnderIndentErr(s1));

            // Now check that at least the needed amount of space will result in
            // no error and no depth increase.
            parser.line_start = true;
            REQUIRE(on_before(
                Token(TokenID::SPACE, std::uint32_t(8)), s1
            ) == nullptr);
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.depth == 2);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            parser.line_start = true;
            REQUIRE(on_after(
                Token(TokenID::SPACE, std::uint32_t(10)), s1
            ) == nullptr);
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.depth == 2);
            REQUIRE(parser.orientation == Orientation::AFTER);

            parser.line_start = true;
            REQUIRE(
                on_after_star(Token(TokenID::SPACE, std::uint32_t(12)), s1) ==
                nullptr
            );
            REQUIRE_FALSE(parser.line_start);
            REQUIRE(parser.depth == 2);
            REQUIRE(parser.orientation == Orientation::AFTER_STAR);
        }
    }

    SECTION("End Of File") {
        // End of file is similar to a newline, but with 2 key differences:
        // 1. If found in brackets, an UnclosedBracketErr occurs.
        // 2. Code is fully dedented upon finding an EOF.

        // Also, because we are finished parsing after finding an EOF, the
        // parser state need only be checked to ensure it is setup well for the
        // next assertion: it's not any more "correct" for the parser to be in
        // one or another state if it's not going to be used anymore.

        parser.line_start = false;
        REQUIRE(on_start_ops(TokenID::END_OF_FILE, s1) == std::deque<Op>{});

        ErrPtr err = on_before(TokenID::END_OF_FILE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::END_OF_FILE, s1));

        REQUIRE(on_after_op(TokenID::END_OF_FILE, s1) == Op(OpID::STMT, s1));

        REQUIRE(
            on_optional_ops(TokenID::END_OF_FILE, s1) ==
            std::deque{Op(OpID::NOTHING, s1), Op(OpID::STMT, s1)}
        );

        REQUIRE(on_end_op(TokenID::END_OF_FILE, s1) == Op(OpID::STMT, s1));

        SECTION("In Brackets") {
            parser.contexts.push_back(Context::CURVED);
            // Note that START, OPTIONAL, and END orientations are impossible
            // inside brackets.
            ErrPtr err = on_before(TokenID::END_OF_FILE, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnclosedBracketErr(s1));

            err = on_after(TokenID::END_OF_FILE, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnclosedBracketErr(s1));

            err = on_after_star(TokenID::END_OF_FILE, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnclosedBracketErr(s1));
        }

        SECTION("Dedent") {
            parser.depth = 2;
            parser.line_start = false;
            parser.contexts.push_back(Context::BLOCK);
            parser.contexts.push_back(Context::BLOCK);
            REQUIRE(
                on_start_ops(TokenID::END_OF_FILE, s1) ==
                std::deque{
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                }
            );
        }

        SECTION("After Construct End") {
            // This section tests that a construct is ended in the AFTER,
            // OPTIONAL, and END orientations when a newline is found when in
            // the CONSTRUCT_END context.
            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_after_ops(TokenID::END_OF_FILE, s1) ==
                std::deque<Op>{
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_optional_ops(TokenID::END_OF_FILE, s1) ==
                std::deque<Op>{
                    Op(OpID::NOTHING, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_end_ops(TokenID::END_OF_FILE, s1) ==
                std::deque<Op>{
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                }
            );
            REQUIRE(parser.contexts.empty());
        }
    }

    SECTION("Nullary Tokens") {
        for (TokenID t: std::vector{TokenID::BREAK, TokenID::CONTINUE}) {
            REQUIRE(on_start_op(t, s1) == Op(tokeninfo(t).op1, s1));
            REQUIRE(parser.orientation == Orientation::END);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            REQUIRE(on_stmt_op(t, s1) == Op(tokeninfo(t).op1, s1));
            REQUIRE(parser.orientation == Orientation::END);

            ErrPtr err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after_star(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Unary Tokens") {
        for (TokenID t: std::vector{TokenID::TILDE}) {
            Op op = Op(tokeninfo(t).op1, s1);
            REQUIRE(on_start_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            REQUIRE(on_stmt_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            REQUIRE(on_before_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            ErrPtr err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            // Confirm that "expansion" is inferred in AFTER_STAR orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(t, s1) ==
                std::deque{Op(OpID::UNPACK_ARGS, s2), op}
            );

            REQUIRE(on_optional_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Binary Tokens") {
        for (TokenID t: std::vector{
            TokenID::AMPERSAND,
            TokenID::AMPERSAND_EQUALS,
            TokenID::AND,
            TokenID::BANG_EQUALS,
            TokenID::CAROT,
            TokenID::CAROT_EQUALS,
            TokenID::COMMA,
            TokenID::DOUBLE_COLON,
            TokenID::DOUBLE_EQUALS,
            TokenID::DOUBLE_LEFT_ANGLE,
            TokenID::DOUBLE_LEFT_ANGLE_EQUALS,
            TokenID::DOUBLE_RIGHT_ANGLE,
            TokenID::DOUBLE_RIGHT_ANGLE_EQUALS,
            TokenID::DOUBLE_STAR_EQUALS,
            TokenID::EQUALS,
            TokenID::IN,
            TokenID::LEFT_ANGLE,
            TokenID::LEFT_ANGLE_EQUALS,
            TokenID::MINUS_EQUALS,
            TokenID::MINUS_RIGHT_ANGLE,
            TokenID::OR,
            TokenID::PERCENT_EQUALS,
            TokenID::PIPE,
            TokenID::PIPE_EQUALS,
            TokenID::PLUS,
            TokenID::PLUS_EQUALS,
            TokenID::RIGHT_ANGLE,
            TokenID::RIGHT_ANGLE_EQUALS,
            TokenID::SLASH,
            TokenID::SLASH_EQUALS,
            TokenID::STAR_EQUALS
        }) {
            Op op = Op(tokeninfo(t).op2, s1);

            ErrPtr err = on_start(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_stmt(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            REQUIRE(on_after_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            // Special case: COMMA is the only pure binary operator allowed in
            // AFTER_STAR orientation, after which star is inferred as an
            // argument separator.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            if (t == TokenID::COMMA) {
                REQUIRE(on_after_star_ops(t, s1) == std::deque{
                    Op(OpID::POS_KW_SEP, s2), op
                });
                REQUIRE(parser.orientation == Orientation::BEFORE);
            } else {
                err = on_after_star(t, s1);
                REQUIRE(err != nullptr);
                REQUIRE(*err == UnexpectedTokenErr(t, s1));
            }

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }

        // Special case: when in brackets and in AFTER orientation, the equals
        // (=) token should result in the BIND operator being emitted.
        parser.contexts.push_back(Context::CURVED);
        REQUIRE(on_after_op(TokenID::EQUALS, s1) == Op(OpID::BIND, s1));
    }

    SECTION("Multiary Tokens") {
        for (TokenID t: std::vector{
            TokenID::COLON,
            TokenID::DOUBLE_STAR,
            TokenID::DOT,
            TokenID::MINUS,
            TokenID::PERCENT,
            TokenID::STAR
        }) {
            Op unary_op = Op(tokeninfo(t).op1, s1);
            Op binary_op = Op(tokeninfo(t).op2, s1);

            REQUIRE(on_start_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            REQUIRE(on_stmt_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            REQUIRE(on_before_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            REQUIRE(on_after_op(t, s1) == binary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            // Check that expansion is inferred in AFTER_STAR orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(t, s1) ==
                std::deque{Op(OpID::UNPACK_ARGS, s2), unary_op}
            );
            REQUIRE(parser.orientation == Orientation::BEFORE);

            REQUIRE(on_optional_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            ErrPtr err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }

        // Special case: check that when in brackets, the STAR operator sends
        // the parser to the AFTER_STAR orientation if previously in the
        // BEFORE, START, or OPTIONAL orientations.
        // Only BEFORE will be checked as the code trivially defers to BEFORE
        // logic when in START or OPTIONAL.
        parser.contexts.push_back(Context::CURVED);
        REQUIRE(on_before_ops(TokenID::STAR, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::AFTER_STAR);
    }

    SECTION("Value Tokens") {
        for (Token t: std::vector{
            Token(TokenID::CHAR, std::uint32_t(100)),
            Token(TokenID::FALSE),
            Token(TokenID::ID, "asdf"),
            Token(TokenID::NONE),
            Token(TokenID::NULL_),
            Token(TokenID::PLAIN_INT, std::int32_t(1000)),
            Token(TokenID::NUMBER, std::int32_t(1000)),
            Token(TokenID::STRING, "a,s,d,f"),
            Token(TokenID::THIS),
            Token(TokenID::TRUE),
            Token(TokenID::VARS)
        }) {
            Data d = t.data;

            // op1 is the "value" operator.
            Op op1 = Op(tokeninfo(t.id).op1, std::move(d), s1);

            // op2 is the "invisible" binary operator associated with this
            // value.
            Op op2 = Op(tokeninfo(t.id).op2, s1);

            // Data from the token will be moved, copy it for each test.
            d = t.data;
            REQUIRE(on_start_op(Token(t.id, std::move(d)), s1) == op1);
            REQUIRE(parser.orientation == Orientation::AFTER);
            REQUIRE(parser.prev == t.id);
            REQUIRE(parser.prev_src == s1);

            d = t.data;
            REQUIRE(on_stmt_op(Token(t.id, std::move(d)), s1) == op1);
            REQUIRE(parser.orientation == Orientation::AFTER);

            d = t.data;
            REQUIRE(on_before_op(Token(t.id, std::move(d)), s1) == op1);
            REQUIRE(parser.orientation == Orientation::AFTER);

            d = t.data;
            REQUIRE(
                on_after_ops(Token(t.id, std::move(d)), s1) ==
                std::deque{op2, op1}
            );
            REQUIRE(parser.orientation == Orientation::AFTER);

            // Check that expansion is inferred in AFTER_STAR orientation.
            d = t.data;
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(Token(t.id, std::move(d)), s1) ==
                std::deque{Op(OpID::UNPACK_ARGS, s2), op1}
            );
            REQUIRE(parser.orientation == Orientation::AFTER);

            d = t.data;
            REQUIRE(on_optional_op(Token(t.id, std::move(d)), s1) == op1);
            REQUIRE(parser.orientation == Orientation::AFTER);

            ErrPtr err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t.id, s1));
        }
    }

    SECTION("Left Tokens") {
        for (
            TokenID t: std::vector{
                TokenID::LEFT_CURLY, TokenID::LEFT_CURVED, TokenID::LEFT_SQUARE
            }
        ) {
            // op1 is the "value" operator.
            // op2 is the "invisible" binary operator associated with this left
            // token (currently always CALL).
            Op op1 = Op(tokeninfo(t).op1, s1);
            Op op2 = Op(tokeninfo(t).op2, s1);
            Context ctx = tokeninfo(t).match;

            REQUIRE(on_start_op(t, s1) == op1);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_stmt_op(t, s1) == op1);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_before_op(t, s1) == op1);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_after_ops(t, s1) == std::deque{op2, op1});
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            // Check that expansion is inferred in AFTER_STAR orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(t, s1) ==
                std::deque{Op(OpID::UNPACK_ARGS, s2), op1}
            );
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_optional_op(t, s1) == op1);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            ErrPtr err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Right Tokens") {
        for (
            TokenID t: std::vector{
                TokenID::RIGHT_CURLY,
                TokenID::RIGHT_CURVED,
                TokenID::RIGHT_SQUARE
            }
        ) {
            // Right tokens always emit the END operator.
            Op op = Op(OpID::END, s1);
            Context ctx = tokeninfo(t).match;

            ErrPtr err = on_start(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_stmt(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            // Should result in UnexpectedTokenErr in BEFORE orientation UNLESS
            // the previous operator is the corresponding LEFT token.
            err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            parser.contexts.push_back(ctx);
            switch(t) {
            case TokenID::RIGHT_CURVED:
                parser.prev = TokenID::LEFT_CURVED;
                break;
            case TokenID::RIGHT_SQUARE:
                parser.prev = TokenID::LEFT_SQUARE;
                break;
            case TokenID::RIGHT_CURLY:
                parser.prev = TokenID::LEFT_CURLY;
                break;
            default:
                REQUIRE(false);
            }

            // Here, the NOTHING operator is emitted to signal empty contents,
            // as usually the contents of brackets are interpreted as a single
            // value.
            REQUIRE(
                on_before_ops(t, s1) == std::deque{Op(OpID::NOTHING, s1), op}
            );

            parser.contexts.push_back(ctx);
            REQUIRE(on_after_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::AFTER);
            REQUIRE(parser.contexts.empty());
            REQUIRE(parser.prev == t);

            // Check that we get an UnexpectedTokenErr is we feed a right token
            // when the context stack is empty in AFTER orientation.
            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            // Check that argument separator is inferred in AFTER_STAR
            // orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            parser.contexts.push_back(ctx);
            REQUIRE(
                on_after_star_ops(t, s1) ==
                std::deque{Op(OpID::POS_KW_SEP, s2), op}
            );
            REQUIRE(parser.orientation == Orientation::AFTER);
            REQUIRE(parser.contexts.empty());

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }

        // Check that a mismatched context results in an UnexpectedTokenErr.
        parser.contexts.push_back(Context::CURVED);
        ErrPtr err = on_after(TokenID::RIGHT_SQUARE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::RIGHT_SQUARE, s1));
    }

    SECTION("Construct First Tokens") {
        for (
            TokenID t: 
            std::vector{
                TokenID::DEF,
                TokenID::FOR,
                TokenID::MATCH,
                TokenID::TRY,
                TokenID::TYPE,
                TokenID::WHILE
            }
        ) {
            parser.parsing_pred = false;
            parser.prev_was_no_pred_construct = false;

            // op is the "unary" operator corresponding to the construct.
            Op op = Op(tokeninfo(t).op1, s1);

            REQUIRE(
                on_start_ops(t, s1) == std::deque{Op(OpID::CONSTRUCT, s1), op}
            );
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT});
            REQUIRE(parser.prev_was_no_pred_construct == (t == TokenID::TRY));
            REQUIRE(parser.parsing_pred);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);
            parser.contexts.clear();

            // START is the only valid orientation for CONSTRUCT_FIRST.
            ErrPtr err = on_stmt(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after_star(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }

        // Check that if we are in the CONSTRUCT context and a token which is
        // neither a CONSTRUCT_MIDDLE, CONSTRUCT_LAST, or SPECIAL token is
        // received, then the construct is closed.
        parser.contexts.push_back(Context::CONSTRUCT);
        REQUIRE(on_start_ops(Token(TokenID::ID, "asdf"), s1) == std::deque{
            Op(OpID::END, s1),
            Op(OpID::STMT, s1),
            Op(OpID::ID, "asdf", s1)
        });
        REQUIRE(parser.contexts.empty());
    }

    SECTION("Construct Middle Tokens") {
        for(
            TokenID t: std::vector{
                TokenID::CASE, TokenID::ELIF, TokenID::EXCEPT
            }
        ) {
            parser.contexts.clear();
            parser.parsing_pred = false;

            // op is the "unary" operator corresponding to the construct middle.
            Op op = Op(tokeninfo(t).op1, s1);

            // Check that we get an UnexpectedTokenErr if Context::CONSTRUCT is
            // not on the context stack.
            ErrPtr err = on_start(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            parser.contexts.push_back(Context::CONSTRUCT);
            REQUIRE(on_start_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.parsing_pred);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            // START is the only valid orientation for CONSTRUCT_MIDDLE.
            err = on_stmt(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after_star(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Construct Last Tokens") {
        for(TokenID t: std::vector{TokenID::FINALLY}) {
            parser.contexts.clear();
            parser.parsing_pred = false;
            parser.prev_was_no_pred_construct = false;

            // op is the "unary" operator corresponding to the construct last.
            Op op = Op(tokeninfo(t).op1, s1);

            // Check that we get an UnexpectedTokenErr if Context::CONSTRUCT is
            // not on the context stack.
            ErrPtr err = on_start(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            parser.contexts.push_back(Context::CONSTRUCT);
            REQUIRE(on_start_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(
                parser.prev_was_no_pred_construct == (t == TokenID::FINALLY)
            );
            REQUIRE(parser.parsing_pred);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);
            REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT_END});

            // START is the only valid orientation for CONSTRUCT_LAST.
            err = on_stmt(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after_star(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Statement with Optional Value") {
        for(TokenID t: std::vector{TokenID::RAISE, TokenID::RETURN}) {
            Op op = Op(tokeninfo(t).op1, s1);
            REQUIRE(on_start_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::OPTIONAL);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            REQUIRE(on_stmt_op(t, s1) == op);
            REQUIRE(parser.orientation == Orientation::OPTIONAL);

            ErrPtr err = on_before(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_after_star(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_optional(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));

            err = on_end(t, s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnexpectedTokenErr(t, s1));
        }
    }

    SECTION("Loop Variables") {
        // Test that commas separating loop variables use the higher precedence
        // operator LOOP_VAR_SEP.
        REQUIRE(!parser.parsing_loop_vars);
        REQUIRE(on_start_ops(TokenID::FOR, s1) == std::deque<Op>{
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1)
        });
        REQUIRE(parser.parsing_loop_vars);

        REQUIRE(
            on_before_op(Token(TokenID::ID, "x"), s1) == Op(OpID::ID, "x", s1)
        );
        REQUIRE(parser.parsing_loop_vars);
        
        REQUIRE(on_after_op(TokenID::COMMA, s1) == Op(OpID::LOOP_VAR_SEP, s1));
        REQUIRE(parser.parsing_loop_vars);

        REQUIRE(
            on_before_op(Token(TokenID::ID, "y"), s1) == Op(OpID::ID, "y", s1)
        );
        REQUIRE(parser.parsing_loop_vars);
        
        REQUIRE(on_after_op(TokenID::COMMA, s1) == Op(OpID::LOOP_VAR_SEP, s1));
        REQUIRE(parser.parsing_loop_vars);

        REQUIRE(
            on_before_op(Token(TokenID::ID, "z"), s1) == Op(OpID::ID, "z", s1)
        );
        REQUIRE(parser.parsing_loop_vars);

        REQUIRE(on_after_op(TokenID::IN, s1) == Op(OpID::IN, s1));
        // Now that neither a variable or comma has been encountered,
        // the `parsing_loop_vars` flag should be off.
        REQUIRE(!parser.parsing_loop_vars);
    }

    SECTION("At Token") {
        // @ is the only dual-affix (prefix and postfix) unary token.
        REQUIRE(on_start_op(TokenID::AT, s1) == Op(OpID::ADDR, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.prev == TokenID::AT);
        REQUIRE(parser.prev_src == s1);

        REQUIRE(on_before_op(TokenID::AT, s1) == Op(OpID::ADDR, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        REQUIRE(on_after_op(TokenID::AT, s1) == Op(OpID::ADDR_TYPE, s1));
        REQUIRE(parser.orientation == Orientation::AFTER);

        // Not expected in the AFTER_STAR orientation.
        ErrPtr err = on_after_star(TokenID::AT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::AT, s1));

        REQUIRE(on_optional_op(TokenID::AT, s1) == Op(OpID::ADDR, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        err = on_end(TokenID::AT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::AT, s1));
    }

    SECTION("Colon Special Cases") {
        // When parser.prev_was_no_pred_construct, colon should move from BEFORE
        // to STMT and emit the BODY operator.
        parser.prev_was_no_pred_construct = true;
        parser.parsing_pred = true;

        REQUIRE(on_before_op(TokenID::COLON, s1) == Op(OpID::BODY, s1));
        REQUIRE(parser.orientation == Orientation::STMT);
        REQUIRE_FALSE(parser.prev_was_no_pred_construct);
        REQUIRE_FALSE(parser.parsing_pred);

        // When parser.parsing_pred, colon should move from AFTER to STMT and
        // emit the LABEL operator.
        parser.parsing_pred = true;
        REQUIRE(on_after_op(TokenID::COLON, s1) == Op(OpID::LABEL, s1));
        REQUIRE(parser.orientation == Orientation::STMT);
        REQUIRE_FALSE(parser.parsing_pred);

        // However, when in brackets, the colon operator should work as usual.
        parser.parsing_pred = true;
        parser.contexts.push_back(Context::CURVED);
        REQUIRE(on_after_op(TokenID::COLON, s1) == Op(OpID::TYPE_LABEL, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.parsing_pred);
    }

    SECTION("Else Token") {
        // "else" is the only token of kind CONSTRUCT_LAST_OR_BINARY.
        // Check that we get an UnexpectedTokenErr if Context::CONSTRUCT is
        // not on the context stack.
        ErrPtr err = on_start(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));

        parser.contexts.push_back(Context::CONSTRUCT);
        REQUIRE(on_start_op(TokenID::ELSE, s1) == Op(OpID::ELSE, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.prev_was_no_pred_construct);
        REQUIRE(parser.parsing_pred);
        REQUIRE(parser.prev == TokenID::ELSE);
        REQUIRE(parser.prev_src == s1);
        REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT_END});
        parser.contexts.clear();

        err = on_stmt(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));

        err = on_before(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));

        REQUIRE(on_after_op(TokenID::ELSE, s1) == Op(OpID::TERNARY_ELSE, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        err = on_after_star(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));

        err = on_optional(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));

        err = on_end(TokenID::ELSE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::ELSE, s1));
    }

    SECTION("If Token") {
        // "if" is the only token of kind CONSTRUCT_FIRST_OR_BINARY.

        REQUIRE(
            on_start_ops(TokenID::IF, s1) == 
            std::deque{Op(OpID::CONSTRUCT, s1), Op(OpID::IF, s1)}
        );
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT});
        REQUIRE(parser.parsing_pred);
        REQUIRE(parser.prev == TokenID::IF);
        REQUIRE(parser.prev_src == s1);
        parser.contexts.clear();

        ErrPtr err = on_stmt(TokenID::IF, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::IF, s1));

        err = on_before(TokenID::IF, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::IF, s1));

        REQUIRE(on_after_op(TokenID::IF, s1) == Op(OpID::TERNARY_IF, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        err = on_after_star(TokenID::IF, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::IF, s1));

        err = on_optional(TokenID::IF, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::IF, s1));

        err = on_end(TokenID::IF, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::IF, s1));
    }

    SECTION("Not Token") {
        // "not" token is a special case, as it corresponds to a unary not
        // operator *unless* it is followed by "in", in which case the unary
        // operator "not in" is the result.
        REQUIRE(on_start_ops(TokenID::NOT, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);
        REQUIRE(parser.prev == TokenID::NOT);
        REQUIRE(parser.prev_src == s1);

        // Check that unary not operator is emitted if a value is fed.
        REQUIRE(parser.feed(Token(TokenID::TRUE), s2) == nullptr);
        REQUIRE(
            parser.queue == std::deque{Op(OpID::NOT, s1), Op(OpID::TRUE, s2)}
        );
        REQUIRE(parser.orientation == Orientation::AFTER);
        parser.queue.clear();

        REQUIRE(on_before_ops(TokenID::NOT, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);

        // Check that "not in" operator is emitted if "in" token is fed.
        REQUIRE(parser.feed(Token(TokenID::IN), s2) == nullptr);
        REQUIRE(parser.queue == std::deque{Op(OpID::NOT_IN, s1)});
        REQUIRE(parser.orientation == Orientation::BEFORE);
        parser.queue.clear();

        ErrPtr err = on_after(TokenID::NOT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::NOT, s1));

        REQUIRE(on_optional_ops(TokenID::NOT, s1) == std::deque<Op>{});
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);

        err = on_end(TokenID::NOT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::NOT, s1));
    }
}

TEST_CASE("Parser Input/Output", "[parse]") {
    // These tests verify the expected output of operators for code which is
    // valid at the parsing step given a sequence of input tokens. Such code may
    // not be valid at a later step. Space tokens that are not indenting are
    // omitted for brevity, as the parser never requires a space separator even
    // if it is technically necessary to lex properly (i.e., a and b must have
    // spaces to prevent it from being lexxed as aandb).
    
    auto feed_all = [](
        std::vector<SourcedToken> tokens
    ) -> Res<std::vector<Op>> {
        auto parser = ParserImpl();
        for (SourcedToken& x: tokens) {
            ErrPtr err = parser.feed(std::move(x.first), x.second);
            if (err)
                return err;
        }
        ErrPtr err = parser.feed(Token(TokenID::END_OF_FILE), s_end);
        if (err)
            return err;
        std::vector<Op> ops;
        for (std::uint32_t i = 0; i < parser.queue.size() - 1; i++)
            ops.push_back(std::move(parser.queue[i]));
        if (parser.queue.back() != Op(OpID::STMT, s_end))
            return ErrPtr(new AssertionFailedErr("No ending statement"));
        return ops;
    };

    SECTION("ADD") {
        // a + b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PLUS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ADD, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Addr") {
        // @a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::AT), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::ADDR, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("AddrType") {
        // Int@
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "Int"), s1},
            SourcedToken{Token(TokenID::AT), s2}
        )) == vec(
            Op(OpID::ID, "Int", s1),
            Op(OpID::ADDR_TYPE, s2)
        ));
    }

    SECTION("And") {
        // true and false
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRUE), s1},
            SourcedToken{Token(TokenID::AND), s2},
            SourcedToken{Token(TokenID::FALSE), s3}
        )) == vec(
            Op(OpID::TRUE, s1),
            Op(OpID::AND, s2),
            Op(OpID::FALSE, s3)
        ));
    }

    SECTION("Assign") {
        // a = b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("BAND") {
        // a & b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::AMPERSAND), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BAND, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("BNOT") {
        // ~a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TILDE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::BNOT, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("BOR") {
        // a | b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PIPE), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BOR, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("BXOR") {
        // a ^ b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::CAROT), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BXOR, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Break") {
        // break
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::BREAK), s1}
        )) == vec(
            Op(OpID::BREAK, s1)
        ));
    }

    SECTION("Cached Call") {
        // Vector[Int]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "Vector"), s1},
            SourcedToken{Token(TokenID::LEFT_SQUARE), s2},
            SourcedToken{Token(TokenID::ID, "Int"), s3},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s4}
        )) == vec(
            Op(OpID::ID, "Vector", s1),
            Op(OpID::CALL, s2),
            Op(OpID::LIST, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::END, s4)
        ));
    }

    SECTION("Cached CallAttr") {
        // obj.attr[T]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "obj"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::ID, "attr"), s3},
            SourcedToken{Token(TokenID::LEFT_SQUARE), s4},
            SourcedToken{Token(TokenID::ID, "T"), s5},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s6}
        )) == vec(
            Op(OpID::ID, "obj", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "attr", s3),
            Op(OpID::CALL, s4),
            Op(OpID::LIST, s4),
            Op(OpID::ID, "T", s5),
            Op(OpID::END, s6)
        ));
    }

    SECTION("Call") {
        // fn()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s3}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::NOTHING, s3),
            Op(OpID::END, s3)
        ));

        // fn(1, 2)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, std::int32_t(2), s5),
            Op(OpID::END, s6)
        ));

        // fn(1, 2, *args)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::STAR), s7},
            SourcedToken{Token(TokenID::ID, "args"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s9}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, std::int32_t(2), s5),
            Op(OpID::SEP, s6),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::END, s9)
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::STAR), s7},
            SourcedToken{Token(TokenID::ID, "args"), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::ID, "kw1"), s10},
            SourcedToken{Token(TokenID::EQUALS), s11},
            SourcedToken{Token(TokenID::STRING, "yes"), s12},
            SourcedToken{Token(TokenID::COMMA), s13},
            SourcedToken{Token(TokenID::ID, "kw2"), s14},
            SourcedToken{Token(TokenID::EQUALS), s15},
            SourcedToken{Token(TokenID::TRUE), s16},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s17}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, std::int32_t(2), s5),
            Op(OpID::SEP, s6),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::SEP, s9),
            Op(OpID::ID, "kw1", s10),
            Op(OpID::BIND, s11),
            Op(OpID::STRING, "yes", s12),
            Op(OpID::SEP, s13),
            Op(OpID::ID, "kw2", s14),
            Op(OpID::BIND, s15),
            Op(OpID::TRUE, s16),
            Op(OpID::END, s17)
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true, **kwargs)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::STAR), s7},
            SourcedToken{Token(TokenID::ID, "args"), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::ID, "kw1"), s10},
            SourcedToken{Token(TokenID::EQUALS), s11},
            SourcedToken{Token(TokenID::STRING, "yes"), s12},
            SourcedToken{Token(TokenID::COMMA), s13},
            SourcedToken{Token(TokenID::ID, "kw2"), s14},
            SourcedToken{Token(TokenID::EQUALS), s15},
            SourcedToken{Token(TokenID::TRUE), s16},
            SourcedToken{Token(TokenID::COMMA), s17},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s18},
            SourcedToken{Token(TokenID::ID, "kwargs"), s19},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s20}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, std::int32_t(2), s5),
            Op(OpID::SEP, s6),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::SEP, s9),
            Op(OpID::ID, "kw1", s10),
            Op(OpID::BIND, s11),
            Op(OpID::STRING, "yes", s12),
            Op(OpID::SEP, s13),
            Op(OpID::ID, "kw2", s14),
            Op(OpID::BIND, s15),
            Op(OpID::TRUE, s16),
            Op(OpID::SEP, s17),
            Op(OpID::UNPACK_KWARGS, s18),
            Op(OpID::ID, "kwargs", s19),
            Op(OpID::END, s20)
        ));

        // fn(1, kw1="yes", 2)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::ID, "kw1"), s5},
            SourcedToken{Token(TokenID::EQUALS), s6},
            SourcedToken{Token(TokenID::STRING, "yes"), s7},
            SourcedToken{Token(TokenID::COMMA), s8},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s9},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s10}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::ID, "kw1", s5),
            Op(OpID::BIND, s6),
            Op(OpID::STRING, "yes", s7),
            Op(OpID::SEP, s8),
            Op(OpID::PLAIN_INT, std::int32_t(2), s9),
            Op(OpID::END, s10)
        ));

        // fn(1, kw1="yes", kw1=true)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "fn"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::ID, "kw1"), s5},
            SourcedToken{Token(TokenID::EQUALS), s6},
            SourcedToken{Token(TokenID::STRING, "yes"), s7},
            SourcedToken{Token(TokenID::COMMA), s8},
            SourcedToken{Token(TokenID::ID, "kw1"), s9},
            SourcedToken{Token(TokenID::EQUALS), s10},
            SourcedToken{Token(TokenID::TRUE), s11},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s12}
        )) == vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, std::int32_t(1), s3),
            Op(OpID::SEP, s4),
            Op(OpID::ID, "kw1", s5),
            Op(OpID::BIND, s6),
            Op(OpID::STRING, "yes", s7),
            Op(OpID::SEP, s8),
            Op(OpID::ID, "kw1", s9),
            Op(OpID::BIND, s10),
            Op(OpID::TRUE, s11),
            Op(OpID::END, s12)
        ));
    }

    SECTION("CallAttr") {
        // thing.do(arg1, arg2, setting=true)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "thing"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::ID, "do"), s3},
            SourcedToken{Token(TokenID::LEFT_CURVED), s4},
            SourcedToken{Token(TokenID::ID, "arg1"), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::ID, "arg2"), s7},
            SourcedToken{Token(TokenID::COMMA), s8},
            SourcedToken{Token(TokenID::ID, "setting"), s9},
            SourcedToken{Token(TokenID::EQUALS), s10},
            SourcedToken{Token(TokenID::TRUE), s11},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s12}
        )) == vec(
            Op(OpID::ID, "thing", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "do", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "arg1", s5),
            Op(OpID::SEP, s6),
            Op(OpID::ID, "arg2", s7),
            Op(OpID::SEP, s8),
            Op(OpID::ID, "setting", s9),
            Op(OpID::BIND, s10),
            Op(OpID::TRUE, s11),
            Op(OpID::END, s12)
        ));
    }

    SECTION("Continue") {
        // continue
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::CONTINUE), s1}
        )) == vec(
            Op(OpID::CONTINUE, s1)
        ));
    }

    SECTION("Declare") {
        // a: Int
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::ID, "Int"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3)
        ));

        // 3: Int
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::ID, "Int"), s3}
        )) == vec(
            Op(OpID::PLAIN_INT, std::int32_t(3), s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3)
        ));
    }

    SECTION("Def") {
        // def f(): return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f():
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::RETURN), s8},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s9}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, std::int32_t(0), s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f() -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s5},
            SourcedToken{Token(TokenID::ID, "Int"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s9},
            SourcedToken{Token(TokenID::RETURN), s10},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s11}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::RETURNS, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::RETURN, s10),
            Op(OpID::PLAIN_INT, std::int32_t(0), s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(arg) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "arg"), s4},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s5},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s6},
            SourcedToken{Token(TokenID::ID, "Int"), s7},
            SourcedToken{Token(TokenID::COLON), s8},
            SourcedToken{Token(TokenID::NEWLINE), s9},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s10},
            SourcedToken{Token(TokenID::RETURN), s11},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s12}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg", s4),
            Op(OpID::END, s5),
            Op(OpID::RETURNS, s6),
            Op(OpID::ID, "Int", s7),
            Op(OpID::LABEL, s8),
            Op(OpID::BLOCK, s10),
            Op(OpID::RETURN, s11),
            Op(OpID::PLAIN_INT, std::int32_t(0), s12),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(arg1, arg2: Int) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "arg1"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "arg2"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::ID, "Int"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s9},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s10},
            SourcedToken{Token(TokenID::ID, "Int"), s11},
            SourcedToken{Token(TokenID::COLON), s12},
            SourcedToken{Token(TokenID::NEWLINE), s13},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s14},
            SourcedToken{Token(TokenID::RETURN), s15},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::END, s9),
            Op(OpID::RETURNS, s10),
            Op(OpID::ID, "Int", s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, std::int32_t(0), s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(arg1, arg2: Int, *args) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "arg1"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "arg2"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::ID, "Int"), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::STAR), s10},
            SourcedToken{Token(TokenID::ID, "args"), s11},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s12},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s13},
            SourcedToken{Token(TokenID::ID, "Int"), s14},
            SourcedToken{Token(TokenID::COLON), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s17},
            SourcedToken{Token(TokenID::RETURN), s18},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s19}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s9),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::END, s12),
            Op(OpID::RETURNS, s13),
            Op(OpID::ID, "Int", s14),
            Op(OpID::LABEL, s15),
            Op(OpID::BLOCK, s17),
            Op(OpID::RETURN, s18),
            Op(OpID::PLAIN_INT, std::int32_t(0), s19),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(arg1, arg2: Int, *args, kw1=1, kw2: Bool) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "arg1"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "arg2"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::ID, "Int"), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::STAR), s10},
            SourcedToken{Token(TokenID::ID, "args"), s11},
            SourcedToken{Token(TokenID::COMMA), s12},
            SourcedToken{Token(TokenID::ID, "kw1"), s13},
            SourcedToken{Token(TokenID::EQUALS), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15},
            SourcedToken{Token(TokenID::COMMA), s16},
            SourcedToken{Token(TokenID::ID, "kw2"), s17},
            SourcedToken{Token(TokenID::COLON), s18},
            SourcedToken{Token(TokenID::ID, "Bool"), s19},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s20},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s21},
            SourcedToken{Token(TokenID::ID, "Int"), s22},
            SourcedToken{Token(TokenID::COLON), s23},
            SourcedToken{Token(TokenID::NEWLINE), s24},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s25},
            SourcedToken{Token(TokenID::RETURN), s26},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s27}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s9),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::SEP, s12),
            Op(OpID::ID, "kw1", s13),
            Op(OpID::BIND, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::SEP, s16),
            Op(OpID::ID, "kw2", s17),
            Op(OpID::TYPE_LABEL, s18),
            Op(OpID::ID, "Bool", s19),
            Op(OpID::END, s20),
            Op(OpID::RETURNS, s21),
            Op(OpID::ID, "Int", s22),
            Op(OpID::LABEL, s23),
            Op(OpID::BLOCK, s25),
            Op(OpID::RETURN, s26),
            Op(OpID::PLAIN_INT, std::int32_t(0), s27),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(
        //     arg1, arg2: Int, *args, kw1=1, kw2: Bool, **kwargs
        // ) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "arg1"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "arg2"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::ID, "Int"), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::STAR), s10},
            SourcedToken{Token(TokenID::ID, "args"), s11},
            SourcedToken{Token(TokenID::COMMA), s12},
            SourcedToken{Token(TokenID::ID, "kw1"), s13},
            SourcedToken{Token(TokenID::EQUALS), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15},
            SourcedToken{Token(TokenID::COMMA), s16},
            SourcedToken{Token(TokenID::ID, "kw2"), s17},
            SourcedToken{Token(TokenID::COLON), s18},
            SourcedToken{Token(TokenID::ID, "Bool"), s19},
            SourcedToken{Token(TokenID::COMMA), s20},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s21},
            SourcedToken{Token(TokenID::ID, "kwargs"), s22},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s23},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s24},
            SourcedToken{Token(TokenID::ID, "Int"), s25},
            SourcedToken{Token(TokenID::COLON), s26},
            SourcedToken{Token(TokenID::NEWLINE), s27},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s28},
            SourcedToken{Token(TokenID::RETURN), s29},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s30}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s9),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::SEP, s12),
            Op(OpID::ID, "kw1", s13),
            Op(OpID::BIND, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::SEP, s16),
            Op(OpID::ID, "kw2", s17),
            Op(OpID::TYPE_LABEL, s18),
            Op(OpID::ID, "Bool", s19),
            Op(OpID::SEP, s20),
            Op(OpID::UNPACK_KWARGS, s21),
            Op(OpID::ID, "kwargs", s22),
            Op(OpID::END, s23),
            Op(OpID::RETURNS, s24),
            Op(OpID::ID, "Int", s25),
            Op(OpID::LABEL, s26),
            Op(OpID::BLOCK, s28),
            Op(OpID::RETURN, s29),
            Op(OpID::PLAIN_INT, std::int32_t(0), s30),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(a: Int, b: String):
        //     return 0
        // case (c: Float32, true) -> Float64:
        //     return 3.14
        // case (1, *, kw:: 2):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "a"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::ID, "Int"), s6},
            SourcedToken{Token(TokenID::COMMA), s7},
            SourcedToken{Token(TokenID::ID, "b"), s8},
            SourcedToken{Token(TokenID::COLON), s9},
            SourcedToken{Token(TokenID::ID, "String"), s10},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s11},
            SourcedToken{Token(TokenID::COLON), s12},
            SourcedToken{Token(TokenID::NEWLINE), s13},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s14},
            SourcedToken{Token(TokenID::RETURN), s15},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s16},
            SourcedToken{Token(TokenID::NEWLINE), s17},
            SourcedToken{Token(TokenID::CASE), s18},
            SourcedToken{Token(TokenID::LEFT_CURVED), s19},
            SourcedToken{Token(TokenID::ID, "c"), s20},
            SourcedToken{Token(TokenID::COLON), s21},
            SourcedToken{Token(TokenID::ID, "Float32"), s22},
            SourcedToken{Token(TokenID::COMMA), s23},
            SourcedToken{Token(TokenID::TRUE), s24},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s25},
            SourcedToken{Token(TokenID::MINUS_RIGHT_ANGLE), s26},
            SourcedToken{Token(TokenID::ID, "Float64"), s27},
            SourcedToken{Token(TokenID::COLON), s28},
            SourcedToken{Token(TokenID::NEWLINE), s29},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s30},
            SourcedToken{Token(TokenID::RETURN), s31},
            SourcedToken{Token(TokenID::NUMBER, 3.14), s32},
            SourcedToken{Token(TokenID::NEWLINE), s33},
            SourcedToken{Token(TokenID::CASE), s34},
            SourcedToken{Token(TokenID::LEFT_CURVED), s35},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s36},
            SourcedToken{Token(TokenID::COMMA), s37},
            SourcedToken{Token(TokenID::STAR), s38},
            SourcedToken{Token(TokenID::COMMA), s39},
            SourcedToken{Token(TokenID::ID, "kw"), s40},
            SourcedToken{Token(TokenID::DOUBLE_COLON), s41},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s42},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s43},
            SourcedToken{Token(TokenID::COLON), s44},
            SourcedToken{Token(TokenID::NEWLINE), s45},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s46},
            SourcedToken{Token(TokenID::RETURN), s47},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s48}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::SEP, s7),
            Op(OpID::ID, "b", s8),
            Op(OpID::TYPE_LABEL, s9),
            Op(OpID::ID, "String", s10),
            Op(OpID::END, s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, std::int32_t(0), s16),
            Op(OpID::STMT, s17),
            Op(OpID::END, s18),
            Op(OpID::STMT, s18),
            Op(OpID::CASE, s18),
            Op(OpID::GROUP, s19),
            Op(OpID::ID, "c", s20),
            Op(OpID::TYPE_LABEL, s21),
            Op(OpID::ID, "Float32", s22),
            Op(OpID::SEP, s23),
            Op(OpID::TRUE, s24),
            Op(OpID::END, s25),
            Op(OpID::RETURNS, s26),
            Op(OpID::ID, "Float64", s27),
            Op(OpID::LABEL, s28),
            Op(OpID::BLOCK, s30),
            Op(OpID::RETURN, s31),
            Op(OpID::NUMBER, 3.14, s32),
            Op(OpID::STMT, s33),
            Op(OpID::END, s34),
            Op(OpID::STMT, s34),
            Op(OpID::CASE, s34),
            Op(OpID::GROUP, s35),
            Op(OpID::PLAIN_INT, std::int32_t(1), s36),
            Op(OpID::SEP, s37),
            Op(OpID::POS_KW_SEP, s38),
            Op(OpID::SEP, s39),
            Op(OpID::ID, "kw", s40),
            Op(OpID::MATCHING, s41),
            Op(OpID::PLAIN_INT, std::int32_t(2), s42),
            Op(OpID::END, s43),
            Op(OpID::LABEL, s44),
            Op(OpID::BLOCK, s46),
            Op(OpID::RETURN, s47),
            Op(OpID::PLAIN_INT, std::int32_t(0), s48),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def 3():
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::RETURN), s8},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s9}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::PLAIN_INT, std::int32_t(3), s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, std::int32_t(0), s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f[]:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_SQUARE), s3},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::RETURN), s8},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s9}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::LIST, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, std::int32_t(0), s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(a=1, b):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "a"), s4},
            SourcedToken{Token(TokenID::EQUALS), s5},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s6},
            SourcedToken{Token(TokenID::COMMA), s7},
            SourcedToken{Token(TokenID::ID, "b"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token(TokenID::RETURN), s13},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, std::int32_t(1), s6),
            Op(OpID::SEP, s7),
            Op(OpID::ID, "b", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, std::int32_t(0), s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(*, *args):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::STAR), s6},
            SourcedToken{Token(TokenID::ID, "args"), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8},
            SourcedToken{Token(TokenID::COLON), s9},
            SourcedToken{Token(TokenID::NEWLINE), s10},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s11},
            SourcedToken{Token(TokenID::RETURN), s12},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s13}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::POS_KW_SEP, s4),
            Op(OpID::SEP, s5),
            Op(OpID::UNPACK_ARGS, s6),
            Op(OpID::ID, "args", s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, std::int32_t(0), s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(a=1, *args):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "a"), s4},
            SourcedToken{Token(TokenID::EQUALS), s5},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s6},
            SourcedToken{Token(TokenID::COMMA), s7},
            SourcedToken{Token(TokenID::STAR), s8},
            SourcedToken{Token(TokenID::ID, "args"), s9},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s15}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, std::int32_t(1), s6),
            Op(OpID::SEP, s7),
            Op(OpID::UNPACK_ARGS, s8),
            Op(OpID::ID, "args", s9),
            Op(OpID::END, s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(0), s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(a=1=2):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "a"), s4},
            SourcedToken{Token(TokenID::EQUALS), s5},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s6},
            SourcedToken{Token(TokenID::EQUALS), s7},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s8},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token(TokenID::RETURN), s13},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, std::int32_t(1), s6),
            Op(OpID::BIND, s7),
            Op(OpID::PLAIN_INT, std::int32_t(2), s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, std::int32_t(0), s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(*args=1):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::ID, "args"), s5},
            SourcedToken{Token(TokenID::EQUALS), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8},
            SourcedToken{Token(TokenID::COLON), s9},
            SourcedToken{Token(TokenID::NEWLINE), s10},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s11},
            SourcedToken{Token(TokenID::RETURN), s12},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s13}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::BIND, s6),
            Op(OpID::PLAIN_INT, std::int32_t(1), s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, std::int32_t(0), s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(**kwargs=1):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s4},
            SourcedToken{Token(TokenID::ID, "kwargs"), s5},
            SourcedToken{Token(TokenID::EQUALS), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8},
            SourcedToken{Token(TokenID::COLON), s9},
            SourcedToken{Token(TokenID::NEWLINE), s10},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s11},
            SourcedToken{Token(TokenID::RETURN), s12},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s13}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_KWARGS, s4),
            Op(OpID::ID, "kwargs", s5),
            Op(OpID::BIND, s6),
            Op(OpID::PLAIN_INT, std::int32_t(1), s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, std::int32_t(0), s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(*3):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s9},
            SourcedToken{Token(TokenID::RETURN), s10},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s11}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::PLAIN_INT, std::int32_t(3), s5),
            Op(OpID::END, s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::RETURN, s10),
            Op(OpID::PLAIN_INT, std::int32_t(0), s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(**3):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s9},
            SourcedToken{Token(TokenID::RETURN), s10},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s11}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_KWARGS, s4),
            Op(OpID::PLAIN_INT, std::int32_t(3), s5),
            Op(OpID::END, s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::RETURN, s10),
            Op(OpID::PLAIN_INT, std::int32_t(0), s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(3: Int):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::ID, "Int"), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::COLON), s8},
            SourcedToken{Token(TokenID::NEWLINE), s9},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s10},
            SourcedToken{Token(TokenID::RETURN), s11},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s12}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::PLAIN_INT, std::int32_t(3), s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::END, s7),
            Op(OpID::LABEL, s8),
            Op(OpID::BLOCK, s10),
            Op(OpID::RETURN, s11),
            Op(OpID::PLAIN_INT, std::int32_t(0), s12),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f(a: Int, a: Float64):
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::ID, "a"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::ID, "Int"), s6},
            SourcedToken{Token(TokenID::COMMA), s7},
            SourcedToken{Token(TokenID::ID, "a"), s8},
            SourcedToken{Token(TokenID::COLON), s9},
            SourcedToken{Token(TokenID::ID, "Float64"), s10},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s11},
            SourcedToken{Token(TokenID::COLON), s12},
            SourcedToken{Token(TokenID::NEWLINE), s13},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s14},
            SourcedToken{Token(TokenID::RETURN), s15},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::SEP, s7),
            Op(OpID::ID, "a", s8),
            Op(OpID::TYPE_LABEL, s9),
            Op(OpID::ID, "Float64", s10),
            Op(OpID::END, s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, std::int32_t(0), s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // def f():
        //     return 0
        // elif true:
        //     return 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::DEF), s1},
            SourcedToken{Token(TokenID::ID, "f"), s2},
            SourcedToken{Token(TokenID::LEFT_CURVED), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::RETURN), s8},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s9},
            SourcedToken{Token(TokenID::NEWLINE), s10},
            SourcedToken{Token(TokenID::ELIF), s11},
            SourcedToken{Token(TokenID::TRUE), s12},
            SourcedToken{Token(TokenID::COLON), s13},
            SourcedToken{Token(TokenID::NEWLINE), s14},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s15},
            SourcedToken{Token(TokenID::RETURN), s16},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s17}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, std::int32_t(0), s9),
            Op(OpID::STMT, s10),
            Op(OpID::END, s11),
            Op(OpID::STMT, s11),
            Op(OpID::ELIF, s11),
            Op(OpID::TRUE, s12),
            Op(OpID::LABEL, s13),
            Op(OpID::BLOCK, s15),
            Op(OpID::RETURN, s16),
            Op(OpID::PLAIN_INT, std::int32_t(1), s17),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

    }

    SECTION("DIV") {
        // a / b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::SLASH), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::DIV, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("For") {
        // for x in c: a += x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::ID, "a"), s6},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s7},
            SourcedToken{Token(TokenID::ID, "x"), s8}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::ID, "a", s6),
            Op(OpID::IADD, s7),
            Op(OpID::ID, "x", s8),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x in c:
        //     a += x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::ID, "a"), s8},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s9},
            SourcedToken{Token(TokenID::ID, "x"), s10}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::ID, "a", s8),
            Op(OpID::IADD, s9),
            Op(OpID::ID, "x", s10),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x, y in c:
        //     a += x + y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::ID, "y"), s4},
            SourcedToken{Token(TokenID::IN), s5},
            SourcedToken{Token(TokenID::ID, "c"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s9},
            SourcedToken{Token(TokenID::ID, "a"), s10},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s11},
            SourcedToken{Token(TokenID::ID, "x"), s12},
            SourcedToken{Token(TokenID::PLUS), s13},
            SourcedToken{Token(TokenID::ID, "y"), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LOOP_VAR_SEP, s3),
            Op(OpID::ID, "y", s4),
            Op(OpID::IN, s5),
            Op(OpID::ID, "c", s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::ID, "a", s10),
            Op(OpID::IADD, s11),
            Op(OpID::ID, "x", s12),
            Op(OpID::ADD, s13),
            Op(OpID::ID, "y", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x in c:
        //     break
        // else:
        //     a += 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::BREAK), s8},
            SourcedToken{Token(TokenID::NEWLINE), s9},
            SourcedToken{Token(TokenID::ELSE), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::ID, "a"), s14},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s15},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::BREAK, s8),
            Op(OpID::STMT, s9),
            Op(OpID::END, s10),
            Op(OpID::STMT, s10),
            Op(OpID::ELSE, s10),
            Op(OpID::BODY, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::ID, "a", s14),
            Op(OpID::IADD, s15),
            Op(OpID::PLAIN_INT, std::int32_t(1), s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x in c
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x:
        //     a += x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::ID, "a"), s6},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s7},
            SourcedToken{Token(TokenID::ID, "x"), s8}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::ID, "a", s6),
            Op(OpID::IADD, s7),
            Op(OpID::ID, "x", s8),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for 3 in c:
        //     a += 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::ID, "a"), s8},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s9},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s10}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::PLAIN_INT, std::int32_t(3), s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::ID, "a", s8),
            Op(OpID::IADD, s9),
            Op(OpID::PLAIN_INT, std::int32_t(1), s10),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // for x in c:
        //     a += x
        // elif true:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FOR), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::IN), s3},
            SourcedToken{Token(TokenID::ID, "c"), s4},
            SourcedToken{Token(TokenID::COLON), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s7},
            SourcedToken{Token(TokenID::ID, "a"), s8},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s9},
            SourcedToken{Token(TokenID::ID, "x"), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::ELIF), s12},
            SourcedToken{Token(TokenID::TRUE), s13},
            SourcedToken{Token(TokenID::COLON), s14},
            SourcedToken{Token(TokenID::NEWLINE), s15},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s16},
            SourcedToken{Token(TokenID::RETURN), s17},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s18}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::ID, "a", s8),
            Op(OpID::IADD, s9),
            Op(OpID::ID, "x", s10),
            Op(OpID::STMT, s11),
            Op(OpID::END, s12),
            Op(OpID::STMT, s12),
            Op(OpID::ELIF, s12),
            Op(OpID::TRUE, s13),
            Op(OpID::LABEL, s14),
            Op(OpID::BLOCK, s16),
            Op(OpID::RETURN, s17),
            Op(OpID::PLAIN_INT, std::int32_t(0), s18),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));
    }

    SECTION("GetAttr") {
        // a.b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3)
        ));

        // a.3
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, std::int32_t(3), s3)
        ));
    }

    SECTION("IADD") {
        // a += b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PLUS_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IADD, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IBAND") {
        // a &= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::AMPERSAND_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBAND, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IBOR") {
        // a |= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PIPE_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBOR, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IBXOR") {
        // a ^= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::CAROT_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBXOR, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("ID") {
        // a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1}
        )) == vec(
            Op(OpID::ID, "a", s1)
        ));
    }

    SECTION("IDIV") {
        // a /= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::SLASH_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IDIV, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("ILSH") {
        // a <<= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_LEFT_ANGLE_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ILSH, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IMOD") {
        // a %= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PERCENT_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IMOD, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IMUL") {
        // a *= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::STAR_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IMUL, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IPOW") {
        // a **= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_STAR_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IPOW, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("IRSH") {
        // a >>= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_RIGHT_ANGLE_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IRSH, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("ISUB") {
        // a -= b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::MINUS_EQUALS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ISUB, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("If") {
        // if x: return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::RETURN), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s5}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::RETURN, s4),
            Op(OpID::PLAIN_INT, std::int32_t(0), s5),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELIF), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        // else:
        //     return 2
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELIF), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::ELSE), s17},
            SourcedToken{Token(TokenID::COLON), s18},
            SourcedToken{Token(TokenID::NEWLINE), s19},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s20},
            SourcedToken{Token(TokenID::RETURN), s21},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s22}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::STMT, s16),
            Op(OpID::END, s17),
            Op(OpID::STMT, s17),
            Op(OpID::ELSE, s17),
            Op(OpID::BODY, s18),
            Op(OpID::BLOCK, s20),
            Op(OpID::RETURN, s21),
            Op(OpID::PLAIN_INT, std::int32_t(2), s22),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // else:
        //     return 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELSE), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token(TokenID::RETURN), s13},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELSE, s9),
            Op(OpID::BODY, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, std::int32_t(1), s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));


        // if x:
        //     return 0
        // elif y:
        //     return 1
        // elif z:
        //     return 2
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELIF), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::ELIF), s17},
            SourcedToken{Token(TokenID::ID, "z"), s18},
            SourcedToken{Token(TokenID::COLON), s19},
            SourcedToken{Token(TokenID::NEWLINE), s20},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s21},
            SourcedToken{Token(TokenID::RETURN), s22},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s23}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::STMT, s16),
            Op(OpID::END, s17),
            Op(OpID::STMT, s17),
            Op(OpID::ELIF, s17),
            Op(OpID::ID, "z", s18),
            Op(OpID::LABEL, s19),
            Op(OpID::BLOCK, s21),
            Op(OpID::RETURN, s22),
            Op(OpID::PLAIN_INT, std::int32_t(2), s23),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // elif y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELIF), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // else y:
        //     return 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::ELSE), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELSE, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // if x:
        //     return 0
        // case y:
        //     return 1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::IF), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::RETURN), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::CASE), s9},
            SourcedToken{Token(TokenID::ID, "y"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::RETURN), s14},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s15}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, std::int32_t(0), s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::CASE, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, std::int32_t(1), s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));
    }

    SECTION("Init") {
        // a: Int = 3
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::ID, "Int"), s3},
            SourcedToken{Token(TokenID::EQUALS), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s5}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::SET, s4),
            Op(OpID::PLAIN_INT, std::int32_t(3), s5)
        ));

        // 3: Int = 3
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::ID, "Int"), s3},
            SourcedToken{Token(TokenID::EQUALS), s4},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s5}
        )) == vec(
            Op(OpID::PLAIN_INT, std::int32_t(3), s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::SET, s4),
            Op(OpID::PLAIN_INT, std::int32_t(3), s5)
        ));
    }

    SECTION("LSH") {
        // a << b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_LEFT_ANGLE), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::LSH, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Lambda Expression") {
        // %(%1 + %2)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PERCENT), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::PERCENT), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s4},
            SourcedToken{Token(TokenID::PLUS), s5},
            SourcedToken{Token(TokenID::PERCENT), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8}
        )) == vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::GROUP, s2),
            Op(OpID::LAMBDA, s3),
            Op(OpID::PLAIN_INT, std::int32_t(1), s4),
            Op(OpID::ADD, s5),
            Op(OpID::LAMBDA, s6),
            Op(OpID::PLAIN_INT, std::int32_t(2), s7),
            Op(OpID::END, s8)
        ));

        // %[%1 + %2]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PERCENT), s1},
            SourcedToken{Token(TokenID::LEFT_SQUARE), s2},
            SourcedToken{Token(TokenID::PERCENT), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s4},
            SourcedToken{Token(TokenID::PLUS), s5},
            SourcedToken{Token(TokenID::PERCENT), s6},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s7},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s8}
        )) == vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::LIST, s2),
            Op(OpID::LAMBDA, s3),
            Op(OpID::PLAIN_INT, std::int32_t(1), s4),
            Op(OpID::ADD, s5),
            Op(OpID::LAMBDA, s6),
            Op(OpID::PLAIN_INT, std::int32_t(2), s7),
            Op(OpID::END, s8)
        ));
    }

    SECTION("Lambda Keyword Var") {
        // %a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PERCENT), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("Lambda Positional Var") {
        // %1
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PERCENT), s1},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s2}
        )) == vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::PLAIN_INT, std::int32_t(1), s2)
        ));

        // %0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PERCENT), s1},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s2}
        )) == vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::PLAIN_INT, std::int32_t(0), s2)
        ));
    }

    SECTION("List") {
        // []
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_SQUARE), s1},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s2}
        )) == vec(
            Op(OpID::LIST, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        ));

        // [a]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_SQUARE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s3}
        )) == vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::END, s3)
        ));

        // [a, b, c]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_SQUARE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::ID, "b"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "c"), s6},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s7}
        )) == vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        ));

        // [a, *args, b]
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_SQUARE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::ID, "args"), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::ID, "b"), s7},
            SourcedToken{Token(TokenID::RIGHT_SQUARE), s8}
        )) == vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s6),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        ));
    }

    SECTION("Literals") {
        // true
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRUE), s1}
        )) == vec(
            Op(OpID::TRUE, s1)
        ));

        // false
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::FALSE), s1}
        )) == vec(
            Op(OpID::FALSE, s1)
        ));

        // 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s1}
        )) == vec(
            Op(OpID::PLAIN_INT, std::int32_t(0), s1)
        ));

        // 0s8
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::int8_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::int8_t(0), s1)
        ));

        // 0s16
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::int16_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::int16_t(0), s1)
        ));

        // 0s32
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::int32_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::int32_t(0), s1)
        ));

        // 0s64
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::int64_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::int64_t(0), s1)
        ));

        // 0u8
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::uint8_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::uint8_t(0), s1)
        ));

        // 0u16
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::uint16_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::uint16_t(0), s1)
        ));

        // 0u32
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::uint32_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::uint32_t(0), s1)
        ));

        // 0u64
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, std::uint64_t(0)), s1}
        )) == vec(
            Op(OpID::NUMBER, std::uint64_t(0), s1)
        ));

        // 0f32
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, 0.0f), s1}
        )) == vec(
            Op(OpID::NUMBER, 0.0f, s1)
        ));

        // 0f64
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::NUMBER, 0.0), s1}
        )) == vec(
            Op(OpID::NUMBER, 0.0, s1)
        ));

        // '\0'
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::CHAR, std::int32_t(0)), s1}
        )) == vec(
            Op(OpID::CHAR, std::int32_t(0), s1)
        ));

        // "asdf"
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::STRING, "asdf"), s1}
        )) == vec(
            Op(OpID::STRING, "asdf", s1)
        ));
    }

    SECTION("MOD") {
        // a % b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::PERCENT), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::MOD, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("MUL") {
        // a * b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::STAR), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::MUL, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Map") {
        // {}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s2}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        ));

        // {a: 1}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s4},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s5}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, std::int32_t(1), s4),
            Op(OpID::END, s5)
        ));

        // {a: 1, b: 2, c: 3}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "b"), s6},
            SourcedToken{Token(TokenID::COLON), s7},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s8},
            SourcedToken{Token(TokenID::COMMA), s9},
            SourcedToken{Token(TokenID::ID, "c"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s12},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s13}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, std::int32_t(1), s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "b", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::PLAIN_INT, std::int32_t(2), s8),
            Op(OpID::SEP, s9),
            Op(OpID::ID, "c", s10),
            Op(OpID::TYPE_LABEL, s11),
            Op(OpID::PLAIN_INT, std::int32_t(3), s12),
            Op(OpID::END, s13)
        ));

        // {a: 1, **kwargs, b: 2}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(1)), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s6},
            SourcedToken{Token(TokenID::ID, "kwargs"), s7},
            SourcedToken{Token(TokenID::COMMA), s8},
            SourcedToken{Token(TokenID::ID, "b"), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(2)), s11},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s12}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, std::int32_t(1), s4),
            Op(OpID::SEP, s5),
            Op(OpID::UNPACK_KWARGS, s6),
            Op(OpID::ID, "kwargs", s7),
            Op(OpID::SEP, s8),
            Op(OpID::ID, "b", s9),
            Op(OpID::TYPE_LABEL, s10),
            Op(OpID::PLAIN_INT, std::int32_t(2), s11),
            Op(OpID::END, s12)
        ));
    }

    SECTION("Match") {
        // match x
        // case Int{%y}: return y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::CASE), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token{TokenID::RETURN}, s11},
            SourcedToken{Token(TokenID::ID, "y"), s12}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::CASE, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::RETURN, s11),
            Op(OpID::ID, "y", s12),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        // case Int{%y}:
        //     return y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::CASE), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token{TokenID::RETURN}, s13},
            SourcedToken{Token(TokenID::ID, "y"), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::CASE, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::ID, "y", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        // case Int{%y}:
        //     return y
        // else:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::CASE), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token{TokenID::RETURN}, s13},
            SourcedToken{Token(TokenID::ID, "y"), s14},
            SourcedToken{Token(TokenID::NEWLINE), s15},
            SourcedToken{Token(TokenID::ELSE), s16},
            SourcedToken{Token(TokenID::COLON), s17},
            SourcedToken{Token(TokenID::NEWLINE), s18},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s19},
            SourcedToken{Token(TokenID::RETURN), s20},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s21}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::CASE, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::ID, "y", s14),
            Op(OpID::STMT, s15),
            Op(OpID::END, s16),
            Op(OpID::STMT, s16),
            Op(OpID::ELSE, s16),
            Op(OpID::BODY, s17),
            Op(OpID::BLOCK, s19),
            Op(OpID::RETURN, s20),
            Op(OpID::PLAIN_INT, std::int32_t(0), s21),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        // case Int{%y}:
        //     return y
        // case false:
        //     return 0
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::CASE), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token{TokenID::RETURN}, s13},
            SourcedToken{Token(TokenID::ID, "y"), s14},
            SourcedToken{Token(TokenID::NEWLINE), s15},
            SourcedToken{Token(TokenID::CASE), s16},
            SourcedToken{Token(TokenID::FALSE), s17},
            SourcedToken{Token(TokenID::COLON), s18},
            SourcedToken{Token(TokenID::NEWLINE), s19},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s20},
            SourcedToken{Token(TokenID::RETURN), s21},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(0)), s22}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::CASE, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::ID, "y", s14),
            Op(OpID::STMT, s15),
            Op(OpID::END, s16),
            Op(OpID::STMT, s16),
            Op(OpID::CASE, s16),
            Op(OpID::FALSE, s17),
            Op(OpID::LABEL, s18),
            Op(OpID::BLOCK, s20),
            Op(OpID::RETURN, s21),
            Op(OpID::PLAIN_INT, std::int32_t(0), s22),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        // case Int{%y}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::CASE), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::CASE, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // match x
        // elif Int{%y}:
        //     return y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MATCH), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::ELIF), s4},
            SourcedToken{Token(TokenID::ID, "Int"), s5},
            SourcedToken{Token(TokenID::LEFT_CURLY), s6},
            SourcedToken{Token(TokenID::PERCENT), s7},
            SourcedToken{Token(TokenID::ID, "y"), s8},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token{TokenID::RETURN}, s13},
            SourcedToken{Token(TokenID::ID, "y"), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s3),
            Op(OpID::ELIF, s4),
            Op(OpID::ID, "Int", s5),
            Op(OpID::CALL, s6),
            Op(OpID::ENCLOSURE, s6),
            Op(OpID::LAMBDA, s7),
            Op(OpID::ID, "y", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::ID, "y", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));
    }

    SECTION("NEG") {
        // -a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::MINUS), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::NEG, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("Or") {
        // a or b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::OR), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::OR, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("POW") {
        // a ** b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_STAR), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::POW, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("RSH") {
        // a >> b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOUBLE_RIGHT_ANGLE), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::RSH, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Raise") {
        // raise
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::RAISE), s1}
        )) == vec(
            Op(OpID::RAISE, s1),
            Op(OpID::NOTHING, s_end)
        ));

        // raise a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::RAISE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::RAISE, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("Return") {
        // return
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::RETURN), s1}
        )) == vec(
            Op(OpID::RETURN, s1),
            Op(OpID::NOTHING, s_end)
        ));

        // return a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::RETURN), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::RETURN, s1),
            Op(OpID::ID, "a", s2)
        ));
    }

    SECTION("SUB") {
        // a - b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::MINUS), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SUB, s2),
            Op(OpID::ID, "b", s3)
        ));
    }

    SECTION("Set") {
        // {a}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s3}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::END, s3)
        ));

        // {a, b, c}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::ID, "b"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "c"), s6},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s7}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        ));

        // {a, *args, b}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURLY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::ID, "args"), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::ID, "b"), s7},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s8}
        )) == vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s6),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        ));
    }

    SECTION("SetAttr") {
        // a.b = c
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3},
            SourcedToken{Token(TokenID::EQUALS), s4},
            SourcedToken{Token(TokenID::ID, "c"), s5}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3),
            Op(OpID::SET, s4),
            Op(OpID::ID, "c", s5)
        ));

        // a.3 = b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s3},
            SourcedToken{Token(TokenID::EQUALS), s4},
            SourcedToken{Token(TokenID::ID, "b"), s5}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, std::int32_t(3), s3),
            Op(OpID::SET, s4),
            Op(OpID::ID, "b", s5)
        ));
    }

    SECTION("Symbol") {
        // :x
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::COLON), s1},
            SourcedToken{Token(TokenID::ID, "x"), s2}
        )) == vec(
            Op(OpID::SYMBOL, s1),
            Op(OpID::ID, "x", s2)
        ));

        // :3
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::COLON), s1},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s2}
        )) == vec(
            Op(OpID::SYMBOL, s1),
            Op(OpID::PLAIN_INT, std::int32_t(3), s2)
        ));

    }

    SECTION("Ternary") {
        // x if y else z
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "x"), s1},
            SourcedToken{Token(TokenID::IF), s2},
            SourcedToken{Token(TokenID::ID, "y"), s3},
            SourcedToken{Token(TokenID::ELSE), s4},
            SourcedToken{Token(TokenID::ID, "z"), s5}
        )) == vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_IF, s2),
            Op(OpID::ID, "y", s3),
            Op(OpID::TERNARY_ELSE, s4),
            Op(OpID::ID, "z", s5)
        ));

        // x if y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "x"), s1},
            SourcedToken{Token(TokenID::IF), s2},
            SourcedToken{Token(TokenID::ID, "y"), s3}
        )) == vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_IF, s2),
            Op(OpID::ID, "y", s3)
        ));

        // x else y
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "x"), s1},
            SourcedToken{Token(TokenID::ELSE), s2},
            SourcedToken{Token(TokenID::ID, "y"), s3}
        )) == vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_ELSE, s2),
            Op(OpID::ID, "y", s3)
        ));
    }

    SECTION("This") {
        // this
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::THIS), s1}
        )) == vec(
            Op(OpID::THIS, s1)
        ));
    }

    SECTION("Try") {
        // try: f()
        // except E{%e}: g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::ID, "f"), s3},
            SourcedToken{Token(TokenID::LEFT_CURVED), s4},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s5},
            SourcedToken{Token(TokenID::NEWLINE), s6},
            SourcedToken{Token(TokenID::EXCEPT), s7},
            SourcedToken{Token(TokenID::ID, "E"), s8},
            SourcedToken{Token(TokenID::LEFT_CURLY), s9},
            SourcedToken{Token(TokenID::PERCENT), s10},
            SourcedToken{Token(TokenID::ID, "e"), s11},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s12},
            SourcedToken{Token(TokenID::COLON), s13},
            SourcedToken{Token(TokenID::ID, "g"), s14},
            SourcedToken{Token(TokenID::LEFT_CURVED), s15},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::ID, "f", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::NOTHING, s5),
            Op(OpID::END, s5),
            Op(OpID::STMT, s6),
            Op(OpID::EXCEPT, s7),
            Op(OpID::ID, "E", s8),
            Op(OpID::CALL, s9),
            Op(OpID::ENCLOSURE, s9),
            Op(OpID::LAMBDA, s10),
            Op(OpID::ID, "e", s11),
            Op(OpID::END, s12),
            Op(OpID::LABEL, s13),
            Op(OpID::ID, "g", s14),
            Op(OpID::CALL, s15),
            Op(OpID::GROUP, s15),
            Op(OpID::NOTHING, s16),
            Op(OpID::END, s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // except E{%e}:
        //     g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::EXCEPT), s9},
            SourcedToken{Token(TokenID::ID, "E"), s10},
            SourcedToken{Token(TokenID::LEFT_CURLY), s11},
            SourcedToken{Token(TokenID::PERCENT), s12},
            SourcedToken{Token(TokenID::ID, "e"), s13},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s14},
            SourcedToken{Token(TokenID::COLON), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s17},
            SourcedToken{Token(TokenID::ID, "g"), s18},
            SourcedToken{Token(TokenID::LEFT_CURVED), s19},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s20}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::EXCEPT, s9),
            Op(OpID::ID, "E", s10),
            Op(OpID::CALL, s11),
            Op(OpID::ENCLOSURE, s11),
            Op(OpID::LAMBDA, s12),
            Op(OpID::ID, "e", s13),
            Op(OpID::END, s14),
            Op(OpID::LABEL, s15),
            Op(OpID::BLOCK, s17),
            Op(OpID::ID, "g", s18),
            Op(OpID::CALL, s19),
            Op(OpID::GROUP, s19),
            Op(OpID::NOTHING, s20),
            Op(OpID::END, s20),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // except E{%e}:
        //     g()
        // finally:
        //     h()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::EXCEPT), s9},
            SourcedToken{Token(TokenID::ID, "E"), s10},
            SourcedToken{Token(TokenID::LEFT_CURLY), s11},
            SourcedToken{Token(TokenID::PERCENT), s12},
            SourcedToken{Token(TokenID::ID, "e"), s13},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s14},
            SourcedToken{Token(TokenID::COLON), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s17},
            SourcedToken{Token(TokenID::ID, "g"), s18},
            SourcedToken{Token(TokenID::LEFT_CURVED), s19},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s20},
            SourcedToken{Token(TokenID::NEWLINE), s21},
            SourcedToken{Token(TokenID::FINALLY), s22},
            SourcedToken{Token(TokenID::COLON), s23},
            SourcedToken{Token(TokenID::NEWLINE), s24},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s25},
            SourcedToken{Token(TokenID::ID, "h"), s26},
            SourcedToken{Token(TokenID::LEFT_CURVED), s27},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s28}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::EXCEPT, s9),
            Op(OpID::ID, "E", s10),
            Op(OpID::CALL, s11),
            Op(OpID::ENCLOSURE, s11),
            Op(OpID::LAMBDA, s12),
            Op(OpID::ID, "e", s13),
            Op(OpID::END, s14),
            Op(OpID::LABEL, s15),
            Op(OpID::BLOCK, s17),
            Op(OpID::ID, "g", s18),
            Op(OpID::CALL, s19),
            Op(OpID::GROUP, s19),
            Op(OpID::NOTHING, s20),
            Op(OpID::END, s20),
            Op(OpID::STMT, s21),
            Op(OpID::END, s22),
            Op(OpID::STMT, s22),
            Op(OpID::FINALLY, s22),
            Op(OpID::BODY, s23),
            Op(OpID::BLOCK, s25),
            Op(OpID::ID, "h", s26),
            Op(OpID::CALL, s27),
            Op(OpID::GROUP, s27),
            Op(OpID::NOTHING, s28),
            Op(OpID::END, s28),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // finally:
        //     g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::FINALLY), s9},
            SourcedToken{Token(TokenID::COLON), s10},
            SourcedToken{Token(TokenID::NEWLINE), s11},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s12},
            SourcedToken{Token(TokenID::ID, "g"), s13},
            SourcedToken{Token(TokenID::LEFT_CURVED), s14},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s15}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::FINALLY, s9),
            Op(OpID::BODY, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::ID, "g", s13),
            Op(OpID::CALL, s14),
            Op(OpID::GROUP, s14),
            Op(OpID::NOTHING, s15),
            Op(OpID::END, s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // except E1{%e}:
        //     g()
        // except E2{%e}:
        //     h()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::EXCEPT), s9},
            SourcedToken{Token(TokenID::ID, "E1"), s10},
            SourcedToken{Token(TokenID::LEFT_CURLY), s11},
            SourcedToken{Token(TokenID::PERCENT), s12},
            SourcedToken{Token(TokenID::ID, "e"), s13},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s14},
            SourcedToken{Token(TokenID::COLON), s15},
            SourcedToken{Token(TokenID::NEWLINE), s16},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s17},
            SourcedToken{Token(TokenID::ID, "g"), s18},
            SourcedToken{Token(TokenID::LEFT_CURVED), s19},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s20},
            SourcedToken{Token(TokenID::NEWLINE), s21},
            SourcedToken{Token(TokenID::EXCEPT), s22},
            SourcedToken{Token(TokenID::ID, "E2"), s23},
            SourcedToken{Token(TokenID::LEFT_CURLY), s24},
            SourcedToken{Token(TokenID::PERCENT), s25},
            SourcedToken{Token(TokenID::ID, "e"), s26},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s27},
            SourcedToken{Token(TokenID::COLON), s28},
            SourcedToken{Token(TokenID::NEWLINE), s29},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s30},
            SourcedToken{Token(TokenID::ID, "h"), s31},
            SourcedToken{Token(TokenID::LEFT_CURVED), s32},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s33}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::EXCEPT, s9),
            Op(OpID::ID, "E1", s10),
            Op(OpID::CALL, s11),
            Op(OpID::ENCLOSURE, s11),
            Op(OpID::LAMBDA, s12),
            Op(OpID::ID, "e", s13),
            Op(OpID::END, s14),
            Op(OpID::LABEL, s15),
            Op(OpID::BLOCK, s17),
            Op(OpID::ID, "g", s18),
            Op(OpID::CALL, s19),
            Op(OpID::GROUP, s19),
            Op(OpID::NOTHING, s20),
            Op(OpID::END, s20),
            Op(OpID::STMT, s21),
            Op(OpID::END, s22),
            Op(OpID::STMT, s22),
            Op(OpID::EXCEPT, s22),
            Op(OpID::ID, "E2", s23),
            Op(OpID::CALL, s24),
            Op(OpID::ENCLOSURE, s24),
            Op(OpID::LAMBDA, s25),
            Op(OpID::ID, "e", s26),
            Op(OpID::END, s27),
            Op(OpID::LABEL, s28),
            Op(OpID::BLOCK, s30),
            Op(OpID::ID, "h", s31),
            Op(OpID::CALL, s32),
            Op(OpID::GROUP, s32),
            Op(OpID::NOTHING, s33),
            Op(OpID::END, s33),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try a:
        //     f()
        // except E{%e}:
        //     g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::ID, "f"), s6},
            SourcedToken{Token(TokenID::LEFT_CURVED), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8},
            SourcedToken{Token(TokenID::NEWLINE), s9},
            SourcedToken{Token(TokenID::EXCEPT), s10},
            SourcedToken{Token(TokenID::ID, "E"), s11},
            SourcedToken{Token(TokenID::LEFT_CURLY), s12},
            SourcedToken{Token(TokenID::PERCENT), s13},
            SourcedToken{Token(TokenID::ID, "e"), s14},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s15},
            SourcedToken{Token(TokenID::COLON), s16},
            SourcedToken{Token(TokenID::NEWLINE), s17},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s18},
            SourcedToken{Token(TokenID::ID, "g"), s19},
            SourcedToken{Token(TokenID::LEFT_CURVED), s20},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s21}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::ID, "f", s6),
            Op(OpID::CALL, s7),
            Op(OpID::GROUP, s7),
            Op(OpID::NOTHING, s8),
            Op(OpID::END, s8),
            Op(OpID::STMT, s9),
            Op(OpID::END, s10),
            Op(OpID::STMT, s10),
            Op(OpID::EXCEPT, s10),
            Op(OpID::ID, "E", s11),
            Op(OpID::CALL, s12),
            Op(OpID::ENCLOSURE, s12),
            Op(OpID::LAMBDA, s13),
            Op(OpID::ID, "e", s14),
            Op(OpID::END, s15),
            Op(OpID::LABEL, s16),
            Op(OpID::BLOCK, s18),
            Op(OpID::ID, "g", s19),
            Op(OpID::CALL, s20),
            Op(OpID::GROUP, s20),
            Op(OpID::NOTHING, s21),
            Op(OpID::END, s21),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // finally a:
        //     g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::FINALLY), s9},
            SourcedToken{Token(TokenID::ID, "a"), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::ID, "g"), s14},
            SourcedToken{Token(TokenID::LEFT_CURVED), s15},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::FINALLY, s9),
            Op(OpID::ID, "a", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::ID, "g", s14),
            Op(OpID::CALL, s15),
            Op(OpID::GROUP, s15),
            Op(OpID::NOTHING, s16),
            Op(OpID::END, s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // try:
        //     f()
        // except E{%e}
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::TRY), s1},
            SourcedToken{Token(TokenID::COLON), s2},
            SourcedToken{Token(TokenID::NEWLINE), s3},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s4},
            SourcedToken{Token(TokenID::ID, "f"), s5},
            SourcedToken{Token(TokenID::LEFT_CURVED), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7},
            SourcedToken{Token(TokenID::NEWLINE), s8},
            SourcedToken{Token(TokenID::EXCEPT), s9},
            SourcedToken{Token(TokenID::ID, "E"), s10},
            SourcedToken{Token(TokenID::LEFT_CURLY), s11},
            SourcedToken{Token(TokenID::PERCENT), s12},
            SourcedToken{Token(TokenID::ID, "e"), s13},
            SourcedToken{Token(TokenID::RIGHT_CURLY), s14}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::TRY, s1),
            Op(OpID::BODY, s2),
            Op(OpID::BLOCK, s4),
            Op(OpID::ID, "f", s5),
            Op(OpID::CALL, s6),
            Op(OpID::GROUP, s6),
            Op(OpID::NOTHING, s7),
            Op(OpID::END, s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::EXCEPT, s9),
            Op(OpID::ID, "E", s10),
            Op(OpID::CALL, s11),
            Op(OpID::ENCLOSURE, s11),
            Op(OpID::LAMBDA, s12),
            Op(OpID::ID, "e", s13),
            Op(OpID::END, s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));
    }

    SECTION("Tuple") {
        // ()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURVED), s1},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s2}
        )) == vec(
            Op(OpID::GROUP, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        ));

        // (a, b, c)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURVED), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::ID, "b"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "c"), s6},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s7}
        )) == vec(
            Op(OpID::GROUP, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        ));

        // a, b, c
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::COMMA), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3},
            SourcedToken{Token(TokenID::COMMA), s4},
            SourcedToken{Token(TokenID::ID, "c"), s5}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SEP, s2),
            Op(OpID::ID, "b", s3),
            Op(OpID::SEP, s4),
            Op(OpID::ID, "c", s5)
        ));

        // (a, *args, b)
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::LEFT_CURVED), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COMMA), s3},
            SourcedToken{Token(TokenID::STAR), s4},
            SourcedToken{Token(TokenID::ID, "args"), s5},
            SourcedToken{Token(TokenID::COMMA), s6},
            SourcedToken{Token(TokenID::ID, "b"), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8}
        )) == vec(
            Op(OpID::GROUP, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s6),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        ));

        // a, *args, b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::COMMA), s2},
            SourcedToken{Token(TokenID::STAR), s3},
            SourcedToken{Token(TokenID::ID, "args"), s4},
            SourcedToken{Token(TokenID::COMMA), s5},
            SourcedToken{Token(TokenID::ID, "b"), s6}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SEP, s2),
            Op(OpID::UNPACK_ARGS, s3),
            Op(OpID::ID, "args", s4),
            Op(OpID::SEP, s5),
            Op(OpID::ID, "b", s6)
        ));
    }

    SECTION("Type") {

    }

    SECTION("Update") {
        // a(i) = b
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::LEFT_CURVED), s2},
            SourcedToken{Token(TokenID::ID, "i"), s3},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s4},
            SourcedToken{Token(TokenID::EQUALS), s5},
            SourcedToken{Token(TokenID::ID, "b"), s6}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::ID, "i", s3),
            Op(OpID::END, s4),
            Op(OpID::SET, s5),
            Op(OpID::ID, "b", s6)
        ));
    }

    SECTION("UpdateAttr") {
        // a.b(i) = c
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::ID, "b"), s3},
            SourcedToken{Token(TokenID::LEFT_CURVED), s4},
            SourcedToken{Token(TokenID::ID, "i"), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6},
            SourcedToken{Token(TokenID::EQUALS), s7},
            SourcedToken{Token(TokenID::ID, "c"), s8}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "i", s5),
            Op(OpID::END, s6),
            Op(OpID::SET, s7),
            Op(OpID::ID, "c", s8)
        ));

        // a.3(i) = c
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::ID, "a"), s1},
            SourcedToken{Token(TokenID::DOT), s2},
            SourcedToken{Token(TokenID::PLAIN_INT, std::int32_t(3)), s3},
            SourcedToken{Token(TokenID::LEFT_CURVED), s4},
            SourcedToken{Token(TokenID::ID, "i"), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6},
            SourcedToken{Token(TokenID::EQUALS), s7},
            SourcedToken{Token(TokenID::ID, "c"), s8}
        )) == vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, std::int32_t(3), s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "i", s5),
            Op(OpID::END, s6),
            Op(OpID::SET, s7),
            Op(OpID::ID, "c", s8)
        ));
    }

    SECTION("Var") {

    }

    SECTION("While") {
        // while a: f()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::WHILE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::ID, "f"), s4},
            SourcedToken{Token(TokenID::LEFT_CURVED), s5},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s6}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::WHILE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::ID, "f", s4),
            Op(OpID::CALL, s5),
            Op(OpID::GROUP, s5),
            Op(OpID::NOTHING, s6),
            Op(OpID::END, s6),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // while a:
        //     f()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::WHILE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::ID, "f"), s6},
            SourcedToken{Token(TokenID::LEFT_CURVED), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::WHILE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::ID, "f", s6),
            Op(OpID::CALL, s7),
            Op(OpID::GROUP, s7),
            Op(OpID::NOTHING, s8),
            Op(OpID::END, s8),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // while a:
        //     f()
        // else:
        //     g()
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::WHILE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2},
            SourcedToken{Token(TokenID::COLON), s3},
            SourcedToken{Token(TokenID::NEWLINE), s4},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s5},
            SourcedToken{Token(TokenID::ID, "f"), s6},
            SourcedToken{Token(TokenID::LEFT_CURVED), s7},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s8},
            SourcedToken{Token(TokenID::NEWLINE), s9},
            SourcedToken{Token(TokenID::ELSE), s10},
            SourcedToken{Token(TokenID::COLON), s11},
            SourcedToken{Token(TokenID::NEWLINE), s12},
            SourcedToken{Token(TokenID::SPACE, std::uint32_t(4)), s13},
            SourcedToken{Token(TokenID::ID, "g"), s14},
            SourcedToken{Token(TokenID::LEFT_CURVED), s15},
            SourcedToken{Token(TokenID::RIGHT_CURVED), s16}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::WHILE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::ID, "f", s6),
            Op(OpID::CALL, s7),
            Op(OpID::GROUP, s7),
            Op(OpID::NOTHING, s8),
            Op(OpID::END, s8),
            Op(OpID::STMT, s9),
            Op(OpID::END, s10),
            Op(OpID::STMT, s10),
            Op(OpID::ELSE, s10),
            Op(OpID::BODY, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::ID, "g", s14),
            Op(OpID::CALL, s15),
            Op(OpID::GROUP, s15),
            Op(OpID::NOTHING, s16),
            Op(OpID::END, s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));

        // while a
        REQUIRE(feed_all(vec(
            SourcedToken{Token(TokenID::WHILE), s1},
            SourcedToken{Token(TokenID::ID, "a"), s2}
        )) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::WHILE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        ));
    }
}