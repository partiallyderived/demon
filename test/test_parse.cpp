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
#include "dl/res.hpp"
#include "dl/span.hpp"

#include "util.hpp"

using namespace dl;

TEST_CASE("Parser Core", "[parse]") {
    auto parser = ParserImpl();
    auto s0 = Span(0, 0);
    auto s1 = Span(1, 1);
    auto s2 = Span(2, 2);
    auto s3 = Span(3, 3);

    auto on_orientation = [&](auto&& token, Orientation orientation, Span src) {
        parser.orientation = orientation;
        if constexpr (std::is_same_v<decltype(token), Token>)
            return parser.feed(std::move(token), src);
        else
            return parser.feed(Token(std::move(token)), src);
    };

    auto on_orientation_op = [&](
        auto&& token, Orientation orientation, Span src
    ) {
        ErrPtr err = on_orientation(std::move(token), orientation, src);
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
        auto&& token, Orientation orientation, Span src
    ) {
        ErrPtr err = on_orientation(std::move(token), orientation, src);
        if (err)
            return Res<std::vector<Op>>(std::move(err));
        std::vector<Op> res;
        while (true) {
            Op op = parser.next();
            if (op.id == OpID::WAITING)
                break;
            res.push_back(std::move(op));
        }
        return Res<std::vector<Op>>(std::move(res));
    };

    auto on_start = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::START, src);
    };

    auto on_before = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::BEFORE, src);
    };

    auto on_after = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::AFTER, src);
    };

    auto on_after_star = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::AFTER_STAR, src);
    };

    auto on_optional = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::OPTIONAL, src);
    };

    auto on_indenting = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::INDENTING, src);
    };

    auto on_end = [&](auto&& token, Span src) {
        return on_orientation(std::move(token), Orientation::END, src);
    };

    auto on_start_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::START, src);
    };

    auto on_before_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::BEFORE, src);
    };

    auto on_after_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::AFTER, src);
    };

    auto on_after_star_op = [&](auto&& token, Span src) {
        return on_orientation_op(
            std::move(token), Orientation::AFTER_STAR, src
        );
    };

    auto on_optional_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::OPTIONAL, src);
    };

    auto on_indenting_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::INDENTING, src);
    };

    auto on_end_op = [&](auto&& token, Span src) {
        return on_orientation_op(std::move(token), Orientation::END, src);
    };

    auto on_start_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(std::move(token), Orientation::START, src);
    };

    auto on_before_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(std::move(token), Orientation::BEFORE, src);
    };

    auto on_after_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(std::move(token), Orientation::AFTER, src);
    };

    auto on_after_star_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(
            std::move(token), Orientation::AFTER_STAR, src
        );
    };

    auto on_optional_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(std::move(token), Orientation::OPTIONAL, src);
    };

    auto on_indenting_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(
            std::move(token), Orientation::INDENTING, src
        );
    };

    auto on_end_ops = [&](auto&& token, Span src) {
        return on_orientation_ops(std::move(token), Orientation::END, src);
    };

    SECTION("ParserImpl Init") {
        REQUIRE(parser.depth == 0);
        REQUIRE(parser.line_start);
        REQUIRE(parser.orientation == Orientation::START);
        REQUIRE(parser.queue.empty());
        REQUIRE(parser.contexts.empty());
        REQUIRE_FALSE(parser.parsing_loop_vars);
    }

    SECTION("Hash") {
        REQUIRE(on_start(TokenID::HASH, s1) == nullptr);
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
            vec(
                Op(OpID::END, s1),
                Op(OpID::STMT, s1),
                Op(OpID::END, s1),
                Op(OpID::STMT, s1)
            )
        );
        REQUIRE(parser.orientation == Orientation::START);
    }

    SECTION("Newline") {
        parser.line_start = false;
        REQUIRE(on_start_ops(TokenID::NEWLINE, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::START);
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
            vec(Op(OpID::NOTHING, s1), Op(OpID::STMT, s1))
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
            REQUIRE(on_before_ops(TokenID::NEWLINE, s1) == std::vector<Op>());
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.line_start);

            parser.line_start = false;
            REQUIRE(on_after_ops(TokenID::NEWLINE, s1) == std::vector<Op>());
            REQUIRE(parser.orientation == Orientation::AFTER);
            REQUIRE(parser.line_start);

            parser.line_start = false;
            REQUIRE(
                on_after_star_ops(TokenID::NEWLINE, s1) == std::vector<Op>()
            );
            REQUIRE(parser.orientation == Orientation::AFTER_STAR);
            REQUIRE(parser.line_start);
        }

        SECTION("After Construct End") {
            // This section tests that a construct is ended in the AFTER,
            // OPTIONAL, and END orientations when a newline is found when in
            // the CONSTRUCT_END context.
            parser.prev_src = s1;
            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_after_ops(TokenID::NEWLINE, s2) ==
                vec(
                    Op(OpID::STMT, s2), Op(OpID::END, s1), Op(OpID::STMT, s1)
                )
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_optional_ops(TokenID::NEWLINE, s2) ==
                vec(
                    Op(OpID::NOTHING, s2),
                    Op(OpID::STMT, s2),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_end_ops(TokenID::NEWLINE, s2) ==
                vec(
                    Op(OpID::STMT, s2), Op(OpID::END, s1), Op(OpID::STMT, s1)
                )
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
            parser.prev_src = s1;
            REQUIRE(
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(4)), s2)
                == vec(Op(OpID::END, s1), Op(OpID::STMT, s1))
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
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(12)), s2)
                == vec(
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
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
                on_start_ops(Token(TokenID::SPACE, std::uint32_t(12)), s2)
                == vec(
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
                    Op(OpID::STMT, s1)
                )
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
                on_start_ops(TokenID::TRUE, s2) ==
                vec(
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::TRUE, s2)
                )
            );
            REQUIRE(parser.depth == 0);
            REQUIRE(parser.contexts.empty());
        }

        SECTION("Leading, Indenting Orientation") {
            // When encountering leading space in the INDENTING orientation, the
            // parser expects an indent to occur.

            parser.line_start = true;
            REQUIRE(
                on_indenting_op(Token(TokenID::SPACE, std::uint32_t(4)), s1)
                == Op(OpID::BLOCK, Span(1, 2, 1, 2))
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
                on_indenting_op(Token(TokenID::SPACE, std::uint32_t(8)), s1)
                == Op(OpID::BLOCK, Span(1, 2, 1, 2))
            );
            REQUIRE(parser.depth == 2);
            REQUIRE(parser.orientation == Orientation::START);
            REQUIRE(
                parser.contexts == std::vector{Context::BLOCK, Context::BLOCK}
            );
            REQUIRE_FALSE(parser.line_start);

            // Now try to dedent once, which is an err in INDENTING orientation.
            parser.line_start = true;
            ErrPtr err = on_indenting(
                Token(TokenID::SPACE, std::uint32_t(4)), s1
            );
            REQUIRE(err != nullptr);
            REQUIRE(*err == UnderIndentErr(s1));

            // Now try with exact space (note that an indent is expected, not
            // a continuation of the same indentation).
            parser.line_start = true;
            err = on_indenting(Token(TokenID::SPACE, std::uint32_t(8)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == ExpectedIndentErr(s1));

            // Now try with less space than another indent.
            parser.line_start = true;
            err = on_indenting(Token(TokenID::SPACE, std::uint32_t(10)), s1);
            REQUIRE(err != nullptr);
            REQUIRE(*err == IncompleteIndentErr(s1));

            // Now try to indent twice, which is an error.
            parser.line_start = true;
            err = on_indenting(Token(TokenID::SPACE, std::uint32_t(16)), s1);
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
        REQUIRE(on_start_ops(TokenID::END_OF_FILE, s1) == std::vector<Op>());

        ErrPtr err = on_before(TokenID::END_OF_FILE, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::END_OF_FILE, s1));

        REQUIRE(on_after_op(TokenID::END_OF_FILE, s1) == Op(OpID::STMT, s1));

        REQUIRE(
            on_optional_ops(TokenID::END_OF_FILE, s1) ==
            vec(Op(OpID::NOTHING, s1), Op(OpID::STMT, s1))
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
            parser.prev_src = s1;
            REQUIRE(
                on_start_ops(TokenID::END_OF_FILE, s2) ==
                vec(
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
            );
        }

        SECTION("After Construct End") {
            // This section tests that a construct is ended in the AFTER,
            // OPTIONAL, and END orientations when a newline is found when in
            // the CONSTRUCT_END context.
            parser.contexts.push_back(Context::CONSTRUCT_END);
            parser.prev_src = s1;
            REQUIRE(
                on_after_ops(TokenID::END_OF_FILE, s2) ==
                vec(
                    Op(OpID::STMT, s2),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_optional_ops(TokenID::END_OF_FILE, s2) ==
                vec(
                    Op(OpID::NOTHING, s2),
                    Op(OpID::STMT, s2),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
            );
            REQUIRE(parser.contexts.empty());

            parser.contexts.push_back(Context::CONSTRUCT_END);
            REQUIRE(
                on_end_ops(TokenID::END_OF_FILE, s2) ==
                vec(
                    Op(OpID::STMT, s2),
                    Op(OpID::END, s1),
                    Op(OpID::STMT, s1)
                )
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
                vec(Op(OpID::UNPACK_ARGS, s2), op.copy())
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
            TokenID::AS,
            TokenID::BANG_EQUALS,
            TokenID::CAROT,
            TokenID::CAROT_EQUALS,
            TokenID::COMMA,
            TokenID::DOUBLE_EQUALS,
            TokenID::DOUBLE_LEFT_ANGLE,
            TokenID::DOUBLE_LEFT_ANGLE_EQUALS,
            TokenID::DOUBLE_PIPE,
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
                REQUIRE(on_after_star_ops(t, s1) == vec(
                    Op(OpID::POS_KW_SEP, s2), op.copy()
                ));
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
            TokenID::DOUBLE_STAR,
            TokenID::DOT,
            TokenID::MINUS,
            TokenID::PERCENT,
            TokenID::PIPE,
            TokenID::STAR
        }) {
            Op unary_op = Op(tokeninfo(t).op1, s1);
            Op binary_op = Op(tokeninfo(t).op2, s1);

            REQUIRE(on_start_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            REQUIRE(on_before_op(t, s1) == unary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            REQUIRE(on_after_op(t, s1) == binary_op);
            REQUIRE(parser.orientation == Orientation::BEFORE);

            // Check that expansion is inferred in AFTER_STAR orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(t, s1) ==
                vec(Op(OpID::UNPACK_ARGS, s2), unary_op.copy())
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
        REQUIRE(on_before_ops(TokenID::STAR, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::AFTER_STAR);
    }

    SECTION("Value Tokens") {
        for (Token& t: vec(
            Token(TokenID::CHAR, std::uint32_t(100)),
            Token(TokenID::FALSE),
            Token(TokenID::FLOAT_TAIL, "1e7"),
            Token(TokenID::ID, "asdf"),
            Token(TokenID::NONE),
            Token(TokenID::NULL_),
            Token(TokenID::PERCENT_DOUBLE_STAR),
            Token(TokenID::PERCENT_STAR),
            Token(TokenID::PLAIN_INT, "1000"),
            Token(TokenID::NUMBER, std::int32_t(1000)),
            Token(TokenID::STRING, "a,s,d,f"),
            Token(TokenID::THIS),
            Token(TokenID::TRUE)
        )) {
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
            REQUIRE(on_before_op(Token(t.id, std::move(d)), s1) == op1);
            REQUIRE(parser.orientation == Orientation::AFTER);

            d = t.data;
            REQUIRE(
                on_after_ops(Token(t.id, std::move(d)), s1) ==
                vec(op2.copy(), op1.copy())
            );
            REQUIRE(parser.orientation == Orientation::AFTER);

            // Check that expansion is inferred in AFTER_STAR orientation.
            d = t.data;
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(Token(t.id, std::move(d)), s1) ==
                vec(Op(OpID::UNPACK_ARGS, s2), op1.copy())
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

            REQUIRE(on_before_op(t, s1) == op1);
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_after_ops(t, s1) == vec(op2.copy(), op1.copy()));
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            // Check that expansion is inferred in AFTER_STAR orientation.
            parser.prev = TokenID::STAR;
            parser.prev_src = s2;
            REQUIRE(
                on_after_star_ops(t, s1) ==
                vec(Op(OpID::UNPACK_ARGS, s2), op1.copy())
            );
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{ctx});
            parser.contexts.clear();

            REQUIRE(on_optional_op(t, s1) == op1.copy());
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
                on_before_ops(t, s1) == vec(Op(OpID::NOTHING, s1), op.copy())
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
                vec(Op(OpID::POS_KW_SEP, s2), op.copy())
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
                TokenID::WHILE
            }
        ) {
            // op is the "unary" operator corresponding to the construct.
            Op op = Op(tokeninfo(t).op1, s1);

            REQUIRE(
                on_start_ops(t, s1) == vec(Op(OpID::CONSTRUCT, s1), op.copy())
            );
            REQUIRE(parser.orientation == Orientation::BEFORE);
            REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT});
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);
            parser.contexts.clear();

            // START is the only valid orientation for CONSTRUCT_FIRST.
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

        // Check that if we are in the CONSTRUCT context and a token which is
        // neither a CONSTRUCT_MIDDLE, CONSTRUCT_LAST, or SPECIAL token is
        // received, then the construct is closed.
        parser.contexts.push_back(Context::CONSTRUCT);
        REQUIRE(on_start_ops(Token(TokenID::ID, "asdf"), s1) == vec(
            Op(OpID::END, s1),
            Op(OpID::STMT, s1),
            Op(OpID::ID, "asdf", s1)
        ));
        REQUIRE(parser.contexts.empty());
    }

    SECTION("Construct Middle Tokens") {
        for(
            TokenID t: std::vector{
                TokenID::CASE, TokenID::ELIF, TokenID::EXCEPT
            }
        ) {
            parser.contexts.clear();

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
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);

            // START is the only valid orientation for CONSTRUCT_MIDDLE.
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
            REQUIRE(parser.prev == t);
            REQUIRE(parser.prev_src == s1);
            REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT_END});

            // START is the only valid orientation for CONSTRUCT_LAST.
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
        REQUIRE(on_start_ops(TokenID::FOR, s1) == vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1)
        ));
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

    SECTION("Colon Token") {
        // No ops pushed immediately when colon is outside brackets.
        REQUIRE(on_before_ops(TokenID::COLON, s1) == std::vector<Op>());

        // Orientation doesn't change yet either.
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.prev == TokenID::COLON);
        REQUIRE(parser.prev_src == s1);

        // After another token is fed, we know we're not indenting, so the colon
        // operator is pushed as well as the new operator.
        REQUIRE(
            on_before_ops(Token(TokenID::ID, "x"), s2) ==
            vec(Op(OpID::SYMBOL, s1), Op(OpID::ID, "x", s2))
        );
        // Now orientation has changed.
        REQUIRE(parser.orientation == Orientation::AFTER);

        // Set colon to prev and repeat, this time with another colon.
        // Now we should only get one operator emitted at first.
        parser.prev = TokenID::COLON;
        parser.prev_src = s1;
        REQUIRE(on_before_op(TokenID::COLON, s2) == Op(OpID::SYMBOL, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        // Now repeat the tests in after orientation.
        parser.prev = TokenID::SPACE;
        parser.prev_src = s0;
        REQUIRE(on_after_ops(TokenID::COLON, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::AFTER);
        REQUIRE(parser.prev == TokenID::COLON);
        REQUIRE(parser.prev_src == s1);

        REQUIRE(
            on_after_ops(Token(TokenID::ID, "Int"), s2) ==
            vec(Op(OpID::TYPE_LABEL, s1), Op(OpID::ID, "Int", s2))
        );

        parser.prev = TokenID::COLON;
        parser.prev_src = s1;
        REQUIRE(on_after_op(TokenID::COLON, s2) == Op(OpID::TYPE_LABEL, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        // Now try inside brackets.
        parser.prev = TokenID::SPACE;
        parser.prev_src = s0;
        parser.contexts.push_back(Context::CURVED);
        REQUIRE(on_before_op(TokenID::COLON, s1) == Op(OpID::SYMBOL, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        parser.prev = TokenID::SPACE;
        parser.prev_src = s0;
        REQUIRE(on_after_op(TokenID::COLON, s1) == Op(OpID::TYPE_LABEL, s1));
        REQUIRE(parser.orientation == Orientation::BEFORE);

        parser.contexts.clear();

        // Now try with a newline following the colon.
        REQUIRE(parser.prev == TokenID::COLON);
        REQUIRE(parser.prev_src == s1);
        REQUIRE(on_before_op(TokenID::NEWLINE, s2) == Op(OpID::BODY, s1));
        REQUIRE(parser.orientation == Orientation::INDENTING);

        // Now try with after orientation.
        REQUIRE(parser.prev == TokenID::COLON);
        REQUIRE(parser.prev_src == s1);
        REQUIRE(on_after_op(TokenID::NEWLINE, s3) == Op(OpID::LABEL, s1));
        REQUIRE(parser.orientation == Orientation::INDENTING);
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
        REQUIRE(parser.prev == TokenID::ELSE);
        REQUIRE(parser.prev_src == s1);
        REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT_END});
        parser.contexts.clear();

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
            vec(Op(OpID::CONSTRUCT, s1), Op(OpID::IF, s1))
        );
        REQUIRE(parser.orientation == Orientation::BEFORE);
        REQUIRE(parser.contexts == std::vector{Context::CONSTRUCT});
        REQUIRE(parser.prev == TokenID::IF);
        REQUIRE(parser.prev_src == s1);
        parser.contexts.clear();

        ErrPtr err = on_before(TokenID::IF, s1);
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
        REQUIRE(on_start_ops(TokenID::NOT, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);
        REQUIRE(parser.prev == TokenID::NOT);
        REQUIRE(parser.prev_src == s1);

        // Check that unary not operator is emitted if a value is fed.
        REQUIRE(parser.feed(Token(TokenID::TRUE), s2) == nullptr);

        REQUIRE(parser.next() == Op(OpID::NOT, s1));
        REQUIRE(parser.next() == Op(OpID::TRUE, s2));
        REQUIRE(parser.queue.empty());

        REQUIRE(parser.orientation == Orientation::AFTER);
        parser.queue.clear();

        REQUIRE(on_before_ops(TokenID::NOT, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);

        // Check that "not in" operator is emitted if "in" token is fed.
        REQUIRE(parser.feed(Token(TokenID::IN), s2) == nullptr);

        REQUIRE(parser.next() == Op(OpID::NOT_IN, Span(s1, s2)));
        REQUIRE(parser.queue.empty());
        REQUIRE(parser.orientation == Orientation::BEFORE);
        parser.queue.clear();

        ErrPtr err = on_after(TokenID::NOT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::NOT, s1));

        REQUIRE(on_optional_ops(TokenID::NOT, s1) == std::vector<Op>());
        REQUIRE(parser.orientation == Orientation::AFTER_NOT);

        err = on_end(TokenID::NOT, s1);
        REQUIRE(err != nullptr);
        REQUIRE(*err == UnexpectedTokenErr(TokenID::NOT, s1));
    }
}
