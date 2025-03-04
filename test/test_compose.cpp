#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/compose/composerimpl.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

#include "util.hpp"

using namespace dl;

Comp feed_all(std::vector<Op> ops) {
    auto composer = ComposerImpl();
    for (Op o: ops)
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
            REQUIRE(feed_all({Op(o, s1)}) == Comp(o, s1));
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
            OpID::MATCH,
            OpID::NEG,
            OpID::NOT,
            OpID::NOT_IN,
            OpID::RAISE,
            OpID::RETURN,
            OpID::SYMBOL,
            OpID::TRY,
            OpID::TYPE,
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
                feed_all({Op(o, s1), Op(OpID::TRUE, s2)}) ==
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
            OpID::MATCHING,
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
            REQUIRE(feed_all({
                Op(OpID::TRUE, s1), // lhs
                Op(o, s2), // operator
                Op(OpID::FALSE, s3) // rhs
            }) == Comp(o, Comp(OpID::TRUE, s1), Comp(OpID::FALSE, s3), s2));
        }
    }

    SECTION("Singleton Operators") {
        for (OpID o: std::vector{
            OpID::FALSE,
            OpID::NONE,
            OpID::NOTHING,
            OpID::POS_KW_SEP,
            OpID::THIS,
            OpID::TRUE,
            OpID::VARS
        }) {
            REQUIRE(feed_all({Op(o, s1)}) == Comp(o, s1));
        }
    }

    SECTION("Data Operators") {
        for (Op o: std::vector{
            Op(OpID::CHAR, 50, s1),
            Op(OpID::FLOAT_TAIL, "1e7", s1),
            Op(OpID::ID, "asdf", s1),
            Op(OpID::PLAIN_INT, 3, s1),
            Op(OpID::STRING, "a string", s1)
        }) {
            // Copy data for verification since it will be moved.
            Data d = o.data;
            REQUIRE(
                feed_all({o}) == Comp(o.id, std::move(d), s1)
            );
        }
    }

    SECTION("Start Operators") {
        for (OpID o: std::vector{OpID::GROUP, OpID::LIST, OpID::ENCLOSURE}) {
            REQUIRE(feed_all({
                Op(o, s1),
                Op(OpID::TRUE, s1),
                Op(OpID::END, s1)
            }) == Comp(o, Comp(OpID::TRUE, s1), s1));
        }
    }

    SECTION("Block Operator") {
        Comp block_contents[] = {
            Comp(OpID::TRUE, s2), Comp(OpID::FALSE, s3), Comp(OpID::NONE, s4)
        };
        REQUIRE(feed_all({
            Op(OpID::BLOCK, s1),
            Op(OpID::TRUE, s2),
            Op(OpID::STMT, s1),
            Op(OpID::FALSE, s3),
            Op(OpID::STMT, s1),
            Op(OpID::NONE, s4),
            Op(OpID::STMT, s1),
            Op(OpID::END, s1)
        }) == Comp(
            OpID::BLOCK,
            std::vector(
                std::make_move_iterator(block_contents), 
                std::make_move_iterator(block_contents + 3)
            ), 
            s1
        ));
    }

    SECTION("Construct Operator") {
        /* This composes the following code:
        if true: false
        elif none:
            null
        else: this
        */
        std::vector<Comp> block_contents;
        block_contents.push_back(Comp(OpID::NULL_, s10));
        Comp construct_contents[] = {
            Comp(
                OpID::IF,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::TRUE, s3),
                    Comp(OpID::FALSE, s5),
                    s4
                ),
                s2
            ), Comp(
                OpID::ELIF,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::NONE, s7),
                    Comp(OpID::BLOCK, std::move(block_contents), s9),
                    s8
                ),
                s6
            ), Comp(
                OpID::ELSE, Comp(OpID::BODY, Comp(OpID::THIS, s13), s12), s11
            )
        };

        REQUIRE(feed_all({
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s2),
            Op(OpID::TRUE, s3),
            Op(OpID::LABEL, s4),
            Op(OpID::FALSE, s5),
            Op(OpID::STMT, s1),
            Op(OpID::ELIF, s6),
            Op(OpID::NONE, s7),
            Op(OpID::LABEL, s8),
            Op(OpID::BLOCK, s9),
            Op(OpID::NULL_, s10),
            Op(OpID::STMT, s1),
            Op(OpID::END, s1),
            Op(OpID::STMT, s1),
            Op(OpID::ELSE, s11),
            Op(OpID::BODY, s12),
            Op(OpID::THIS, s13),
            Op(OpID::STMT, s1),
            Op(OpID::END, s1)
        }) == Comp(
            OpID::CONSTRUCT,
            std::vector(
                std::make_move_iterator(construct_contents),
                std::make_move_iterator(construct_contents + 3)
            ),
            s1
        ));
    }

    SECTION("Precedence") {
        // a = 1 + 2 * 3 ** 4
        REQUIRE(feed_all({
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s1),
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::MUL, s1),
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "4", s1)
        }) == Comp(
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
        REQUIRE(feed_all({
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s1),
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::MUL, s1),
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "4", s1)
        }) == Comp(
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
        REQUIRE(feed_all({
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::ADD, s1),
            Op(OpID::PLAIN_INT, "3", s1)
        }) == Comp(
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
        REQUIRE(feed_all({
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "2", s1),
            Op(OpID::POW, s1),
            Op(OpID::PLAIN_INT, "3", s1)
        }) == Comp(
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
        REQUIRE(feed_all({
            Op(OpID::ADDR, s1),
            Op(OpID::ID, "Int", s1),
            Op(OpID::ADDR_TYPE, s1)
        }) == Comp(
            OpID::ADDR,
            Comp(OpID::ADDR_TYPE, Comp(OpID::ID, "Int", s1), s1),
            s1
        ));

        // 1 + -2: Unary should apply first.
        REQUIRE(feed_all({
            Op(OpID::PLAIN_INT, "1", s1),
            Op(OpID::ADD, s1),
            Op(OpID::NEG, s1),
            Op(OpID::PLAIN_INT, "2", s1)
        }) == Comp(
            OpID::ADD,
            Comp(OpID::PLAIN_INT, "1", s1),
            Comp(OpID::NEG, Comp(OpID::PLAIN_INT, "2", s1), s1),
            s1
        ));
    }
}

TEST_CASE("Composer Input/Output", "[compose]") {
    SECTION("ADD") {
        // a + b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ADD, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::ADD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Addr") {
        // @a
        REQUIRE(feed_all(vec(
            Op(OpID::ADDR, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::ADDR, Comp(OpID::ID, "a", s2), s1
        ));
    }

    SECTION("AddrType") {
        // Int@
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "Int", s1),
            Op(OpID::ADDR_TYPE, s2)
        )) == Comp(
            OpID::ADDR_TYPE, Comp(OpID::ID, "Int", s1), s2
        ));
    }

    SECTION("And") {
        // true and false
        REQUIRE(feed_all(vec(
            Op(OpID::TRUE, s1),
            Op(OpID::AND, s2),
            Op(OpID::FALSE, s3)
        )) == Comp(
            OpID::AND, Comp(OpID::TRUE, s1), Comp(OpID::FALSE, s3), s2
        ));
    }

    SECTION("Assign") {
        // a = b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SET, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::SET, Comp(OpID::ID, "a", s1), Comp(OpID::ID, "b", s3), s2
        ));
    }

    SECTION("BAND") {
        // a & b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BAND, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::BAND,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("BNOT") {
        // ~a
        REQUIRE(feed_all(vec(
            Op(OpID::BNOT, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::BNOT,
            Comp(OpID::ID, "a", s2),
            s1
        ));
    }

    SECTION("BOR") {
        // a | b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BOR, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::BOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("BXOR") {
        // a ^ b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::BXOR, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::BXOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Break") {
        // break
        REQUIRE(feed_all(vec(
            Op(OpID::BREAK, s1)
        )) == Comp(
            OpID::BREAK, s1
        ));
    }

    SECTION("Cached Call") {
        // Vector[Int]
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "Vector", s1),
            Op(OpID::CALL, s2),
            Op(OpID::LIST, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::END, s4)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "Vector", s1),
            Comp(OpID::LIST, Comp(OpID::ID, "Int", s3), s2),
            s2
        ));
    }

    SECTION("Cached CallAttr") {
        // obj.attr[T]
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "obj", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "attr", s3),
            Op(OpID::CALL, s4),
            Op(OpID::LIST, s4),
            Op(OpID::ID, "T", s5),
            Op(OpID::END, s6)
        )) == Comp(
            OpID::CALL,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "obj", s1),
                Comp(OpID::ID, "attr", s3),
                s2
            ),
            Comp(OpID::LIST, Comp(OpID::ID, "T", s5), s4),
            s4
        ));
    }

    SECTION("Call") {
        // fn()
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::NOTHING, s3),
            Op(OpID::END, s3)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(OpID::GROUP, Comp(OpID::NOTHING, s3), s2),
            s2
        ));

        // fn(1, 2)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, "2", s5),
            Op(OpID::END, s6)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                Comp(
                    OpID::SEP,
                    Comp(OpID::PLAIN_INT, "1", s3),
                    Comp(OpID::PLAIN_INT, "2", s5),
                    s4
                ),
                s2
            ),
            s2
        ));

        // fn(1, 2, *args)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s4),
            Op(OpID::PLAIN_INT, "2", s5),
            Op(OpID::SEP, s6),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::END, s9)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                Comp(
                    OpID::SEP,
                    Comp(
                        OpID::SEP,
                        Comp(OpID::PLAIN_INT, "1", s3),
                        Comp(OpID::PLAIN_INT, "2", s5),
                        s4
                    ),
                    Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s8), s7),
                    s6
                ),
                s2
            ),
            s2
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s0),
            Op(OpID::PLAIN_INT, "2", s5),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s10),
            Op(OpID::BIND, s11),
            Op(OpID::STRING, "yes", s12),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw2", s14),
            Op(OpID::BIND, s15),
            Op(OpID::TRUE, s16),
            Op(OpID::END, s17)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                csv(
                    Comp(OpID::PLAIN_INT, "1", s3),
                    Comp(OpID::PLAIN_INT, "2", s5),
                    Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s8), s7),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw1", s10),
                        Comp(OpID::STRING, "yes", s12),
                        s11
                    ),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw2", s14),
                        Comp(OpID::TRUE, s16),
                        s15
                    )
                ),
                s2
            ),
            s2
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true, **kwargs)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s0),
            Op(OpID::PLAIN_INT, "2", s5),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s7),
            Op(OpID::ID, "args", s8),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s10),
            Op(OpID::BIND, s11),
            Op(OpID::STRING, "yes", s12),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw2", s14),
            Op(OpID::BIND, s15),
            Op(OpID::TRUE, s16),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_KWARGS, s18),
            Op(OpID::ID, "kwargs", s19),
            Op(OpID::END, s20)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                csv(
                    Comp(OpID::PLAIN_INT, "1", s3),
                    Comp(OpID::PLAIN_INT, "2", s5),
                    Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s8), s7),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw1", s10),
                        Comp(OpID::STRING, "yes", s12),
                        s11
                    ),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw2", s14),
                        Comp(OpID::TRUE, s16),
                        s15
                    ),
                    Comp(
                        OpID::UNPACK_KWARGS,
                        Comp(OpID::ID, "kwargs", s19),
                        s18
                    )
                ),
                s2
            ),
            s2
        ));

        // fn(1, kw1="yes", 2)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s5),
            Op(OpID::BIND, s6),
            Op(OpID::STRING, "yes", s7),
            Op(OpID::SEP, s0),
            Op(OpID::PLAIN_INT, "2", s9),
            Op(OpID::END, s10)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                csv(
                    Comp(OpID::PLAIN_INT, "1", s3),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw1", s5),
                        Comp(OpID::STRING, "yes", s7),
                        s6
                    ),
                    Comp(OpID::PLAIN_INT, "2", s9)
                ),
                s2
            ),
            s2
        ));

        // fn(1, kw1="yes", kw1=true)
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "fn", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::PLAIN_INT, "1", s3),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s5),
            Op(OpID::BIND, s6),
            Op(OpID::STRING, "yes", s7),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s9),
            Op(OpID::BIND, s10),
            Op(OpID::TRUE, s11),
            Op(OpID::END, s12)
        )) == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(
                OpID::GROUP,
                csv(
                    Comp(OpID::PLAIN_INT, "1", s3),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw1", s5),
                        Comp(OpID::STRING, "yes", s7),
                        s6
                    ),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "kw1", s9),
                        Comp(OpID::TRUE, s11),
                        s10
                    )
                ),
                s2
            ),
            s2
        ));
    }

    SECTION("CallAttr") {
        // thing.do(arg1, arg2, setting=true)
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CALL,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "thing", s1),
                Comp(OpID::ID, "do", s3),
                s2
            ),
            Comp(
                OpID::GROUP,
                Comp(
                    OpID::SEP,
                    Comp(
                        OpID::SEP,
                        Comp(OpID::ID, "arg1", s5),
                        Comp(OpID::ID, "arg2", s7),
                        s6
                    ),
                    Comp(
                        OpID::BIND,
                        Comp(OpID::ID, "setting", s9),
                        Comp(OpID::TRUE, s11),
                        s10
                    ),
                    s8
                ),
                s4
            ),
            s4
        ));

        // a.3()
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, "3", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::NOTHING, s5),
            Op(OpID::END, s5)
        )) == Comp(
            OpID::CALL,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::PLAIN_INT, "3", s3),
                s2
            ),
            Comp(
                OpID::GROUP,
                Comp(OpID::NOTHING, s5),
                s4
            ),
            s4
        ));

        // a.true()
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::TRUE, s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::NOTHING, s5),
            Op(OpID::END, s5)
        )) == Comp(
            OpID::CALL,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::TRUE, s3),
                s2
            ),
            Comp(
                OpID::GROUP,
                Comp(OpID::NOTHING, s5),
                s4
            ),
            s4
        ));
    }

    SECTION("Continue") {
        // continue
        REQUIRE(feed_all(vec(
            Op(OpID::CONTINUE, s1)
        )) == Comp(
            OpID::CONTINUE, s1
        ));
    }

    SECTION("Declare") {
        // a: Int
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3)
        )) == Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "Int", s3),
            s2
        ));

        // 3: Int
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3)
        )) == Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::PLAIN_INT, "3", s1),
            Comp(OpID::ID, "Int", s3),
            s2
        ));

        // true: Int
        REQUIRE(feed_all(vec(
            Op(OpID::TRUE, s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3)
        )) == Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::TRUE, s1),
            Comp(OpID::ID, "Int", s3),
            s2
        ));
    }

    SECTION("Def") {
        // def f(): return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                        s3
                    ),
                    Comp(
                        OpID::RETURN,
                        Comp(OpID::PLAIN_INT, "0", s7),
                        s6
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // def f():
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s9),
                            s8
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // def f() -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s6),
                        s5
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s11),
                            s10
                        )),
                        s9
                    ),
                    s7
                ),
                s1
            )),
            s1
        ));

        // def f(arg) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s12),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP, Comp(OpID::ID, "arg", s4), s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s7),
                        s6
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s12),
                            s11
                        )),
                        s10
                    ),
                    s8
                ),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::END, s9),
            Op(OpID::RETURNS, s10),
            Op(OpID::ID, "Int", s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, "0", s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "arg1", s4),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "arg2", s6),
                                        Comp(OpID::ID, "Int", s8),
                                        s7
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s11),
                        s10
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s16),
                            s15
                        )),
                        s14
                    ),
                    s12
                ),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int, *args) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::END, s12),
            Op(OpID::RETURNS, s13),
            Op(OpID::ID, "Int", s14),
            Op(OpID::LABEL, s15),
            Op(OpID::BLOCK, s17),
            Op(OpID::RETURN, s18),
            Op(OpID::PLAIN_INT, "0", s19),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "arg1", s4),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "arg2", s6),
                                        Comp(OpID::ID, "Int", s8),
                                        s7
                                    ),
                                    Comp(
                                        OpID::UNPACK_ARGS,
                                        Comp(OpID::ID, "args", s11),
                                        s10
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s14),
                        s13
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s19),
                            s18
                        )),
                        s17
                    ),
                    s15
                ),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int, *args, kw1=1, kw2: Bool) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s13),
            Op(OpID::BIND, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw2", s17),
            Op(OpID::TYPE_LABEL, s18),
            Op(OpID::ID, "Bool", s19),
            Op(OpID::END, s20),
            Op(OpID::RETURNS, s21),
            Op(OpID::ID, "Int", s22),
            Op(OpID::LABEL, s23),
            Op(OpID::BLOCK, s25),
            Op(OpID::RETURN, s26),
            Op(OpID::PLAIN_INT, "0", s27),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "arg1", s4),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "arg2", s6),
                                        Comp(OpID::ID, "Int", s8),
                                        s7
                                    ),
                                    Comp(
                                        OpID::UNPACK_ARGS,
                                        Comp(OpID::ID, "args", s11),
                                        s10
                                    ),
                                    Comp(
                                        OpID::BIND,
                                        Comp(OpID::ID, "kw1", s13),
                                        Comp(OpID::PLAIN_INT, "1", s15),
                                        s14
                                    ),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "kw2", s17),
                                        Comp(OpID::ID, "Bool", s19),
                                        s18
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s22),
                        s21
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s27),
                            s26
                        )),
                        s25
                    ),
                    s23
                ),
                s1
            )),
            s1
        ));

        // def f(
        //     arg1, arg2: Int, *args, kw1=1, kw2: Bool, **kwargs
        // ) -> Int:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "arg1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "arg2", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::ID, "Int", s8),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s10),
            Op(OpID::ID, "args", s11),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw1", s13),
            Op(OpID::BIND, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw2", s17),
            Op(OpID::TYPE_LABEL, s18),
            Op(OpID::ID, "Bool", s19),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_KWARGS, s21),
            Op(OpID::ID, "kwargs", s22),
            Op(OpID::END, s23),
            Op(OpID::RETURNS, s24),
            Op(OpID::ID, "Int", s25),
            Op(OpID::LABEL, s26),
            Op(OpID::BLOCK, s28),
            Op(OpID::RETURN, s29),
            Op(OpID::PLAIN_INT, "0", s30),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "arg1", s4),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "arg2", s6),
                                        Comp(OpID::ID, "Int", s8),
                                        s7
                                    ),
                                    Comp(
                                        OpID::UNPACK_ARGS,
                                        Comp(OpID::ID, "args", s11),
                                        s10
                                    ),
                                    Comp(
                                        OpID::BIND,
                                        Comp(OpID::ID, "kw1", s13),
                                        Comp(OpID::PLAIN_INT, "1", s15),
                                        s14
                                    ),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "kw2", s17),
                                        Comp(OpID::ID, "Bool", s19),
                                        s18
                                    ),
                                    Comp(
                                        OpID::UNPACK_KWARGS,
                                        Comp(OpID::ID, "kwargs", s22),
                                        s21
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s25),
                        s24
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s30),
                            s29
                        )),
                        s28
                    ),
                    s26
                ),
                s1
            )),
            s1
        ));

        // def f(a: Int, b: String):
        //     return 0
        // case (c: Float32, true) -> Int64:
        //     return 3s64
        // case (1, *, kw:: 2):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s8),
            Op(OpID::TYPE_LABEL, s9),
            Op(OpID::ID, "String", s10),
            Op(OpID::END, s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, "0", s16),
            Op(OpID::STMT, s17),
            Op(OpID::END, s18),
            Op(OpID::STMT, s18),
            Op(OpID::CASE, s18),
            Op(OpID::GROUP, s19),
            Op(OpID::ID, "c", s20),
            Op(OpID::TYPE_LABEL, s21),
            Op(OpID::ID, "Float32", s22),
            Op(OpID::SEP, s0),
            Op(OpID::TRUE, s24),
            Op(OpID::END, s25),
            Op(OpID::RETURNS, s26),
            Op(OpID::ID, "Int64", s27),
            Op(OpID::LABEL, s28),
            Op(OpID::BLOCK, s30),
            Op(OpID::RETURN, s31),
            Op(OpID::NUMBER, std::int64_t(3), s32),
            Op(OpID::STMT, s33),
            Op(OpID::END, s34),
            Op(OpID::STMT, s34),
            Op(OpID::CASE, s34),
            Op(OpID::GROUP, s35),
            Op(OpID::PLAIN_INT, "1", s36),
            Op(OpID::SEP, s0),
            Op(OpID::POS_KW_SEP, s38),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "kw", s40),
            Op(OpID::MATCHING, s41),
            Op(OpID::PLAIN_INT, "2", s42),
            Op(OpID::END, s43),
            Op(OpID::LABEL, s44),
            Op(OpID::BLOCK, s46),
            Op(OpID::RETURN, s47),
            Op(OpID::PLAIN_INT, "0", s48),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::DEF,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "a", s4),
                                        Comp(OpID::ID, "Int", s6),
                                        s5
                                    ),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "b", s8),
                                        Comp(OpID::ID, "String", s10),
                                        s9
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s16),
                                s15
                            )),
                            s14
                        ),
                        s12
                    ),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::RETURNS,
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "c", s20),
                                        Comp(OpID::ID, "Float32", s22),
                                        s21
                                    ),
                                    Comp(OpID::TRUE, s24)
                                ),
                                s19
                            ),
                            Comp(OpID::ID, "Int64", s27),
                            s26
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::NUMBER, std::int64_t(3), s32),
                                s31
                            )),
                            s30
                        ),
                        s28
                    ),
                    s18
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(OpID::PLAIN_INT, "1", s36),
                                Comp(OpID::POS_KW_SEP, s38),
                                Comp(
                                    OpID::MATCHING,
                                    Comp(OpID::ID, "kw", s40),
                                    Comp(OpID::PLAIN_INT, "2", s42),
                                    s41
                                )
                            ),
                            s35
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(
                                Comp(
                                    OpID::RETURN,
                                    Comp(OpID::PLAIN_INT, "0", s48),
                                    s47
                                )
                            ),
                            s46
                        ),
                        s44
                    ),
                    s34
                )
            ),
            s1
        ));

        // def 3():
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::PLAIN_INT, "3", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, "0", s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::PLAIN_INT, "3", s2),
                        Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s9),
                            s8
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // def f()
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::CALL,
                    Comp(OpID::ID, "f", s2),
                    Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                    s3
                ),
                s1
            )),
            s1
        ));

        // def f:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "f", s2),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s7),
                            s6
                        )),
                        s5
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // def true():
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::TRUE, s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::NOTHING, s4),
            Op(OpID::END, s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::RETURN, s8),
            Op(OpID::PLAIN_INT, "0", s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::TRUE, s2),
                        Comp(OpID::GROUP, Comp(OpID::NOTHING, s4), s3),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s9),
                            s8
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // def f[]:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s9),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(OpID::LIST, Comp(OpID::NOTHING, s4), s3),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s9),
                            s8
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // def f(a=1, b):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, "1", s6),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, "0", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::ID, "a", s4),
                                    Comp(OpID::PLAIN_INT, "1", s6),
                                    s5
                                ),
                                Comp(OpID::ID, "b", s8)
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s14),
                            s13
                        )),
                        s12
                    ),
                    s10
                ),
                s1
            )),
            s1
        ));

        // def f(*, *args):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::POS_KW_SEP, s4),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s6),
            Op(OpID::ID, "args", s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, "0", s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(OpID::POS_KW_SEP, s4),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s7),
                                    s6
                                )
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s13),
                            s12
                        )),
                        s11
                    ),
                    s9
                ),
                s1
            )),
            s1
        ));

        // def f(a=1, *args):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, "1", s6),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s8),
            Op(OpID::ID, "args", s9),
            Op(OpID::END, s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "0", s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::ID, "a", s4),
                                    Comp(OpID::PLAIN_INT, "1", s6),
                                    s5
                                ),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s9),
                                    s8
                                )
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s15),
                            s14
                        )),
                        s13
                    ),
                    s11
                ),
                s1
            )),
            s1
        ));

        // def f(a=1=2):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::BIND, s5),
            Op(OpID::PLAIN_INT, "1", s6),
            Op(OpID::BIND, s7),
            Op(OpID::PLAIN_INT, "2", s8),
            Op(OpID::END, s9),
            Op(OpID::LABEL, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, "0", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::BIND,
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::ID, "a", s4),
                                    Comp(OpID::PLAIN_INT, "1", s6),
                                    s5
                                ),
                                Comp(OpID::PLAIN_INT, "2", s8),
                                s7
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s14),
                            s13
                        )),
                        s12
                    ),
                    s10
                ),
                s1
            )),
            s1
        ));

        // def f(*args=1):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::BIND, s6),
            Op(OpID::PLAIN_INT, "1", s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, "0", s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::BIND,
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s5),
                                    s4
                                ),
                                Comp(OpID::PLAIN_INT, "1", s7),
                                s6
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s13),
                            s12
                        )),
                        s11
                    ),
                    s9
                ),
                s1
            )),
            s1
        ));

        // def f(**kwargs=1):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_KWARGS, s4),
            Op(OpID::ID, "kwargs", s5),
            Op(OpID::BIND, s6),
            Op(OpID::PLAIN_INT, "1", s7),
            Op(OpID::END, s8),
            Op(OpID::LABEL, s9),
            Op(OpID::BLOCK, s11),
            Op(OpID::RETURN, s12),
            Op(OpID::PLAIN_INT, "0", s13),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::BIND,
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::ID, "kwargs", s5),
                                    s4
                                ),
                                Comp(OpID::PLAIN_INT, "1", s7),
                                s6
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s13),
                            s12
                        )),
                        s11
                    ),
                    s9
                ),
                s1
            )),
            s1
        ));

        // def f(*3):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::PLAIN_INT, "3", s5),
            Op(OpID::END, s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::RETURN, s10),
            Op(OpID::PLAIN_INT, "0", s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::UNPACK_ARGS,
                                Comp(OpID::PLAIN_INT, "3", s5),
                                s4
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s11),
                            s10
                        )),
                        s9
                    ),
                    s7
                ),
                s1
            )),
            s1
        ));

        // def f(**3):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::UNPACK_KWARGS, s4),
            Op(OpID::PLAIN_INT, "3", s5),
            Op(OpID::END, s6),
            Op(OpID::LABEL, s7),
            Op(OpID::BLOCK, s9),
            Op(OpID::RETURN, s10),
            Op(OpID::PLAIN_INT, "0", s11),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::UNPACK_KWARGS,
                                Comp(OpID::PLAIN_INT, "3", s5),
                                s4
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s11),
                            s10
                        )),
                        s9
                    ),
                    s7
                ),
                s1
            )),
            s1
        ));

        // def f(3: Int):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::PLAIN_INT, "3", s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::END, s7),
            Op(OpID::LABEL, s8),
            Op(OpID::BLOCK, s10),
            Op(OpID::RETURN, s11),
            Op(OpID::PLAIN_INT, "0", s12),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(OpID::PLAIN_INT, "3", s4),
                                Comp(OpID::ID, "Int", s6),
                                s5
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s12),
                            s11
                        )),
                        s10
                    ),
                    s8
                ),
                s1
            )),
            s1
        ));

        // def f(a: Int, a: Float64):
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::DEF, s1),
            Op(OpID::ID, "f", s2),
            Op(OpID::CALL, s3),
            Op(OpID::GROUP, s3),
            Op(OpID::ID, "a", s4),
            Op(OpID::TYPE_LABEL, s5),
            Op(OpID::ID, "Int", s6),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "a", s8),
            Op(OpID::TYPE_LABEL, s9),
            Op(OpID::ID, "Float64", s10),
            Op(OpID::END, s11),
            Op(OpID::LABEL, s12),
            Op(OpID::BLOCK, s14),
            Op(OpID::RETURN, s15),
            Op(OpID::PLAIN_INT, "0", s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s2),
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(
                                    OpID::TYPE_LABEL,
                                    Comp(OpID::ID, "a", s4),
                                    Comp(OpID::ID, "Int", s6),
                                    s5
                                ),
                                Comp(
                                    OpID::TYPE_LABEL,
                                    Comp(OpID::ID, "a", s8),
                                    Comp(OpID::ID, "Float64", s10),
                                    s9
                                )
                            ),
                            s3
                        ),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s16),
                            s15
                        )),
                        s14
                    ),
                    s12
                ),
                s1
            )),
            s1
        ));

        // def f():
        //     return 0
        // elif true:
        //     return 1
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s9),
            Op(OpID::STMT, s10),
            Op(OpID::END, s11),
            Op(OpID::STMT, s11),
            Op(OpID::ELIF, s11),
            Op(OpID::TRUE, s12),
            Op(OpID::LABEL, s13),
            Op(OpID::BLOCK, s15),
            Op(OpID::RETURN, s16),
            Op(OpID::PLAIN_INT, "1", s17),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::DEF,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s2),
                            Comp(
                                OpID::GROUP,
                                Comp(OpID::NOTHING, s4),
                                s3
                            ),
                            s3
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s9),
                                s8
                            )),
                            s7
                        ),
                        s5
                    ),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::TRUE, s12),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s17),
                                s16
                            )),
                            s15
                        ),
                        s13
                    ),
                    s11
                )
            ),
            s1
        ));
    }

    SECTION("DIV") {
        // a / b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::DIV, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::DIV,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("For") {
        // for x in c: a += x
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::IN,
                        Comp(OpID::ID, "x", s2),
                        Comp(OpID::ID, "c", s4),
                        s3
                    ),
                    Comp(
                        OpID::IADD,
                        Comp(OpID::ID, "a", s6),
                        Comp(OpID::ID, "x", s8),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // for x in c:
        //     a += x
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::IN,
                        Comp(OpID::ID, "x", s2),
                        Comp(OpID::ID, "c", s4),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::IADD,
                            Comp(OpID::ID, "a", s8),
                            Comp(OpID::ID, "x", s10),
                            s9
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // for x, y in c:
        //     a += x + y
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::IN,
                        Comp(
                            OpID::LOOP_VAR_SEP,
                            Comp(OpID::ID, "x", s2),
                            Comp(OpID::ID, "y", s4),
                            s3
                        ),
                        Comp(OpID::ID, "c", s6),
                        s5
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::IADD,
                            Comp(OpID::ID, "a", s10),
                            Comp(
                                OpID::ADD,
                                Comp(OpID::ID, "x", s12),
                                Comp(OpID::ID, "y", s14),
                                s13
                            ),
                            s11
                        )),
                        s9
                    ),
                    s7
                ),
                s1
            )),
            s1
        ));

        // for x in c:
        //     break
        // else:
        //     a += 1
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "1", s16),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::FOR,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::IN,
                            Comp(OpID::ID, "x", s2),
                            Comp(OpID::ID, "c", s4),
                            s3
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(OpID::BREAK, s8)),
                            s7
                        ),
                        s5
                    ),
                    s1
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::IADD,
                                Comp(OpID::ID, "a", s14),
                                Comp(OpID::PLAIN_INT, "1", s16),
                                s15
                            )),
                            s13
                        ),
                        s11
                    ),
                    s10
                )
            ),
            s1
        ));

        // for x in c
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::IN,
                    Comp(OpID::ID, "x", s2),
                    Comp(OpID::ID, "c", s4),
                    s3
                ),
                s1
            )),
            s1
        ));

        // for x:
        //     a += x
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "x", s2),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::IADD,
                            Comp(OpID::ID, "a", s6),
                            Comp(OpID::ID, "x", s8),
                            s7
                        )),
                        s5
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // for 3 in c:
        //     a += 1
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::FOR, s1),
            Op(OpID::PLAIN_INT, "3", s2),
            Op(OpID::IN, s3),
            Op(OpID::ID, "c", s4),
            Op(OpID::LABEL, s5),
            Op(OpID::BLOCK, s7),
            Op(OpID::ID, "a", s8),
            Op(OpID::IADD, s9),
            Op(OpID::PLAIN_INT, "1", s10),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::IN,
                        Comp(OpID::PLAIN_INT, "3", s2),
                        Comp(OpID::ID, "c", s4),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::IADD,
                            Comp(OpID::ID, "a", s8),
                            Comp(OpID::PLAIN_INT, "1", s10),
                            s9
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        ));

        // for x in c:
        //     a += x
        // elif true:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s18),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::FOR,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::IN,
                            Comp(OpID::ID, "x", s2),
                            Comp(OpID::ID, "c", s4),
                            s3
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::IADD,
                                Comp(OpID::ID, "a", s8),
                                Comp(OpID::ID, "x", s10),
                                s9
                            )),
                            s7
                        ),
                        s5
                    ),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::TRUE, s13),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s18),
                                s17
                            )),
                            s16
                        ),
                        s14
                    ),
                    s12
                )
            ),
            s1
        ));
    }

    SECTION("GetAttr") {
        // a.b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));

        // a.3
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, "3", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::PLAIN_INT, "3", s3),
            s2
        ));

        // a.true
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::TRUE, s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::TRUE, s3),
            s2
        ));
    }

    SECTION("IADD") {
        // a += b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IADD, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IADD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IBAND") {
        // a &= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBAND, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IBAND,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IBOR") {
        // a |= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBOR, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IBOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IBXOR") {
        // a ^= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IBXOR, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IBXOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("ID") {
        // a
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1)
        )) == Comp(
            OpID::ID, "a", s1
        ));
    }

    SECTION("IDIV") {
        // a /= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IDIV, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IDIV,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("ILSH") {
        // a <<= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ILSH, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::ILSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IMOD") {
        // a %= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IMOD, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IMOD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IMUL") {
        // a *= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IMUL, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IMUL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IPOW") {
        // a **= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IPOW, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IPOW,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("IRSH") {
        // a >>= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::IRSH, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::IRSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("ISUB") {
        // a -= b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::ISUB, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::ISUB,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("If") {
        // if x: return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::RETURN, s4),
            Op(OpID::PLAIN_INT, "0", s5),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::IF,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "x", s2),
                    Comp(
                        OpID::RETURN,
                        Comp(OpID::PLAIN_INT, "0", s5),
                        s4
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // if x:
        //     return 0
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::IF,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "x", s2),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s7),
                            s6
                        )),
                        s5
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s15),
                                s14
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                )
            ),
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        // else:
        //     return 2
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::STMT, s16),
            Op(OpID::END, s17),
            Op(OpID::STMT, s17),
            Op(OpID::ELSE, s17),
            Op(OpID::BODY, s18),
            Op(OpID::BLOCK, s20),
            Op(OpID::RETURN, s21),
            Op(OpID::PLAIN_INT, "2", s22),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s15),
                                s14
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "2", s22),
                                s21
                            )),
                            s20
                        ),
                        s18
                    ),
                    s17
                )
            ),
            s1
        ));

        // if x:
        //     return 0
        // else:
        //     return 1
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELSE, s9),
            Op(OpID::BODY, s10),
            Op(OpID::BLOCK, s12),
            Op(OpID::RETURN, s13),
            Op(OpID::PLAIN_INT, "1", s14),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s9
                )
            ),
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        // elif z:
        //     return 2
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::STMT, s16),
            Op(OpID::END, s17),
            Op(OpID::STMT, s17),
            Op(OpID::ELIF, s17),
            Op(OpID::ID, "z", s18),
            Op(OpID::LABEL, s19),
            Op(OpID::BLOCK, s21),
            Op(OpID::RETURN, s22),
            Op(OpID::PLAIN_INT, "2", s23),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s15),
                                s14
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "z", s18),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "2", s23),
                                s22
                            )),
                            s21
                        ),
                        s19
                    ),
                    s17
                )
            ),
            s1
        ));

        // if x
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::IF,
                Comp(OpID::ID, "x", s2),
                s1
            )),
            s1
        ));

        // if x:
        //     return 0
        // elif y
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELIF, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(OpID::ELIF, Comp(OpID::ID, "y", s10), s9)
            ),
            s1
        ));

        // if x:
        //     return 0
        // else y:
        //     return 1
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::ELSE, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s15),
                                s14
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                )
            ),
            s1
        ));

        // if x:
        //     return 0
        // case y:
        //     return 1
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::IF, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::LABEL, s3),
            Op(OpID::BLOCK, s5),
            Op(OpID::RETURN, s6),
            Op(OpID::PLAIN_INT, "0", s7),
            Op(OpID::STMT, s8),
            Op(OpID::END, s9),
            Op(OpID::STMT, s9),
            Op(OpID::CASE, s9),
            Op(OpID::ID, "y", s10),
            Op(OpID::LABEL, s11),
            Op(OpID::BLOCK, s13),
            Op(OpID::RETURN, s14),
            Op(OpID::PLAIN_INT, "1", s15),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s7),
                                s6
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", s15),
                                s14
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                )
            ),
            s1
        ));
    }

    SECTION("Init") {
        // a: Int = 3
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::SET, s4),
            Op(OpID::PLAIN_INT, "3", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        ));

        // 3: Int = 3
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "3", s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::SET, s4),
            Op(OpID::PLAIN_INT, "3", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::PLAIN_INT, "3", s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        ));

        // true: Int = 3
        REQUIRE(feed_all(vec(
            Op(OpID::TRUE, s1),
            Op(OpID::TYPE_LABEL, s2),
            Op(OpID::ID, "Int", s3),
            Op(OpID::SET, s4),
            Op(OpID::PLAIN_INT, "3", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::TRUE, s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        ));
    }

    SECTION("LSH") {
        // a << b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::LSH, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::LSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Lambda Expression") {
        // %(%1 + %2)
        REQUIRE(feed_all(vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::GROUP, s2),
            Op(OpID::LAMBDA, s3),
            Op(OpID::PLAIN_INT, "1", s4),
            Op(OpID::ADD, s5),
            Op(OpID::LAMBDA, s6),
            Op(OpID::PLAIN_INT, "2", s7),
            Op(OpID::END, s8)
        )) == Comp(
            OpID::LAMBDA,
            Comp(
                OpID::GROUP,
                Comp(
                    OpID::ADD,
                    Comp(
                        OpID::LAMBDA,
                        Comp(OpID::PLAIN_INT, "1", s4),
                        s3
                    ),
                    Comp(
                        OpID::LAMBDA,
                        Comp(OpID::PLAIN_INT, "2", s7),
                        s6
                    ),
                    s5
                ),
                s2
            ),
            s1
        ));

        // %[%1 + %2]
        REQUIRE(feed_all(vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::LIST, s2),
            Op(OpID::LAMBDA, s3),
            Op(OpID::PLAIN_INT, "1", s4),
            Op(OpID::ADD, s5),
            Op(OpID::LAMBDA, s6),
            Op(OpID::PLAIN_INT, "2", s7),
            Op(OpID::END, s8)
        )) == Comp(
            OpID::LAMBDA,
            Comp(
                OpID::LIST,
                Comp(
                    OpID::ADD,
                    Comp(
                        OpID::LAMBDA,
                        Comp(OpID::PLAIN_INT, "1", s4),
                        s3
                    ),
                    Comp(
                        OpID::LAMBDA,
                        Comp(OpID::PLAIN_INT, "2", s7),
                        s6
                    ),
                    s5
                ),
                s2
            ),
            s1
        ));
    }

    SECTION("Lambda Var") {
        // %1
        REQUIRE(feed_all(vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::PLAIN_INT, "1", s2)
        )) == Comp(
            OpID::LAMBDA, Comp(OpID::PLAIN_INT, "1", s2), s1
        ));

        // %a
        REQUIRE(feed_all(vec(
            Op(OpID::LAMBDA, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::LAMBDA, Comp(OpID::ID, "a", s2), s1
        ));
    }

    SECTION("List") {
        // []
        REQUIRE(feed_all(vec(
            Op(OpID::LIST, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        )) == Comp(
            OpID::LIST, Comp(OpID::NOTHING, s2), s1
        ));

        // [a]
        REQUIRE(feed_all(vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::END, s3)
        )) == Comp(
            OpID::LIST, Comp(OpID::ID, "a", s2), s1
        ));

        // [a, b, c]
        REQUIRE(feed_all(vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        )) == Comp(
            OpID::LIST,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        ));

        // [a, *args, b]
        REQUIRE(feed_all(vec(
            Op(OpID::LIST, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        )) == Comp(
            OpID::LIST,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        ));
    }

    SECTION("Literals") {
        // true
        REQUIRE(feed_all(vec(
            Op(OpID::TRUE, s1)
        )) == Comp(
            OpID::TRUE, s1
        ));

        // false
        REQUIRE(feed_all(vec(
            Op(OpID::FALSE, s1)
        )) == Comp(
            OpID::FALSE, s1
        ));

        // 0
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "0", s1)
        )) == Comp(
            OpID::PLAIN_INT, "0", s1
        ));

        // 0s8
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::int8_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::int8_t(0), s1
        ));

        // 0s16
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::int16_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::int16_t(0), s1
        ));

        // 0s32
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::int32_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::int32_t(0), s1
        ));

        // 0s64
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::int64_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::int64_t(0), s1
        ));

        // 0u8
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::uint8_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::uint8_t(0), s1
        ));

        // 0u16
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::uint16_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::uint16_t(0), s1
        ));

        // 0u32
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::uint32_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::uint32_t(0), s1
        ));

        // 0u64
        REQUIRE(feed_all(vec(
            Op(OpID::NUMBER, std::uint64_t(0), s1)
        )) == Comp(
            OpID::NUMBER, std::uint64_t(0), s1
        ));

        // 0f32
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "0f32", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "0f32", s1
        ));

        // 0f64
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "0f64", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "0f64", s1
        ));

        // 1e7
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "1e7", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "1e7", s1
        ));

        // 1e-7
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "1e-7", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "1e-7", s1
        ));

        // 12.34
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "12", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, "34", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::PLAIN_INT, "34", s3),
            s2
        ));

        // 12.34f32
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "12", s1),
            Op(OpID::GET, s2),
            Op(OpID::FLOAT_TAIL, "34f32", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34f32", s3),
            s2
        ));

        // 12.34e5
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "12", s1),
            Op(OpID::GET, s2),
            Op(OpID::FLOAT_TAIL, "34e5", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34e5", s3),
            s2
        ));

        // 12.34e5f32
        REQUIRE(feed_all(vec(
            Op(OpID::PLAIN_INT, "12", s1),
            Op(OpID::GET, s2),
            Op(OpID::FLOAT_TAIL, "34e5f32", s3)
        )) == Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34e5f32", s3),
            s2
        ));

        // 1e46f32
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "1e46f32", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "1e46f32", s1
        ));

        // 1e309
        REQUIRE(feed_all(vec(
            Op(OpID::FLOAT_TAIL, "1e309", s1)
        )) == Comp(
            OpID::FLOAT_TAIL, "1e309", s1
        ));

        // '\0'
        REQUIRE(feed_all(vec(
            Op(OpID::CHAR, std::int32_t(0), s1)
        )) == Comp(
            OpID::CHAR, std::int32_t(0), s1
        ));

        // "asdf"
        REQUIRE(feed_all(vec(
            Op(OpID::STRING, "asdf", s1)
        )) == Comp(
            OpID::STRING, "asdf", s1
        ));
    }

    SECTION("MOD") {
        // a % b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::MOD, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::MOD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("MUL") {
        // a * b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::MUL, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::MUL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Map") {
        // {}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        )) == Comp(
            OpID::ENCLOSURE, Comp(OpID::NOTHING, s2), s1
        ));

        // {a: 1}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, "1", s4),
            Op(OpID::END, s5)
        )) == Comp(
            OpID::ENCLOSURE,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::ID, "a", s2),
                Comp(OpID::PLAIN_INT, "1", s4),
                s3
            ),
            s1
        ));

        // {a: 1, b: 2, c: 3}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, "1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s6),
            Op(OpID::TYPE_LABEL, s7),
            Op(OpID::PLAIN_INT, "2", s8),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "c", s10),
            Op(OpID::TYPE_LABEL, s11),
            Op(OpID::PLAIN_INT, "3", s12),
            Op(OpID::END, s13)
        )) == Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(OpID::ID, "a", s2),
                    Comp(OpID::PLAIN_INT, "1", s4),
                    s3
                ),
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(OpID::ID, "b", s6),
                    Comp(OpID::PLAIN_INT, "2", s8),
                    s7
                ),
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(OpID::ID, "c", s10),
                    Comp(OpID::PLAIN_INT, "3", s12),
                    s11
                )
            ),
            s1
        ));

        // {a: 1, **kwargs, b: 2}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::TYPE_LABEL, s3),
            Op(OpID::PLAIN_INT, "1", s4),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_KWARGS, s6),
            Op(OpID::ID, "kwargs", s7),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s9),
            Op(OpID::TYPE_LABEL, s10),
            Op(OpID::PLAIN_INT, "2", s11),
            Op(OpID::END, s12)
        )) == Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(OpID::ID, "a", s2),
                    Comp(OpID::PLAIN_INT, "1", s4),
                    s3
                ),
                Comp(
                    OpID::UNPACK_KWARGS,
                    Comp(OpID::ID, "kwargs", s7),
                    s6
                ),
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(OpID::ID, "b", s9),
                    Comp(OpID::PLAIN_INT, "2", s11),
                    s10
                )
            ),
            s1
        ));
    }

    SECTION("Match") {
        // match x
        // case Int{%y}: return y
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "Int", s5),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                                s6
                            ),
                            s6
                        ),
                        Comp(OpID::RETURN, Comp(OpID::ID, "y", s12), s11),
                        s10
                    ),
                    s4
                )
            ),
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "Int", s5),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN, Comp(OpID::ID, "y", s14), s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        // else:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s21),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "Int", s5),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN, Comp(OpID::ID, "y", s14), s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s21),
                                s20
                            )),
                            s19
                        ),
                        s17
                    ),
                    s16
                )
            ),
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        // case false:
        //     return 0
        REQUIRE(feed_all(vec(
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
            Op(OpID::PLAIN_INT, "0", s22),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "Int", s5),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN, Comp(OpID::ID, "y", s14), s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::FALSE, s17),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s22),
                                s21
                            )),
                            s20
                        ),
                        s18
                    ),
                    s16
                )
            ),
            s1
        ));

        // match x
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::MATCH, s1),
            Op(OpID::ID, "x", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::MATCH, Comp(OpID::ID, "x", s2), s1
            )),
            s1
        ));

        // match x
        // case Int{%y}
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "Int", s5),
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                            s6
                        ),
                        s6
                    ),
                    s4
                )
            ),
            s1
        ));

        // match x
        // elif Int{%y}:
        //     return y
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(OpID::MATCH, Comp(OpID::ID, "x", s2), s1),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "Int", s5),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(OpID::LAMBDA, Comp(OpID::ID, "y", s8), s7),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN, Comp(OpID::ID, "y", s14), s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        ));
    }

    SECTION("NEG") {
        // -a
        REQUIRE(feed_all(vec(
            Op(OpID::NEG, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::NEG,
            Comp(OpID::ID, "a", s2),
            s1
        ));
    }

    SECTION("Or") {
        // a or b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::OR, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::OR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("POW") {
        // a ** b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::POW, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::POW,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("RSH") {
        // a >> b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::RSH, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::RSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Raise") {
        // raise
        REQUIRE(feed_all(vec(
            Op(OpID::RAISE, s1),
            Op(OpID::NOTHING, s_end)
        )) == Comp(
            OpID::RAISE, Comp(OpID::NOTHING, s_end), s1
        ));

        // raise a
        REQUIRE(feed_all(vec(
            Op(OpID::RAISE, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::RAISE, Comp(OpID::ID, "a", s2), s1
        ));
    }

    SECTION("Return") {
        // return
        REQUIRE(feed_all(vec(
            Op(OpID::RETURN, s1),
            Op(OpID::NOTHING, s_end)
        )) == Comp(
            OpID::RETURN, Comp(OpID::NOTHING, s_end), s1
        ));

        // return a
        REQUIRE(feed_all(vec(
            Op(OpID::RETURN, s1),
            Op(OpID::ID, "a", s2)
        )) == Comp(
            OpID::RETURN, Comp(OpID::ID, "a", s2), s1
        ));
    }

    SECTION("SUB") {
        // a - b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SUB, s2),
            Op(OpID::ID, "b", s3)
        )) == Comp(
            OpID::SUB,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        ));
    }

    SECTION("Set") {
        // {a}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::END, s3)
        )) == Comp(
            OpID::ENCLOSURE,
            Comp(OpID::ID, "a", s2),
            s1
        ));

        // {a, b, c}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        )) == Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        ));

        // {a, *args, b}
        REQUIRE(feed_all(vec(
            Op(OpID::ENCLOSURE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        )) == Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        ));
    }

    SECTION("SetAttr") {
        // a.b = c
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3),
            Op(OpID::SET, s4),
            Op(OpID::ID, "c", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::ID, "b", s3),
                s2
            ),
            Comp(OpID::ID, "c", s5),
            s4
        ));

        // a.3 = b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, "3", s3),
            Op(OpID::SET, s4),
            Op(OpID::ID, "b", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::PLAIN_INT, "3", s3),
                s2
            ),
            Comp(OpID::ID, "b", s5),
            s4
        ));

        // a.true = b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::TRUE, s3),
            Op(OpID::SET, s4),
            Op(OpID::ID, "b", s5)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::TRUE, s3),
                s2
            ),
            Comp(OpID::ID, "b", s5),
            s4
        ));
    }

    SECTION("Symbol") {
        // :x
        REQUIRE(feed_all(vec(
            Op(OpID::SYMBOL, s1),
            Op(OpID::ID, "x", s2)
        )) == Comp(
            OpID::SYMBOL, Comp(OpID::ID, "x", s2), s1
        ));

        // :3
        REQUIRE(feed_all(vec(
            Op(OpID::SYMBOL, s1),
            Op(OpID::PLAIN_INT, "3", s2)
        )) == Comp(
            OpID::SYMBOL,
            Comp(OpID::PLAIN_INT, "3", s2),
            s1
        ));
    }

    SECTION("Ternary") {
        // x if y else z
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_IF, s2),
            Op(OpID::ID, "y", s3),
            Op(OpID::TERNARY_ELSE, s4),
            Op(OpID::ID, "z", s5)
        )) == Comp(
            OpID::TERNARY_ELSE,
            Comp(
                OpID::TERNARY_IF,
                Comp(OpID::ID, "x", s1),
                Comp(OpID::ID, "y", s3),
                s2
            ),
            Comp(OpID::ID, "z", s5),
            s4
        ));

        // x if y
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_IF, s2),
            Op(OpID::ID, "y", s3)
        )) == Comp(
            OpID::TERNARY_IF,
            Comp(OpID::ID, "x", s1),
            Comp(OpID::ID, "y", s3),
            s2
        ));

        // x else y
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "x", s1),
            Op(OpID::TERNARY_ELSE, s2),
            Op(OpID::ID, "y", s3)
        )) == Comp(
            OpID::TERNARY_ELSE,
            Comp(OpID::ID, "x", s1),
            Comp(OpID::ID, "y", s3),
            s2
        ));
    }

    SECTION("This") {
        // this
        REQUIRE(feed_all(vec(
            Op(OpID::THIS, s1)
        )) == Comp(
            OpID::THIS, s1
        ));
    }

    SECTION("Try") {
        // try: f()
        // except E{%e}: g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s3),
                            Comp(OpID::GROUP, Comp(OpID::NOTHING, s5), s4),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E", s8),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA,
                                    Comp(OpID::ID, "e", s11),
                                    s10
                                ),
                                s9
                            ),
                            s9
                        ),
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "g", s14),
                            Comp(OpID::GROUP, Comp(OpID::NOTHING, s16), s15),
                            s15
                        ),
                        s13
                    ),
                    s7
                )
            ),
            s1
        ));

        // try:
        //     f()
        // except E{%e}:
        //     g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E", s10),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA, Comp(OpID::ID, "e", s13), s12
                                ),
                                s11
                            ),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s18),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s20), s19
                                ),
                                s19
                            )),
                            s17
                        ),
                        s15
                    ),
                    s9
                )
            ),
            s1
        ));

        // try:
        //     f()
        // except E{%e}:
        //     g()
        // finally:
        //     h()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E", s10),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA, Comp(OpID::ID, "e", s13), s12
                                ),
                                s11
                            ),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s18),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s20), s19
                                ),
                                s19
                            )),
                            s17
                        ),
                        s15
                    ),
                    s9
                ),
                Comp(
                    OpID::FINALLY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "h", s26),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s28), s27                                ),
                                s27
                            )),
                            s25
                        ),
                        s23
                    ),
                    s22
                )
            ),
            s1
        ));

        // try:
        //     f()
        // finally:
        //     g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::FINALLY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s13),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s15), s14
                                ),
                                s14
                            )),
                            s12
                        ),
                        s10
                    ),
                    s9
                )
            ),
            s1
        ));

        // try:
        //     f()
        // except E1{%e}:
        //     g()
        // except E2{%e}:
        //     h()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E1", s10),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA, Comp(OpID::ID, "e", s13), s12
                                ),
                                s11
                            ),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s18),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s20), s19
                                ),
                                s19
                            )),
                            s17
                        ),
                        s15
                    ),
                    s9
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E2", s23),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA, Comp(OpID::ID, "e", s26), s25
                                ),
                                s24
                            ),
                            s24
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "h", s31),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s33), s32
                                ),
                                s32
                            )),
                            s30
                        ),
                        s28
                    ),
                    s22
                )
            ),
            s1
        ));

        // try:
        //     f()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::TRY,
                Comp(
                    OpID::BODY,
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s5),
                            Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                            s6
                        )),
                        s4
                    ),
                    s2
                ),
                s1
            )),
            s1
        ));

        // try a:
        //     f()
        // except E{%e}:
        //     g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "a", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s6),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s8), s7),
                                s7
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "E", s11),
                            Comp(
                                OpID::ENCLOSURE,
                                Comp(
                                    OpID::LAMBDA, Comp(OpID::ID, "e", s14), s13
                                ),
                                s12
                            ),
                            s12
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s19),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s21), s20
                                ),
                                s20
                            )),
                            s18
                        ),
                        s16
                    ),
                    s10
                )
            ),
            s1
        ));

        // try:
        //     f()
        // finally a:
        //     g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::FINALLY,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "a", s10),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s14),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s16), s15
                                ),
                                s15
                            )),
                            s13
                        ),
                        s11
                    ),
                    s9
                )
            ),
            s1
        ));

        // try:
        //     f()
        // except E{%e}
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::TRY,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s5),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s7), s6),
                                s6
                            )),
                            s4
                        ),
                        s2
                    ),
                    s1
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "E", s10),
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::LAMBDA, Comp(OpID::ID, "e", s13), s12
                            ),
                            s11
                        ),
                        s11
                    ),
                    s9
                )
            ),
            s1
        ));
    }

    SECTION("Tuple") {
        // ()
        REQUIRE(feed_all(vec(
            Op(OpID::GROUP, s1),
            Op(OpID::NOTHING, s2),
            Op(OpID::END, s2)
        )) == Comp(
            OpID::GROUP, Comp(OpID::NOTHING, s2), s1
        ));

        // (a, b, c)
        REQUIRE(feed_all(vec(
            Op(OpID::GROUP, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "c", s6),
            Op(OpID::END, s7)
        )) == Comp(
            OpID::GROUP,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        ));

        // a, b, c
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s3),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "c", s5)
        )) == csv(
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            Comp(OpID::ID, "c", s5)
        ));

        // (a, *args, b)
        REQUIRE(feed_all(vec(
            Op(OpID::GROUP, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s4),
            Op(OpID::ID, "args", s5),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s7),
            Op(OpID::END, s8)
        )) == Comp(
            OpID::GROUP,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        ));

        // a, *args, b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::SEP, s0),
            Op(OpID::UNPACK_ARGS, s3),
            Op(OpID::ID, "args", s4),
            Op(OpID::SEP, s0),
            Op(OpID::ID, "b", s6)
        )) == csv(
            Comp(OpID::ID, "a", s1),
            Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s4), s3),
            Comp(OpID::ID, "b", s6)
        ));
    }

    SECTION("Update") {
        // a(i) = b
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::CALL, s2),
            Op(OpID::GROUP, s2),
            Op(OpID::ID, "i", s3),
            Op(OpID::END, s4),
            Op(OpID::SET, s5),
            Op(OpID::ID, "b", s6)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::CALL,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::GROUP, Comp(OpID::ID, "i", s3), s2),
                s2
            ),
            Comp(OpID::ID, "b", s6),
            s5
        ));
    }

    SECTION("UpdateAttr") {
        // a.b(i) = c
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::ID, "b", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "i", s5),
            Op(OpID::END, s6),
            Op(OpID::SET, s7),
            Op(OpID::ID, "c", s8)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::CALL,
                Comp(
                    OpID::GET,
                    Comp(OpID::ID, "a", s1),
                    Comp(OpID::ID, "b", s3),
                    s2
                ),
                Comp(
                    OpID::GROUP,
                    Comp(OpID::ID, "i", s5),
                    s4
                ),
                s4
            ),
            Comp(OpID::ID, "c", s8),
            s7
        ));

        // a.3(i) = c
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::PLAIN_INT, "3", s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "i", s5),
            Op(OpID::END, s6),
            Op(OpID::SET, s7),
            Op(OpID::ID, "c", s8)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::CALL,
                Comp(
                    OpID::GET,
                    Comp(OpID::ID, "a", s1),
                    Comp(OpID::PLAIN_INT, "3", s3),
                    s2
                ),
                Comp(
                    OpID::GROUP,
                    Comp(OpID::ID, "i", s5),
                    s4
                ),
                s4
            ),
            Comp(OpID::ID, "c", s8),
            s7
        ));

        // a.true(i) = c
        REQUIRE(feed_all(vec(
            Op(OpID::ID, "a", s1),
            Op(OpID::GET, s2),
            Op(OpID::TRUE, s3),
            Op(OpID::CALL, s4),
            Op(OpID::GROUP, s4),
            Op(OpID::ID, "i", s5),
            Op(OpID::END, s6),
            Op(OpID::SET, s7),
            Op(OpID::ID, "c", s8)
        )) == Comp(
            OpID::SET,
            Comp(
                OpID::CALL,
                Comp(
                    OpID::GET,
                    Comp(OpID::ID, "a", s1),
                    Comp(OpID::TRUE, s3),
                    s2
                ),
                Comp(
                    OpID::GROUP,
                    Comp(OpID::ID, "i", s5),
                    s4
                ),
                s4
            ),
            Comp(OpID::ID, "c", s8),
            s7
        ));
    }

    SECTION("While") {
        // while a: f()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::WHILE,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "a", s2),
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", s4),
                        Comp(OpID::GROUP, Comp(OpID::NOTHING, s6), s5),
                        s5
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // while a:
        //     f()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::WHILE,
                Comp(
                    OpID::LABEL,
                    Comp(OpID::ID, "a", s2),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", s6),
                            Comp(OpID::GROUP, Comp(OpID::NOTHING, s8), s7),
                            s7
                        )),
                        s5
                    ),
                    s3
                ),
                s1
            )),
            s1
        ));

        // while a:
        //     f()
        // else:
        //     g()
        REQUIRE(feed_all(vec(
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
        )) == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::WHILE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "a", s2),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s6),
                                Comp(OpID::GROUP, Comp(OpID::NOTHING, s8), s7),
                                s7
                            )),
                            s5
                        ),
                        s3
                    ),
                    s1
                ),
                Comp(
                    OpID::ELSE,
                    Comp(
                        OpID::BODY,
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s14),
                                Comp(
                                    OpID::GROUP, Comp(OpID::NOTHING, s16), s15
                                ),
                                s15
                            )),
                            s13
                        ),
                        s11
                    ),
                    s10
                )
            ),
            s1
        ));

        // while a
        REQUIRE(feed_all(vec(
            Op(OpID::CONSTRUCT, s1),
            Op(OpID::WHILE, s1),
            Op(OpID::ID, "a", s2),
            Op(OpID::STMT, s_end),
            Op(OpID::END, s_end)
        )) == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::WHILE,
                Comp(OpID::ID, "a", s2),
                s1
            )),
            s1
        ));
    }    
}
