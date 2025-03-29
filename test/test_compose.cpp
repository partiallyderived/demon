#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/compose/composerimpl.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"

#include "util.hpp"

using namespace dl;

auto s1 = Span(1, 1);
auto s2 = Span(2, 2);
auto s3 = Span(3, 3);
auto s_end = Span(4, 4);

Comp feed_all(std::vector<Op>&& ops) {
    auto composer = ComposerImpl();
    for (Op& o: ops)
        composer.feed(std::move(o));
    composer.feed(Op(OpID::STMT, s_end));
    if (composer.queue.size() != 1)
        // Use this Comp to indicate that no or multiple Comps were found, a
        // test failure when using feed_all, which for convenience assumes a
        // single Comp was expected.
        return Comp(OpID::WAITING, s1);
    return composer.next();
}

TEST_CASE("Composer Core", "[compose]") {
    SECTION("Nullary Operators") {
        for (OpID o: std::vector{OpID::BREAK, OpID::CONTINUE}) {
            REQUIRE(feed_all(vec(Op(o, s1))) == Comp(o, s1));
        }
    }

    SECTION("Unary Operators") {
        for (OpID o: std::vector{
            OpID::ADDR,
            OpID::ADDR_TYPE,
            OpID::BNOT,
            OpID::BODY,
            OpID::CASE,
            OpID::DEF,
            OpID::EXCEPT,
            OpID::ELIF,
            OpID::ELSE,
            OpID::FINALLY,
            OpID::FOR,
            OpID::IF,
            OpID::LAMBDA,
            OpID::LITERALLY,
            OpID::MATCH,
            OpID::NEG,
            OpID::NOT,
            OpID::NOT_IN,
            OpID::RAISE,
            OpID::RETURN,
            OpID::SYMBOL,
            OpID::TRY,
            OpID::WHILE,
            OpID::UNPACK_ARGS,
            OpID::UNPACK_KWARGS,
            OpID::UP
        }) {
            // Use OpID::TRUE as an easy way to feed a value to the composer for
            // the unary operators to operate on.
            // Of course, most of the comps don't make sense, but it is not the
            // job of composer to make sense of the given operators, just to
            // compose them.
            REQUIRE(
                feed_all(vec(Op(o, s1), Op(OpID::TRUE, s2))) ==
                Comp(o, Comp(OpID::TRUE, s2), s1)
            );
        }
    }

    SECTION("Binary Operators") {
        for (OpID o: std::vector{
            OpID::ADD,
            OpID::AND,
            OpID::BAND,
            OpID::BIND,
            OpID::BOR,
            OpID::BXOR,
            OpID::CALL,
            OpID::CONCAT,
            OpID::DIV,
            OpID::EQ,
            OpID::GET,
            OpID::GTE,
            OpID::IADD,
            OpID::IBAND,
            OpID::IBOR,
            OpID::IBXOR,
            OpID::IDIV,
            OpID::ILSH,
            OpID::IMOD,
            OpID::IMUL,
            OpID::IN,
            OpID::IPOW,
            OpID::IRSH,
            OpID::ISUB,
            OpID::LABEL,
            OpID::LSH,
            OpID::LT,
            OpID::LTE,
            OpID::MOD,
            OpID::MUL,
            OpID::NEQ,
            OpID::OR,
            OpID::POW,
            OpID::RETURNS,
            OpID::RSH,
            OpID::SEP,
            OpID::SET,
            OpID::SUB,
            OpID::SUFFIX,
            OpID::TERNARY_ELSE,
            OpID::TERNARY_IF,
            OpID::TYPE_LABEL
        }) {
            REQUIRE(feed_all(vec(
                Op(OpID::TRUE, s1), // lhs
                Op(o, s2), // operator
                Op(OpID::FALSE, s3) // rhs
            )) == Comp(o, Comp(OpID::TRUE, s1), Comp(OpID::FALSE, s3), s2));
        }
    }

    SECTION("Singleton Operators") {
        for (OpID o: std::vector{
            OpID::FALSE,
            OpID::LAMBDA_ARGS,
            OpID::LAMBDA_KWARGS,
            OpID::NONE,
            OpID::NOTHING,
            OpID::POS_KW_SEP,
            OpID::THIS,
            OpID::TRUE
        }) {
            REQUIRE(feed_all(vec(Op(o, s1))) == Comp(o, s1));
        }
    }

    SECTION("Data Operators") {
        for (Op& o: vec(
            Op(OpID::CHAR, 50, s1),
            Op(OpID::FLOAT_TAIL, "1e7", s1),
            Op(OpID::ID, "asdf", s1),
            Op(OpID::PLAIN_INT, 3, s1),
            Op(OpID::STRING, "a string", s1)
        )) {
            // Copy data for verification since it will be moved.
            Data d = o.data;
            REQUIRE(
                feed_all(vec(std::move(o))) == Comp(o.id, std::move(d), s1)
            );
        }
    }

    SECTION("Start Operators") {
        for (OpID o: std::vector{OpID::GROUP, OpID::LIST, OpID::ENCLOSURE}) {
            REQUIRE(feed_all(vec(
                Op(o, s1),
                Op(OpID::TRUE, s1),
                Op(OpID::END, s2)
            )) == Comp(o, Comp(OpID::TRUE, s1), Span(s1, s2)));
        }
    }

    SECTION("Block Operator") {
        // A Block with the contents:
        //     true
        //     false
        //     none
        // Given spans are realistic.
        REQUIRE(feed_all(vec(
            Op(OpID::BLOCK, Span(2, 5)),
            Op(OpID::TRUE, Span(2, 5, 4)),
            Op(OpID::STMT, Span(2, 9)),
            Op(OpID::FALSE, Span(3, 5, 5)),
            Op(OpID::STMT, Span(3, 10)),
            Op(OpID::NONE, Span(4, 5, 4)),
            Op(OpID::STMT, Span(4, 9)),
            Op(OpID::END, Span(4, 8))
        )) == Comp(
            OpID::BLOCK,
            vec(
                Comp(OpID::TRUE, Span(2, 5, 4)),
                Comp(OpID::FALSE, Span(3, 5, 5)),
                Comp(OpID::NONE, Span(4, 5, 4))
            ), 
            Span(2, 5, 4, 8)
        ));
    }

    SECTION("Construct Operator") {
        /* This composes the following code:
        if true:
            false
        elif none:
            null
        else:
            this
        */
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, Span(1, 1)),
            Op(OpID::IF, Span(1, 1, 2)),
            Op(OpID::TRUE, Span(1, 4, 4)),
            Op(OpID::LABEL, Span(1, 9)),
            Op(OpID::BLOCK, Span(2, 5)),
            Op(OpID::FALSE, Span(2, 5, 5)),
            Op(OpID::STMT, Span(2, 10)),
            Op(OpID::END, Span(2, 9)),
            Op(OpID::STMT, Span(2, 9)),
            Op(OpID::ELIF, Span(3, 1, 4)),
            Op(OpID::NONE, Span(3, 6, 4)),
            Op(OpID::LABEL, Span(3, 10)),
            Op(OpID::BLOCK, Span(4, 5)),
            Op(OpID::NULL_, Span(4, 5, 4)),
            Op(OpID::STMT, Span(4, 9)),
            Op(OpID::END, Span(4, 8)),
            Op(OpID::STMT, Span(4, 8)),
            Op(OpID::ELSE, Span(5, 1, 4)),
            Op(OpID::BODY, Span(5, 5)),
            Op(OpID::BLOCK, Span(6, 5)),
            Op(OpID::THIS, Span(6, 5, 4)),
            Op(OpID::STMT, Span(6, 9)),
            Op(OpID::END, Span(6, 8)),
            Op(OpID::STMT, Span(6, 8)),
            Op(OpID::END, Span(6, 8))
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::TRUE, Span(1, 4, 4)),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(OpID::FALSE, Span(2, 5, 5))),
                            Span(2, 5, 5)
                        ),
                        Span(1, 9)
                    ),
                    Span(1, 1, 2)
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::NONE, Span(3, 6, 4)),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(OpID::NULL_, Span(4, 5, 4))),
                            Span(4, 5, 4)
                        ),
                        Span(3, 10)
                    ),
                    Span(3, 1, 4)
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(OpID::THIS, Span(6, 5, 4))),
                            Span(6, 5, 4)
                        ),
                        Span(5, 5)
                    ),
                    Span(5, 1, 4)
                )
            ),
            Span(1, 1, 6, 8)
        ));
    }

    SECTION("Precedence") {
        // a = 1 + 2 * 3 ** 4
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s1),
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::MUL, s1),
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "4", s1)
        )) == Comp(
            OpID::SET,
            Comp(OpID::ID, "a", s1),
            Comp(
                OpID::ADD,
                Comp(OpID::PLAIN_INT, "1", s1),
                Comp(
                    OpID::MUL,
                    Comp(OpID::PLAIN_INT, "2", s1),
                    Comp(
                        OpID::POW,
                        Comp(OpID::PLAIN_INT, "3", s1),
                        Comp(OpID::PLAIN_INT, "4", s1),
                        s1
                    ),
                    s1
                ),
                s1
            ),
            s1
        ));

        // Now the other way: a = 1 ** 2 * 3 + 4
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s1),
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::MUL, s1),
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "4", s1)
        )) == Comp(
            OpID::SET,
            Comp(OpID::ID, "a", s1),
            Comp(
                OpID::ADD,
                Comp(
                    OpID::MUL,
                    Comp(
                        OpID::POW,
                        Comp(OpID::PLAIN_INT, "1", s1),
                        Comp(OpID::PLAIN_INT, "2", s1),
                        s1
                    ),
                    Comp(OpID::PLAIN_INT, "3", s1),
                    s1
                ),
                Comp(OpID::PLAIN_INT, "4", s1),
                s1
            ),
            s1
        ));

        // Left-associative example: 1 + 2 + 3
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "3", s1)
        )) == Comp(
            OpID::ADD,
            Comp(
                OpID::ADD,
                Comp(OpID::PLAIN_INT, "1", s1),
                Comp(OpID::PLAIN_INT, "2", s1),
                s1
            ),
            Comp(OpID::PLAIN_INT, "3", s1),
            s1
        ));

        // Right-associative example: 1 ** 2 ** 3
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "3", s1)
        )) == Comp(
            OpID::POW,
            Comp(OpID::PLAIN_INT, "1", s1),
            Comp(
                OpID::POW,
                Comp(OpID::PLAIN_INT, "2", s1),
                Comp(OpID::PLAIN_INT, "3", s1),
                s1
            ),
            s1
        ));

        // @Int@: Postfix should apply first.
        REQUIRE(feed_all(vec(
            Op(OpID::ADDR, s1),
            Op(OpID::ID, "Int", s1),
            Op(OpID::ADDR_TYPE, s1)
        )) == Comp(
            OpID::ADDR,
            Comp(OpID::ADDR_TYPE, Comp(OpID::ID, "Int", s1), s1),
            s1
        ));

        // 1 + -2: Unary should apply first.
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::NEG, s1),
            Op(OpID::PLAIN_INT, "2", s1)
        )) == Comp(
            OpID::ADD,
            Comp(OpID::PLAIN_INT, "1", s1),
            Comp(OpID::NEG, Comp(OpID::PLAIN_INT, "2", s1), s1),
            s1
        ));
    }
}
