#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/interpret/interpreterimpl.hpp"
#include "dl/lex/token.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/res.hpp"

#include "capturingcontroller.hpp"
#include "streamfile.hpp"
#include "util.hpp"

using namespace dl;

CapturingController capture(const char* str) {
    auto f = StreamFile();
    auto cc = CapturingController(&f);
    cc.capture(str);
    return cc;
}

TEST_CASE("interpret", "[interpret]") {
    auto interpreter = InterpreterImpl();

    SECTION("ADD") {
        auto cc = capture("a + b");
        REQUIRE(cc.tokens() == vec(
            std::pair<Token, Span>(Token(TokenID::ID, "a"), Span(1, 1)),
            std::pair<Token, Span>(
                Token(TokenID::SPACE, std::uint32_t(1)), Span(1, 2)
            ),
            std::pair<Token, Span>(Token(TokenID::PLUS), Span(1, 3)),
            std::pair<Token, Span>(
                Token(TokenID::SPACE, std::uint32_t(1)), Span(1, 4)
            ),
            std::pair<Token, Span>(Token(TokenID::ID, "b"), Span(1, 5)),
            std::pair<Token, Span>(Token(TokenID::END_OF_FILE), Span(1, 6))
        ));

        REQUIRE(cc.ops() == vec(
            Op(OpID::ID, "a", Span(1, 1)),
            Op(OpID::ADD, Span(1, 3)),
            Op(OpID::ID, "b", Span(1, 5)),
            Op(OpID::STMT, Span(1, 6))
        ));

        REQUIRE(cc.comp() == Comp(
            OpID::ADD,
            Comp(OpID::ID, "a", Span(1, 1)),
            Comp(OpID::ID, "b", Span(1, 5)),
            Span(1, 3)
        ));

        REQUIRE(*cc.node() == Add(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("Addr") {
        REQUIRE(*capture(
            "@a"
        ).node() == Addr(
            NodePtr(new ID("a", Span(1, 2))),
            Span(1, 1)
        ));
    }

    SECTION("AddrType") {
        REQUIRE(*capture(
            "Int@"
        ).node() == AddrType(
            NodePtr(new ID("Int", Span(1, 1, 3))),
            Span(1, 4)
        ));
    }

    SECTION("And") {
        REQUIRE(*capture(
            "true and false"
        ).node() == And(
            NodePtr(new Bool(true, Span(1, 1, 4))),
            NodePtr(new Bool(false, Span(1, 10, 5))),
            Span(1, 6, 3)
        ));
    }

    SECTION("Assign") {
        REQUIRE(*capture(
            "a = b"
        ).node() == Assign(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "a, = 1,"
        ).node() == Assign(
            NodePtr(new Tuple(
                vec(NodePtr(new ID("a", Span(1, 1)))),
                Span(1, 1, 2)
            )),
            NodePtr(new Tuple(
                vec(NodePtr(new Int32(1, Span(1, 6)))),
                Span(1, 6, 2)
            )),
            Span(1, 4)
        ));
    }

    SECTION("BAND") {
        REQUIRE(*capture(
            "a & b"
        ).node() == BitAnd(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("BNOT") {
        REQUIRE(*capture(
            "~a"
        ).node() == BitNot(
            NodePtr(new ID("a", Span(1, 2))),
            Span(1, 1)
        ));
    }

    SECTION("BOR") {
        REQUIRE(*capture(
            "a | b"
        ).node() == BitOr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("BXOR") {
        REQUIRE(*capture(
            "a ^ b"
        ).node() == BitXor(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("Break") {
        REQUIRE(*capture(
            "break"
        ).node() == Break(
            Span(1, 1, 5)
        ));
    }

    SECTION("Cached Call") {
        REQUIRE(*capture(
            "Vector[Int]"
        ).node() == CachedCall(
            NodePtr(new ID("Vector", Span(1, 1, 6))),
            NodePtr(new Args(
                vec(NodePtr(new ID("Int", Span(1, 8, 3)))),
                {},
                Span(1, 7, 5)
            )),
            Span(1, 7)
        ));
    }

    SECTION("Call") {
        auto cc = capture("fn()");
        REQUIRE(cc.ops() == vec(
            Op(OpID::ID, "fn", Span(1, 1, 2)),
            Op(OpID::CALL, Span(1, 3)),
            Op(OpID::GROUP, Span(1, 3)),
            Op(OpID::NOTHING, Span(1, 4)),
            Op(OpID::END, Span(1, 4)),
            Op(OpID::STMT, Span(1, 5))
        ));

        REQUIRE(cc.comp() == Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", Span(1, 1, 2)),
            Comp(
                OpID::GROUP,
                Comp(OpID::NOTHING, Span(1, 4)),
                Span(1, 3, 2)
            ),
            Span(1, 3)
        ));

        REQUIRE(*cc.node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args({}, {}, Span(1, 3, 2))),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1, 2)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(
                    NodePtr(new Int32(1, Span(1, 4))),
                    NodePtr(new Int32(2, Span(1, 7)))
                ),
                {},
                Span(1, 3, 6)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1, 2, *args)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(
                    NodePtr(new Int32(1, Span(1, 4))),
                    NodePtr(new Int32(2, Span(1, 7))),
                    NodePtr(new Expansion(
                        NodePtr(new ID("args", Span(1, 11, 4))),
                        Span(1, 10)
                    ))
                ),
                {},
                Span(1, 3, 13)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1, 2, *args, kw1=\"yes\", kw2=true)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(
                    NodePtr(new Int32(1, Span(1, 4))),
                    NodePtr(new Int32(2, Span(1, 7))),
                    NodePtr(new Expansion(
                        NodePtr(new ID("args", Span(1, 11, 4))),
                        Span(1, 10)
                    ))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        NodePtr(new ID("kw1", Span(1, 17, 3))),
                        NodePtr(new String("yes", Span(1, 21, 5))),
                        Span(1, 20)
                    )),
                    NodePtr(new KeywordArg{
                        NodePtr(new ID("kw2", Span(1, 28, 3))),
                        NodePtr(new Bool(true, Span(1, 32, 4))),
                        Span(1, 31)
                    })
                ),
                Span(1, 3, 1, 36)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1, 2, *args, kw1=\"yes\", kw2=true, **kwargs)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(
                    NodePtr(new Int32(1, Span(1, 4))),
                    NodePtr(new Int32(2, Span(1, 7))),
                    NodePtr(new Expansion(
                        NodePtr(new ID("args", Span(1, 11, 4))),
                        Span(1, 10)
                    ))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        NodePtr(new ID("kw1", Span(1, 17, 3))),
                        NodePtr(new String("yes", Span(1, 21, 5))),
                        Span(1, 20)
                    )),
                    NodePtr(new KeywordArg{
                        NodePtr(new ID("kw2", Span(1, 28, 3))),
                        NodePtr(new Bool(true, Span(1, 32, 4))),
                        Span(1, 31)
                    }),
                    NodePtr(new Expansion(
                        NodePtr(new ID("kwargs", Span(1, 40, 6))),
                        Span(1, 38, 2)
                    ))
                ),
                Span(1, 3, 1, 46)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1,)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(NodePtr(new Int32(1, Span(1, 4)))),
                {},
                Span(1, 3, 4)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(1, kw1=\"yes\", 2)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                vec(NodePtr(new Int32(1, Span(1, 4)))),
                vec(
                    NodePtr(new KeywordArg(
                        NodePtr(new ID("kw1", Span(1, 7, 3))),
                        NodePtr(new String("yes", Span(1, 11, 5))),
                        Span(1, 10)
                    )),
                    NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedKeywordArgExprErr()),
                        Comp(OpID::PLAIN_INT, "2", Span(1, 18))
                    ))
                ),
                Span(1, 3, 1, 19)
            )),
            Span(1, 3)
        ));

        REQUIRE(*capture(
            "fn(break=1)"
        ).node() == Call(
            NodePtr(new ID("fn", Span(1, 1, 2))),
            NodePtr(new Args(
                {},
                vec(NodePtr(new KeywordArg(
                    NodePtr(new ErrorNode(
                        ErrPtr(new ExpectedValueErr()),
                        Span(1, 4, 5)
                    )),
                    NodePtr(new Int32(1, Span(1, 10))),
                    Span(1, 9)
                ))),
                Span(1, 3, 1, 11)
            )),
            Span(1, 3)
        ));
    }

    SECTION("Continue") {
        REQUIRE(*capture(
            "continue"
        ).node() == Continue(
            Span(1, 1, 8)
        ));
    }

    SECTION("Declare") {
        REQUIRE(*capture(
            "a: Int"
        ).node() == Declare(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("Int", Span(1, 4, 3))),
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "3: Int"
        ).node() == Declare(
            NodePtr(new NumID(3, Span(1, 1))),
            NodePtr(new ID("Int", Span(1, 4, 3))),
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "true: Int"
        ).node() == Declare(
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(OpID::TRUE, Span(1, 1, 4))
            )),
            NodePtr(new ID("Int", Span(1, 7, 3))),
            Span(1, 5)
        ));
    }

    SECTION("Def") {
        auto cc = capture(
            "def f():\n"
            "    return 0"
        );

        REQUIRE(cc.ops() == vec(
            Op(OpID::CONSTRUCT, Span(1, 1, 3)),
            Op(OpID::DEF, Span(1, 1, 3)),
            Op(OpID::ID, "f", Span(1, 5)),
            Op(OpID::CALL, Span(1, 6)),
            Op(OpID::GROUP, Span(1, 6)),
            Op(OpID::NOTHING, Span(1, 7)),
            Op(OpID::END, Span(1, 7)),
            Op(OpID::LABEL, Span(1, 8)),
            Op(OpID::BLOCK, Span(2, 5)),
            Op(OpID::RETURN, Span(2, 5, 6)),
            Op(OpID::PLAIN_INT, "0", Span(2, 12)),
            Op(OpID::STMT, Span(2, 13)),
            Op(OpID::END, Span(2, 12)),
            Op(OpID::STMT, Span(2, 12)),
            Op(OpID::END, Span(2, 12)),
            Op(OpID::STMT, Span(2, 12))
        ));

        REQUIRE(cc.comp() == Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::CALL,
                        Comp(OpID::ID, "f", Span(1, 5)),
                        Comp(
                            OpID::GROUP,
                            Comp(OpID::NOTHING, Span(1, 7)),
                            Span(1, 6, 2)
                        ),
                        Span(1, 6)
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", Span(2, 12)),
                            Span(2, 5, 6)
                        )),
                        Span(2, 5, 2, 12)
                    ),
                    Span(1, 8)
                ),
                Span(1, 1, 3)
            )),
            Span(1, 1, 2, 12)
        ));

        REQUIRE(*cc.node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs({}, {}, Span(1, 6, 2))),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f() -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs({}, {}, Span(1, 6, 2))),
                nullptr,
                NodePtr(new ID("Int", Span(1, 12, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(arg) -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new ID("arg", Span(1, 7, 3)))),
                    {},
                    Span(1, 6, 5)
                )),
                nullptr,
                NodePtr(new ID("Int", Span(1, 15, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(arg1, arg2: Int) -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", Span(1, 7, 4))),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", Span(1, 13, 4))),
                            NodePtr(new ID("Int", Span(1, 19, 3))),
                            Span(1, 17)
                        ))
                    ),
                    {},
                    Span(1, 6, 1, 22)
                )),
                nullptr,
                NodePtr(new ID("Int", Span(1, 27, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(arg1, arg2: Int, *args) -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", Span(1, 7, 4))),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", Span(1, 13, 4))),
                            NodePtr(new ID("Int", Span(1, 19, 3))),
                            Span(1, 17)
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(1, 25, 4))),
                            Span(1, 24)
                        ))
                    ),
                    {},
                    Span(1, 6, 1, 29)
                )),
                nullptr,
                NodePtr(new ID("Int", Span(1, 34, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(arg1, arg2: Int, *args, *, kw1=1, kw2: Bool) -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", Span(1, 7, 4))),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", Span(1, 13, 4))),
                            NodePtr(new ID("Int", Span(1, 19, 3))),
                            Span(1, 17)
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(1, 25, 4))),
                            Span(1, 24)
                        ))
                    ),
                    vec(
                        NodePtr(new Defaulted(
                            NodePtr(new ID("kw1", Span(1, 34, 3))),
                            NodePtr(new Int32(1, Span(1, 38))),
                            Span(1, 37)
                        )),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("kw2", Span(1, 41, 3))),
                            NodePtr(new ID("Bool", Span(1, 46, 4))),
                            Span(1, 44)
                        ))
                    ),
                    Span(1, 6, 1, 50)
                )),
                nullptr,
                NodePtr(new ID("Int", Span(1, 55, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(\n"
            "    arg1, arg2: Int, *args, *, kw1=1, kw2: Bool, **kwargs\n"
            ") -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", Span(2, 5, 4))),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", Span(2, 11, 4))),
                            NodePtr(new ID("Int", Span(2, 17, 3))),
                            Span(2, 15)
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(2, 23, 4))),
                            Span(2, 22)
                        ))
                    ),
                    vec(
                        NodePtr(new Defaulted(
                            NodePtr(new ID("kw1", Span(2, 32, 3))),
                            NodePtr(new Int32(1, Span(2, 36))),
                            Span(2, 35)
                        )),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("kw2", Span(2, 39, 3))),
                            NodePtr(new ID("Bool", Span(2, 44, 4))),
                            Span(2, 42)
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", Span(2, 52, 6))),
                            Span(2, 50, 2)
                        ))
                    ),
                    Span(1, 6, 3, 1)
                )),
                nullptr,
                NodePtr(new ID("Int", Span(3, 6, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(4, 12))),
                        Span(4, 5, 6)
                    ))),
                    Span(4, 5, 4, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(a: Int, b: String):\n"
            "    return 0\n"
            "case (c: Float32, true) -> Int64:\n"
            "    return 3s64\n"
            "case (1, *, 2 as kw):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs(
                        vec(
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("a", Span(1, 7))),
                                NodePtr(new ID("Int", Span(1, 10, 3))),
                                Span(1, 8)
                            )),
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("b", Span(1, 15))),
                                NodePtr(new ID("String", Span(1, 18, 6))),
                                Span(1, 16)
                            ))
                        ),
                        {},
                        Span(1, 6, 1, 24)
                    )),
                    nullptr,
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs(
                        vec(
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("c", Span(3, 7))),
                                NodePtr(new ID("Float32", Span(3, 10, 7))),
                                Span(3, 8)
                            )),
                            NodePtr(new Bool(true, Span(3, 19, 4)))
                        ),
                        {},
                        Span(3, 6, 3, 23)
                    )),
                    nullptr,
                    NodePtr(new ID("Int64", Span(3, 28, 5))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int64(3, Span(4, 12, 4))),
                            Span(4, 5, 6)
                        ))),
                        Span(4, 5, 4, 15)
                    )),
                    Span(3, 1, 4)
                )),
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs(
                        vec(NodePtr(new Int32(1, Span(5, 7)))),
                        vec(NodePtr(new As(
                            NodePtr(new Int32(2, Span(5, 13))),
                            NodePtr(new ID("kw", Span(5, 18, 2))),
                            Span(5, 15, 2)
                        ))),
                        Span(5, 6, 5, 20)
                    )),
                    nullptr,
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(6, 12))),
                            Span(6, 5, 6)
                        ))),
                        Span(6, 5, 6, 12)
                    )),
                    Span(5, 1, 4)
                ))
            ),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(a,):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new ID("a", Span(1, 7)))),
                    {},
                    Span(1, 6, 4)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def 3():\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new NumID(3, Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs({}, {}, Span(1, 6, 2))),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(x: Int = 1):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("x", Span(1, 7))),
                            NodePtr(new ID("Int", Span(1, 10, 3))),
                            Span(1, 8)
                        )),
                        NodePtr(new Int32(1, Span(1, 16))),
                        Span(1, 14)
                    ))),
                    {},
                    Span(1, 6, 1, 17)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(x: Int) if x > 2 -> Int:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new ID("x", Span(1, 7))),
                        NodePtr(new ID("Int", Span(1, 10, 3))),
                        Span(1, 8)
                    ))),
                    {},
                    Span(1, 6, 1, 13)
                )),
                NodePtr(new GreaterThan(
                    NodePtr(new ID("x", Span(1, 18))),
                    NodePtr(new Int32(2, Span(1, 22))),
                    Span(1, 20)
                )),
                NodePtr(new ID("Int", Span(1, 27, 3))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f((3, x) as y = (3, 4)):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new As(
                            NodePtr(new MatchTuple(
                                vec(
                                    NodePtr(new Int32(3, Span(1, 8))),
                                    NodePtr(new ID("x", Span(1, 11)))
                                ),
                                Span(1, 7, 6)
                            )),
                            NodePtr(new ID("y", Span(1, 17))),
                            Span(1, 14, 2)
                        )),
                        NodePtr(new Tuple(
                            vec(
                                NodePtr(new Int32(3, Span(1, 22))),
                                NodePtr(new Int32(4, Span(1, 25)))
                            ),
                            Span(1, 21, 6)
                        )),
                        Span(1, 19)
                    ))),
                    {},
                    Span(1, 6, 1, 27)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f()\n"
            "case (x)"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs({}, {}, Span(1, 6, 2))),
                    nullptr,
                    nullptr,
                    NodePtr(new ErrorNode(
                        ErrPtr(new MissingBodyErr()),
                        Span(1, 8)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs(
                        vec(NodePtr(new ID("x", Span(2, 7)))),
                        {},
                        Span(2, 6, 3)
                    )),
                    nullptr,
                    nullptr,
                    NodePtr(new ErrorNode(
                        ErrPtr(new MissingBodyErr()),
                        Span(2, 9)
                    )),
                    Span(2, 1, 4)
                ))
            ),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def:\n"
            "    return 0\n"
            "case:\n"
            "    return 1"
        ).node() == Def(
            NodePtr(new ErrorNode(
                ErrPtr(new MissingIDErr()),
                Span(1, 4)
            )),
            vec(
                NodePtr(new DefCase(
                    NodePtr(new ErrorNode(
                        ErrPtr(new MissingArgSpecErr()),
                        Span(1, 4)
                    )),
                    nullptr,
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new DefCase(
                    NodePtr(new ErrorNode(
                        ErrPtr(new MissingArgSpecErr()),
                        Span(3, 5)
                    )),
                    nullptr,
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(1, Span(4, 12))),
                            Span(4, 5, 6)
                        ))),
                        Span(4, 5, 4, 12)
                    )),
                    Span(3, 1, 4)
                ))
            ),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f:\n"
            "    return 0\n"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingArgSpecErr()),
                    Span(1, 6)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def ():\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ErrorNode(
                ErrPtr(new MissingIDErr()),
                Span(1, 5)
            )),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs({}, {}, Span(1, 5, 2))),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def true():\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(OpID::TRUE, Span(1, 5, 4))
            )),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs({}, {}, Span(1, 9, 2))),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f[]:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedParentheticalErr()),
                    Comp(
                        OpID::LIST,
                        Comp(OpID::NOTHING, Span(1, 7)),
                        Span(1, 6, 2)
                    )
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(x, *, *, y):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new ID("x", Span(1, 7)))),
                    vec(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedKeywordArgMatchExprErr()),
                            Comp(OpID::POS_KW_SEP, Span(1, 13))
                        )),
                        NodePtr(new ID("y", Span(1, 16)))
                    ),
                    Span(1, 6, 1, 17)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(*, *args):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    {},
                    vec(NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedKeywordArgMatchExprErr()),
                        Comp(
                            OpID::UNPACK_ARGS,
                            Comp(OpID::ID, "args", Span(1, 11, 4)),
                            Span(1, 10)
                        )
                    ))),
                    Span(1, 6, 1, 15)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(a=1=2):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedDefaultableExprErr()),
                            Comp(
                                OpID::BIND,
                                Comp(OpID::ID, "a", Span(1, 7)),
                                Comp(OpID::PLAIN_INT, "1", Span(1, 9)),
                                Span(1, 8)
                            )
                        )),
                        NodePtr(new Int32(2, Span(1, 11))),
                        Span(1, 10)
                    ))),
                    {},
                    Span(1, 6, 1, 12)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(*args=1):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedDefaultableExprErr()),
                            Comp(
                                OpID::UNPACK_ARGS,
                                Comp(OpID::ID, "args", Span(1, 8, 4)),
                                Span(1, 7)
                            )
                        )),
                        NodePtr(new Int32(1, Span(1, 13))),
                        Span(1, 12)
                    ))),
                    {},
                    Span(1, 6, 1, 14)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f(**kwargs=1):\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(NodePtr(new DefCase(
                NodePtr(new MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedDefaultableExprErr()),
                            Comp(
                                OpID::UNPACK_KWARGS,
                                Comp(OpID::ID, "kwargs", Span(1, 9, 6)),
                                Span(1, 7, 2)
                            )
                        )),
                        NodePtr(new Int32(1, Span(1, 16))),
                        Span(1, 15)
                    ))),
                    {},
                    Span(1, 6, 1, 17)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def f():\n"
            "    return 0\n"
            "elif true:\n"
            "    return 1"
        ).node() == Def(
            NodePtr(new ID("f", Span(1, 5))),
            vec(
                NodePtr(new DefCase(
                    NodePtr(new MatchArgs({}, {}, Span(1, 6, 2))),
                    nullptr,
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedCaseErr()),
                    Comp(
                        OpID::ELIF,
                        Comp(
                            OpID::LABEL,
                            Comp(OpID::TRUE, Span(3, 6, 4)),
                            Comp(
                                OpID::BLOCK,
                                vec(Comp(
                                    OpID::RETURN,
                                    Comp(OpID::PLAIN_INT, "1", Span(4, 12)),
                                    Span(4, 5, 6)
                                )),
                                Span(4, 5, 4, 12)
                            ),
                            Span(3, 10)
                        ),
                        Span(3, 1, 4)
                    ),
                    Span(3, 1, 4)
                ))
            ),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "def break:\n"
            "    return 0"
        ).node() == Def(
            NodePtr(new ErrorNode(
                ErrPtr(new ExpectedValueErr()),
                Span(1, 5, 5)
            )),
            vec(NodePtr(new DefCase(
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingArgSpecErr()),
                    Span(1, 10)
                )),
                nullptr,
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 3)
            ))),
            Span(1, 1, 3)
        ));
    }

    SECTION("DIV") {
        REQUIRE(*capture(
            "a / b"
        ).node() == Divide(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("EQ") {
        REQUIRE(*capture(
            "a == b"
        ).node() == Equals(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("Enter Object") {
        REQUIRE(*capture(
            "a:\n"
            "    .b = 1"
        ).node() == EnterObject(
            NodePtr(new ID("a", Span(1, 1))),
            vec(NodePtr(new Assign(
                NodePtr(new Var(
                    1,
                    NodePtr(new ID("b", Span(2, 6))),
                    Span(2, 5)
                )),
                NodePtr(new Int32(1, Span(2, 10))),
                Span(2, 8)
            ))),
            Span(1, 2)
        ));
    }

    SECTION("For") {
        REQUIRE(*capture(
            "for x in c:\n"
            "    a += x"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 10))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("x", Span(1, 5))),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new IAdd(
                        NodePtr(new ID("a", Span(2, 5))),
                        NodePtr(new ID("x", Span(2, 10))),
                        Span(2, 7, 2)
                    ))),
                    Span(2, 5, 2, 10)
                )),
                Span(1, 1, 3)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x in c if x > 0:\n"
            "    a += x"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 10))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("x", Span(1, 5))),
                NodePtr(new GreaterThan(
                    NodePtr(new ID("x", Span(1, 15))),
                    NodePtr(new Int32(0, Span(1, 19))),
                    Span(1, 17)
                )),
                NodePtr(new Block(
                    vec(NodePtr(new IAdd(
                        NodePtr(new ID("a", Span(2, 5))),
                        NodePtr(new ID("x", Span(2, 10))),
                        Span(2, 7, 2)
                    ))),
                    Span(2, 5, 2, 10)
                )),
                Span(1, 1, 3)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x, y in c:\n"
            "    a += x + y\n"
            "case _:\n"
            "    a += 1"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 13))),
            vec(
                NodePtr(new MatchCase(
                    NodePtr(new MatchTuple(
                        vec(
                            NodePtr(new ID("x", Span(1, 5))),
                            NodePtr(new ID("y", Span(1, 8)))
                        ),
                        Span(1, 5, 4)
                    )),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new IAdd(
                            NodePtr(new ID("a", Span(2, 5))),
                            NodePtr(new Add(
                                NodePtr(new ID("x", Span(2, 10))),
                                NodePtr(new ID("y", Span(2, 14))),
                                Span(2, 12)
                            )),
                            Span(2, 7, 2)
                        ))),
                        Span(2, 5, 2, 14)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new MatchCase(
                    NodePtr(new Placeholder(Span(3, 6))),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new IAdd(
                            NodePtr(new ID("a", Span(4, 5))),
                            NodePtr(new Int32(1, Span(4, 10))),
                            Span(4, 7, 2)
                        ))),
                        Span(4, 5, 4, 10)
                    )),
                    Span(3, 1, 4)
                ))
            ),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x in c:\n"
            "    break\n"
            "else:\n"
            "    a += 1"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 10))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("x", Span(1, 5))),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Break(Span(2, 5, 5)))),
                    Span(2, 5, 5)
                )),
                Span(1, 1, 3)
            ))),
            NodePtr(new Block(
                vec(NodePtr(new IAdd(
                    NodePtr(new ID("a", Span(4, 5))),
                    NodePtr(new Int32(1, Span(4, 10))),
                    Span(4, 7, 2)
                ))),
                Span(3, 1, 4)
            )),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x, y in c:\n"
            "    a += x + y\n"
            "case _:\n"
            "    break\n"
            "else:\n"
            "    a += 1"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 13))),
            vec(
                NodePtr(new MatchCase(
                    NodePtr(new MatchTuple(
                        vec(
                            NodePtr(new ID("x", Span(1, 5))),
                            NodePtr(new ID("y", Span(1, 8)))
                        ),
                        Span(1, 5, 4)
                    )),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new IAdd(
                            NodePtr(new ID("a", Span(2, 5))),
                            NodePtr(new Add(
                                NodePtr(new ID("x", Span(2, 10))),
                                NodePtr(new ID("y", Span(2, 14))),
                                Span(2, 12)
                            )),
                            Span(2, 7, 2)
                        ))),
                        Span(2, 5, 2, 14)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new MatchCase(
                    NodePtr(new Placeholder(Span(3, 6))),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Break(Span(4, 5, 5)))),
                        Span(4, 5, 5)
                    )),
                    Span(3, 1, 4)
                ))
            ),
            NodePtr(new Block(
                vec(NodePtr(new IAdd(
                    NodePtr(new ID("a", Span(6, 5))),
                    NodePtr(new Int32(1, Span(6, 10))),
                    Span(6, 7, 2)
                ))),
                Span(5, 1, 4)
            )),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x, in c:\n"
            "    a += x"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 11))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new ID("x", Span(1, 5)))),
                    Span(1, 5, 2)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new IAdd(
                        NodePtr(new ID("a", Span(2, 5))),
                        NodePtr(new ID("x", Span(2, 10))),
                        Span(2, 7, 2)
                    ))),
                    Span(2, 5, 2, 10)
                )),
                Span(1, 1, 3)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x in c"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 10))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("x", Span(1, 5))),
                nullptr,
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingBodyErr()),
                    Span(1, 11)
                )),
                Span(1, 1, 3)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x:\n"
            "    a += x"
        ).node() == For(
            NodePtr(new ErrorNode(
                ErrPtr(new MissingIterableErr()),
                Span(1, 6)
            )),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("x", Span(1, 5))),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new IAdd(
                        NodePtr(new ID("a", Span(2, 5))),
                        NodePtr(new ID("x", Span(2, 10))),
                        Span(2, 7, 2)
                    ))),
                    Span(2, 5, 2, 10)
                )),
                Span(1, 1, 3)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "for x in c:\n"
            "    a += x\n"
            "elif true:\n"
            "    return 0"
        ).node() == For(
            NodePtr(new ID("c", Span(1, 10))),
            vec(
                NodePtr(new MatchCase(
                    NodePtr(new ID("x", Span(1, 5))),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new IAdd(
                            NodePtr(new ID("a", Span(2, 5))),
                            NodePtr(new ID("x", Span(2, 10))),
                            Span(2, 7, 2)
                        ))),
                        Span(2, 5, 2, 10)
                    )),
                    Span(1, 1, 3)
                )),
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedCaseErr()),
                    Comp(
                        OpID::ELIF,
                        Comp(
                            OpID::LABEL,
                            Comp(OpID::TRUE, Span(3, 6, 4)),
                            Comp(
                                OpID::BLOCK,
                                vec(Comp(
                                    OpID::RETURN,
                                    Comp(OpID::PLAIN_INT, "0", Span(4, 12)),
                                    Span(4, 5, 6)
                                )),
                                Span(4, 5, 4, 12)
                            ),
                            Span(3, 10)
                        ),
                        Span(3, 1, 4)
                    ),
                    Span(3, 1, 4)
                ))
            ),
            nullptr,
            Span(1, 1, 3)
        ));
    }

    SECTION("GT") {
        REQUIRE(*capture(
            "a > b"
        ).node() == GreaterThan(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("GTE") {
        REQUIRE(*capture(
            "a >= b"
        ).node() == GreaterThanOrEqual(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("GetAttr") {
        REQUIRE(*capture(
            "a.b"
        ).node() == GetAttr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 3))),
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "a.3"
        ).node() == GetAttr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new NumID(3, Span(1, 3))),
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "a.true"
        ).node() == GetAttr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(OpID::TRUE, Span(1, 3, 4))
            )),
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "a.break"
        ).node() == GetAttr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ErrorNode(
                ErrPtr(new ExpectedValueErr()),
                Span(1, 3, 5)
            )),
            Span(1, 2)
        ));
    }

    SECTION("IADD") {
        REQUIRE(*capture(
            "a += b"
        ).node() == IAdd(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("IBAND") {
        REQUIRE(*capture(
            "a &= b"
        ).node() == IBitAnd(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("IBOR") {
        REQUIRE(*capture(
            "a |= b"
        ).node() == IBitOr(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("IBXOR") {
        REQUIRE(*capture(
            "a ^= b"
        ).node() == IBitXor(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("ID") {
        REQUIRE(*capture(
            "a"
        ).node() == ID(
            "a", Span(1, 1)
        ));
    }

    SECTION("IDIV") {
        REQUIRE(*capture(
            "a /= b"
        ).node() == IDivide(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("ILSH") {
        REQUIRE(*capture(
            "a <<= b"
        ).node() == ILeftShift(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 7))),
            Span(1, 3, 3)
        ));
    }

    SECTION("IMOD") {
        REQUIRE(*capture(
            "a %= b"
        ).node() == IMod(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("IMUL") {
        REQUIRE(*capture(
            "a *= b"
        ).node() == IMultiply(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("IPOW") {
        REQUIRE(*capture(
            "a **= b"
        ).node() == IPower(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 7))),
            Span(1, 3, 3)
        ));
    }

    SECTION("IRSH") {
        REQUIRE(*capture(
            "a >>= b"
        ).node() == IRightShift(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 7))),
            Span(1, 3, 3)
        ));
    }

    SECTION("ISUB") {
        REQUIRE(*capture(
            "a -= b"
        ).node() == ISubtract(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("If") {
        REQUIRE(*capture(
            "if x:\n"
            "    return 0"
        ).node() == If(
            vec(NodePtr(new Case(
                NodePtr(new ID("x", Span(1, 4))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 2)
            ))),
            nullptr,
            Span(1, 1, 2)
        ));

        auto cc = capture(
            "if x:\n"
            "    return 0\n"
            "elif y:\n"
            "    return 1"
        );

        REQUIRE(cc.ops() == vec(
            Op(OpID::CONSTRUCT, Span(1, 1, 2)),
            Op(OpID::IF, Span(1, 1, 2)),
            Op(OpID::ID, "x", Span(1, 4)),
            Op(OpID::LABEL, Span(1, 5)),
            Op(OpID::BLOCK, Span(2, 5)),
            Op(OpID::RETURN, Span(2, 5, 6)),
            Op(OpID::PLAIN_INT, "0", Span(2, 12)),
            Op(OpID::STMT, Span(2, 13)),
            Op(OpID::END, Span(2, 12)),
            Op(OpID::STMT, Span(2, 12)),
            Op(OpID::ELIF, Span(3, 1, 4)),
            Op(OpID::ID, "y", Span(3, 6)),
            Op(OpID::LABEL, Span(3, 7)),
            Op(OpID::BLOCK, Span(4, 5)),
            Op(OpID::RETURN, Span(4, 5, 6)),
            Op(OpID::PLAIN_INT, "1", Span(4, 12)),
            Op(OpID::STMT, Span(4, 13)),
            Op(OpID::END, Span(4, 12)),
            Op(OpID::STMT, Span(4, 12)),
            Op(OpID::END, Span(4, 12)),
            Op(OpID::STMT, Span(4, 12))
        ));

        REQUIRE(cc.comp() == Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::IF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "x", Span(1, 4)),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", Span(2, 12)),
                                Span(2, 5, 6)
                            )),
                            Span(2, 5, 2, 12)
                        ),
                        Span(1, 5)
                    ),
                    Span(1, 1, 2)
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", Span(3, 6)),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "1", Span(4, 12)),
                                Span(4, 5, 6)
                            )),
                            Span(4, 5, 4, 12)
                        ),
                        Span(3, 7)
                    ),
                    Span(3, 1, 4)
                )
            ),
            Span(1, 1, 4, 12)
        ));

        REQUIRE(*cc.node() == If(
            vec(
                NodePtr(new Case(
                    NodePtr(new ID("x", Span(1, 4))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 2)
                )),
                NodePtr(new Case(
                    NodePtr(new ID("y", Span(3, 6))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(1, Span(4, 12))),
                            Span(4, 5, 6)
                        ))),
                        Span(4, 5, 4, 12)
                    )),
                    Span(3, 1, 4)
                ))
            ),
            nullptr,
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x:\n"
            "    return 0\n"
            "elif y:\n"
            "    return 1\n"
            "else:\n"
            "    return 2"
        ).node() == If(
            vec(
                NodePtr(new Case(
                    NodePtr(new ID("x", Span(1, 4))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 2)
                )),
                NodePtr(new Case(
                    NodePtr(new ID("y", Span(3, 6))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(1, Span(4, 12))),
                            Span(4, 5, 6)
                        ))),
                        Span(4, 5, 4, 12)
                    )),
                    Span(3, 1, 4)
                ))
            ),
            NodePtr(new Block(
                vec(NodePtr(new Return(
                    NodePtr(new Int32(2, Span(6, 12))),
                    Span(6, 5, 6)
                ))),
                Span(5, 1, 4)
            )),
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x:\n"
            "    return 0\n"
            "else:\n"
            "    return 1"
        ).node() == If(
            vec(NodePtr(new Case(
                NodePtr(new ID("x", Span(1, 4))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 2)
            ))),
            NodePtr(new Block(
                vec(NodePtr(new Return(
                    NodePtr(new Int32(1, Span(4, 12))),
                    Span(4, 5, 6)
                ))),
                Span(3, 1, 4)
            )),
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x:\n"
            "    return 0\n"
            "elif y:\n"
            "    return 1\n"
            "elif z:\n"
            "    return 2"
        ).node() == If(
            vec(
                NodePtr(new Case(
                    NodePtr(new ID("x", Span(1, 4))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 2)
                )),
                NodePtr(new Case(
                    NodePtr(new ID("y", Span(3, 6))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(1, Span(4, 12))),
                            Span(4, 5, 6)
                        ))),
                        Span(4, 5, 4, 12)
                    )),
                    Span(3, 1, 4)
                )),
                NodePtr(new Case(
                    NodePtr(new ID("z", Span(5, 6))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(2, Span(6, 12))),
                            Span(6, 5, 6)
                        ))),
                        Span(6, 5, 6, 12)
                    )),
                    Span(5, 1, 4)
                ))
            ),
            nullptr,
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if:\n"
            "    return 0"
        ).node() == If(
            vec(NodePtr(new Case(
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingPredicateErr()),
                    Span(1, 3)
                )),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 2)
            ))),
            nullptr,
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x"
        ).node() == If(
            vec(NodePtr(new Case(
                NodePtr(new ID("x", Span(1, 4))),
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingBodyErr()),
                    Span(1, 5)
                )),
                Span(1, 1, 2)
            ))),
            nullptr,
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x:\n"
            "    return 0\n"
            "else y:\n"
            "    return 1"
        ).node() == If(
            vec(NodePtr(new Case(
                NodePtr(new ID("x", Span(1, 4))),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(2, 12))),
                        Span(2, 5, 6)
                    ))),
                    Span(2, 5, 2, 12)
                )),
                Span(1, 1, 2)
            ))),
            NodePtr(new Case(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new UnexpectedPredicateErr()),
                    Comp(OpID::ID, "y", Span(3, 6))
                )),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(1, Span(4, 12))),
                        Span(4, 5, 6)
                    ))),
                    Span(4, 5, 4, 12)
                )),
                Span(3, 1, 4)
            )),
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "if x:\n"
            "    return 0\n"
            "case y:\n"
            "    return 1"
        ).node() == If(
            vec(
                NodePtr(new Case(
                    NodePtr(new ID("x", Span(1, 4))),
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(2, 12))),
                            Span(2, 5, 6)
                        ))),
                        Span(2, 5, 2, 12)
                    )),
                    Span(1, 1, 2)
                )),
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedElifOrElseErr()),
                    Comp(
                        OpID::CASE,
                        Comp(
                            OpID::LABEL,
                            Comp(OpID::ID, "y", Span(3, 6)),
                            Comp(
                                OpID::BLOCK,
                                vec(Comp(
                                    OpID::RETURN,
                                    Comp(OpID::PLAIN_INT, "1", Span(4, 12)),
                                    Span(4, 5, 6)
                                )),
                                Span(4, 5, 4, 12)
                            ),
                            Span(3, 7)
                        ),
                        Span(3, 1, 4)
                    ),
                    Span(3, 1, 4)
                ))
            ),
            nullptr,
            Span(1, 1, 2)
        ));
    }

    SECTION("Init") {
        REQUIRE(*capture(
            "a: Int = 3"
        ).node() == Init(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("Int", Span(1, 4, 3))),
            NodePtr(new Int32(3, Span(1, 10))),
            Span(1, 8)
        ));

        REQUIRE(*capture(
            "3: Int = 3"
        ).node() == Init(
            NodePtr(new NumID(3, Span(1, 1))),
            NodePtr(new ID("Int", Span(1, 4, 3))),
            NodePtr(new Int32(3, Span(1, 10))),
            Span(1, 8)
        ));

        REQUIRE(*capture(
            "true: Int = 3"
        ).node() == Init(
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(OpID::TRUE, Span(1, 1, 4))
            )),
            NodePtr(new ID("Int", Span(1, 7, 3))),
            NodePtr(new Int32(3, Span(1, 13))),
            Span(1, 11)
        ));
    }

    SECTION("LSH") {
        REQUIRE(*capture(
            "a << b"
        ).node() == LeftShift(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("LT") {
        REQUIRE(*capture(
            "a < b"
        ).node() == LessThan(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("LTE") {
        REQUIRE(*capture(
            "a <= b"
        ).node() == LessThanOrEqual(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("Lambda Expression") {
        REQUIRE(*capture(
            "%(%1 + %2)"
        ).node() == Lambda(
            NodePtr(new Add(
                NodePtr(new LambdaVar(
                    1,
                    NodePtr(new NumID(1, Span(1, 4))),
                    Span(1, 3)
                )),
                NodePtr(new LambdaVar(
                    1,
                    NodePtr(new NumID(2, Span(1, 9))),
                    Span(1, 8)
                )),
                Span(1, 6)
            )),
            Span(1, 1, 10)
        ));

        REQUIRE(*capture(
            "%%(%1 + %2)"
        ).node() == LambdaVar(
            2,
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(
                    OpID::GROUP,
                    Comp(
                        OpID::ADD,
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "1", Span(1, 5)),
                            Span(1, 4)
                        ),
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "2", Span(1, 10)),
                            Span(1, 9)
                        ),
                        Span(1, 7)
                    ),
                    Span(1, 3, 1, 11)
                )
            )),
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "%[%1 + %2]"
        ).node() == LambdaVar(
            1,
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedGeneralIDErr()),
                Comp(
                    OpID::LIST,
                    Comp(
                        OpID::ADD,
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "1", Span(1, 4)),
                            Span(1, 3)
                        ),
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "2", Span(1, 9)),
                            Span(1, 8)
                        ),
                        Span(1, 6)
                    ),
                    Span(1, 2, 1, 10)
                )
            )),
            Span(1, 1)
        ));
    }

    SECTION("Lambda Var") {
        REQUIRE(*capture(
            "%1"
        ).node() == LambdaVar(
            1,
            NodePtr(new NumID(1, Span(1, 2))),
            Span(1, 1)
        ));

        REQUIRE(*capture(
            "%a"
        ).node() == LambdaVar(
            1,
            NodePtr(new ID("a", Span(1, 2))),
            Span(1, 1)
        ));

        REQUIRE(*capture(
            "%%%1"
        ).node() == LambdaVar(
            3,
            NodePtr(new NumID(1, Span(1, 4))),
            Span(1, 1, 3)
        ));
    }

    SECTION("Lambda Var Args") {
        REQUIRE(*capture(
            "%*"
        ).node() == LambdaArgs(
            1, Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "%%%*"
        ).node() == LambdaArgs(
            3, Span(1, 1, 4)
        ));
    }

    SECTION("Lambda Var Keyword Args") {
        REQUIRE(*capture(
            "%**"
        ).node() == LambdaKeywordArgs(
            1, Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "%%%**"
        ).node() == LambdaKeywordArgs(
            3, Span(1, 1, 5)
        ));
    }

    SECTION("List") {
        REQUIRE(*capture(
            "[]"
        ).node() == List(
            {}, Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "[a]"
        ).node() == List(
            vec(NodePtr(new ID("a", Span(1, 2)))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "[a, b, c]"
        ).node() == List(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new ID("b", Span(1, 5))),
                NodePtr(new ID("c", Span(1, 8)))
            ),
            Span(1, 1, 9)
        ));

        REQUIRE(*capture(
            "[a, *args, b]"
        ).node() == List(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", Span(1, 6, 4))),
                    Span(1, 5)
                )),
                NodePtr(new ID("b", Span(1, 12)))
            ),
            Span(1, 1, 13)
        ));

        REQUIRE(*capture(
            "[a,]"
        ).node() == List(
            vec(NodePtr(new ID("a", Span(1, 2)))),
            Span(1, 1, 4)
        ));
    }

    SECTION("Literals") {
        REQUIRE(*capture(
            "true"
        ).node() == Bool(
            true, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "false"
        ).node() == Bool(
            false, Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "0"
        ).node() == Int32(
            0, Span(1, 1)
        ));

        REQUIRE(*capture(
            "0s8"
        ).node() == Int8(
            0, Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "0s16"
        ).node() == Int16(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0s32"
        ).node() == Int32(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0s64"
        ).node() == Int64(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0u8"
        ).node() == UInt8(
            0, Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "0u16"
        ).node() == UInt16(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0u32"
        ).node() == UInt32(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0u64"
        ).node() == UInt64(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0f32"
        ).node() == Float32(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "0f64"
        ).node() == Float64(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "1e7"
        ).node() == Float64(
            1e7, Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "1e-7"
        ).node() == Float64(
            1e-7, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "12.34"
        ).node() == Float64(
            12.34, Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "12.34f32"
        ).node() == Float32(
            12.34f, Span(1, 1, 8)
        ));

        REQUIRE(*capture(
            "12.34e5"
        ).node() == Float64(
            12.34e5, Span(1, 1, 7)
        ));

        REQUIRE(*capture(
            "12.34e5f32"
        ).node() == Float32(
            12.34e5f, Span(1, 1, 10)
        ));

        REQUIRE(*capture(
            "1e46f32"
        ).node() == ErrorWithComp(
            ErrPtr(new OutOfRangeErr()),
            Comp(OpID::FLOAT_TAIL, "1e46f32", Span(1, 1, 7))
        ));


        REQUIRE(*capture(
            "1e309"
        ).node() == ErrorWithComp(
            ErrPtr(new OutOfRangeErr()),
            Comp(OpID::FLOAT_TAIL, "1e309", Span(1, 1, 5))
        ));

        REQUIRE(*capture(
            "'\\0'"
        ).node() == Char(
            0, Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "\"asdf\""
        ).node() == String(
            "asdf", Span(1, 1, 6)
        ));
    }

    SECTION("MOD") {
        REQUIRE(*capture(
            "a % b"
        ).node() == Mod(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("MUL") {
        REQUIRE(*capture(
            "a * b"
        ).node() == Multiply(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("Map") {
        REQUIRE(*capture(
            "{}"
        ).node() == Map(
            {}, Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "{a: 1}"
        ).node() == Map(
            vec(NodePtr(new Entry(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new Int32(1, Span(1, 5))),
                Span(1, 3)
            ))),
            Span(1, 1, 6)
        ));

        REQUIRE(*capture(
            "{a: 1, b: 2, c: 3}"
        ).node() == Map(
            vec(
                NodePtr(new Entry(
                    NodePtr(new ID("a", Span(1, 2))),
                    NodePtr(new Int32(1, Span(1, 5))),
                    Span(1, 3)
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("b", Span(1, 8))),
                    NodePtr(new Int32(2, Span(1, 11))),
                    Span(1, 9)
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("c", Span(1, 14))),
                    NodePtr(new Int32(3, Span(1, 17))),
                    Span(1, 15)
                ))
            ),
            Span(1, 1, 18)
        ));

        REQUIRE(*capture(
            "{a: 1, **kwargs, b: 2}"
        ).node() == Map(
            vec(
                NodePtr(new Entry(
                    NodePtr(new ID("a", Span(1, 2))),
                    NodePtr(new Int32(1, Span(1, 5))),
                    Span(1, 3)
                )),
                NodePtr(new Expansion(
                    NodePtr(new ID("kwargs", Span(1, 10, 6))),
                    Span(1, 8, 2)
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("b", Span(1, 18))),
                    NodePtr(new Int32(2, Span(1, 21))),
                    Span(1, 19)
                ))
            ),
            Span(1, 1, 22)
        ));

        REQUIRE(*capture(
            "{a: 1,}"
        ).node() == Map(
            vec(NodePtr(new Entry(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new Int32(1, Span(1, 5))),
                Span(1, 3)
            ))),
            Span(1, 1, 7)
        ));

        REQUIRE(*capture(
            "{a: 1, break}"
        ).node() == Map(
            vec(
                NodePtr(new Entry(
                    NodePtr(new ID("a", Span(1, 2))),
                    NodePtr(new Int32(1, Span(1, 5))),
                    Span(1, 3)
                )),
                NodePtr(new ErrorNode(
                    ErrPtr(new ExpectedValueErr()),
                    Span(1, 8, 5)
                ))
            ),
            Span(1, 1, 13)
        ));
    }

    SECTION("Match") {
        REQUIRE(*capture(
            "match x\n"
            "case y:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("y", Span(2, 6))),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y: Int:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("y", Span(2, 6))),
                    NodePtr(new ID("Int", Span(2, 9, 3))),
                    Span(2, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y if y > 0:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("y", Span(2, 6))),
                NodePtr(new GreaterThan(
                    NodePtr(new ID("y", Span(2, 11))),
                    NodePtr(new Int32(0, Span(2, 15))),
                    Span(2, 13)
                )),
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y: Int:\n"
            "    return y\n"
            "case _:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(
                NodePtr(new MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("y", Span(2, 6))),
                        NodePtr(new ID("Int", Span(2, 9, 3))),
                        Span(2, 7)
                    )),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new ID("y", Span(3, 12))),
                            Span(3, 5, 6)
                        ))),
                        Span(3, 5, 3, 12)
                    )),
                    Span(2, 1, 4)
                )),
                NodePtr(new MatchCase(
                    NodePtr(new Placeholder(Span(4, 6))),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Return(
                            NodePtr(new Int32(0, Span(5, 12))),
                            Span(5, 5, 6)
                        ))),
                        Span(5, 5, 5, 12)
                    )),
                    Span(4, 1, 4)
                ))
            ),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (y):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("y", Span(2, 7))),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (y, z):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new ID("z", Span(2, 10)))
                    ),
                    Span(2, 6, 6)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (y, *args, z):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(2, 11, 4))),
                            Span(2, 10)
                        )),
                        NodePtr(new ID("z", Span(2, 17)))
                    ),
                    Span(2, 6, 2, 18)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (*args):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new Expansion(
                        NodePtr(new ID("args", Span(2, 8, 4))),
                        Span(2, 7)
                    ))),
                    Span(2, 6, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (y,):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new ID("y", Span(2, 7)))),
                    Span(2, 6, 4)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [y, z]:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new ID("z", Span(2, 10)))
                    ),
                    Span(2, 6, 6)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [y, *args, z]:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(2, 11, 4))),
                            Span(2, 10)
                        )),
                        NodePtr(new ID("z", Span(2, 17)))
                    ),
                    Span(2, 6, 2, 18)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [y,]:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new ID("y", Span(2, 7)))),
                    Span(2, 6, 4)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3: y, 4: z}:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(
                        NodePtr(new Entry(
                            NodePtr(new Int32(3, Span(2, 7))),
                            NodePtr(new ID("y", Span(2, 10))),
                            Span(2, 8)
                        )),
                        NodePtr(new Entry(
                            NodePtr(new Int32(4, Span(2, 13))),
                            NodePtr(new ID("z", Span(2, 16))),
                            Span(2, 14)
                        ))
                    ),
                    Span(2, 6, 2, 17)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3: Int() as y}:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new Entry(
                        NodePtr(new Int32(3, Span(2, 7))),
                        NodePtr(new As(
                            NodePtr(new MatchObject(
                                NodePtr(new ID("Int", Span(2, 10, 3))),
                                NodePtr(new MatchArgs({}, {}, Span(2, 13, 2))),
                                Span(2, 13)
                            )),
                            NodePtr(new ID("y", Span(2, 19))),
                            Span(2, 16, 2)
                        )),
                        Span(2, 8)
                    ))),
                    Span(2, 6, 2, 20)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3: y, 4: z, **kwargs}:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(
                        NodePtr(new Entry(
                            NodePtr(new Int32(3, Span(2, 7))),
                            NodePtr(new ID("y", Span(2, 10))),
                            Span(2, 8)
                        )),
                        NodePtr(new Entry(
                            NodePtr(new Int32(4, Span(2, 13))),
                            NodePtr(new ID("z", Span(2, 16))),
                            Span(2, 14)
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", Span(2, 21, 6))),
                            Span(2, 19, 2)
                        ))
                    ),
                    Span(2, 6, 2, 27)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3: y,}:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new Entry(
                        NodePtr(new Int32(3, Span(2, 7))),
                        NodePtr(new ID("y", Span(2, 10))),
                        Span(2, 8)
                    ))),
                    Span(2, 6, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case T(y, w=z):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", Span(2, 6))),
                    NodePtr(new MatchArgs(
                        vec(NodePtr(new ID("y", Span(2, 8)))),
                        vec(NodePtr(new MatchKeywordArg(
                            NodePtr(new ID("w", Span(2, 11))),
                            NodePtr(new ID("z", Span(2, 13))),
                            Span(2, 12)
                        ))),
                        Span(2, 7, 2, 14)
                    )),
                    Span(2, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case T(y, *args, **kwargs):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", Span(2, 6))),
                    NodePtr(new MatchArgs(
                        vec(
                            NodePtr(new ID("y", Span(2, 8))),
                            NodePtr(new Expansion(
                                NodePtr(new ID("args", Span(2, 12, 4))),
                                Span(2, 11)
                            ))
                        ),
                        vec(NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", Span(2, 20, 6))),
                            Span(2, 18, 2)
                        ))),
                        Span(2, 7, 2, 26)
                    )),
                    Span(2, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case T(y,):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", Span(2, 6))),
                    NodePtr(new MatchArgs(
                        vec(NodePtr(new ID("y", Span(2, 8)))),
                        {},
                        Span(2, 7, 4)
                    )),
                    Span(2, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case 1 || 2:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchOptions(
                    vec(
                        NodePtr(new Int32(1, Span(2, 6))),
                        NodePtr(new Int32(2, Span(2, 11)))
                    ),
                    Span(2, 6, 6)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case Int(y) || String(y):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchOptions(
                    vec(
                        NodePtr(new MatchObject(
                            NodePtr(new ID("Int", Span(2, 6, 3))),
                            NodePtr(new MatchArgs(
                                vec(NodePtr(new ID("y", Span(2, 10)))),
                                {},
                                Span(2, 9, 3)
                            )),
                            Span(2, 9)
                        )),
                        NodePtr(new MatchObject(
                            NodePtr(new ID("String", Span(2, 16, 6))),
                            NodePtr(new MatchArgs(
                                vec(NodePtr(new ID("y", Span(2, 23)))),
                                {},
                                Span(2, 22, 3)
                            )),
                            Span(2, 22)
                        ))
                    ),
                    Span(2, 6, 2, 24)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case 1 || 2 as y:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new As(
                    NodePtr(new MatchOptions(
                        vec(
                            NodePtr(new Int32(1, Span(2, 6))),
                            NodePtr(new Int32(2, Span(2, 11)))
                        ),
                        Span(2, 6, 6)
                    )),
                    NodePtr(new ID("y", Span(2, 16))),
                    Span(2, 13, 2)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (3 as *args):\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new As(
                        NodePtr(new Int32(3, Span(2, 7))),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(2, 13, 4))),
                            Span(2, 12)
                        )),
                        Span(2, 9, 2)
                    ))),
                    Span(2, 6, 2, 17)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (*args: Int):\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", Span(2, 8, 4))),
                            Span(2, 7)
                        )),
                        NodePtr(new ID("Int", Span(2, 14, 3))),
                        Span(2, 12)
                    ))),
                    Span(2, 6, 2, 17)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3 as **kwargs}:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new As(
                        NodePtr(new Int32(3, Span(2, 7))),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", Span(2, 14, 6))),
                            Span(2, 12, 2)
                        )),
                        Span(2, 9, 2)
                    ))),
                    Span(2, 6, 2, 20)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case |f(y):\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 7))),
                    NodePtr(new Args(
                        vec(NodePtr(new ID("y", Span(2, 9)))),
                        {},
                        Span(2, 8, 3)
                    )),
                    Span(2, 8)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x"
        ).node() == ErrorWithComp(
            ErrPtr(new NoMatchCasesErr()),
            Comp(
                OpID::MATCH,
                Comp(OpID::ID, "x", Span(1, 7)),
                Span(1, 1, 5)
            ),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case *args:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedMatchExprErr()),
                    Comp(
                        OpID::UNPACK_ARGS,
                        Comp(OpID::ID, "args", Span(2, 7, 4)),
                        Span(2, 6)
                    )
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case **kwargs:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedMatchExprErr()),
                    Comp(
                        OpID::UNPACK_KWARGS,
                        Comp(OpID::ID, "kwargs", Span(2, 8, 6)),
                        Span(2, 6, 2)
                    )
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [**kwargs]:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedElementMatchExprErr()),
                        Comp(
                            OpID::UNPACK_KWARGS,
                            Comp(OpID::ID, "kwargs", Span(2, 9, 6)),
                            Span(2, 7, 2)
                        )
                    ))),
                    Span(2, 6, 2, 15)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {*args}:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedEntryMatchExprErr()),
                        Comp(
                            OpID::UNPACK_ARGS,
                            Comp(OpID::ID, "args", Span(2, 8, 4)),
                            Span(2, 7)
                        )
                    ))),
                    Span(2, 6, 2, 12)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [y=2]:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedElementMatchExprErr()),
                        Comp(
                            OpID::BIND,
                            Comp(OpID::ID, "y", Span(2, 7)),
                            Comp(OpID::PLAIN_INT, "2", Span(2, 9)),
                            Span(2, 8)
                        )
                    ))),
                    Span(2, 6, 5)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y as z as w:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new As(
                    NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedAsLHSExprErr()),
                        Comp(
                            OpID::AS,
                            Comp(OpID::ID, "y", Span(2, 6)),
                            Comp(OpID::ID, "z", Span(2, 11)),
                            Span(2, 8, 2)
                        )
                    )),
                    NodePtr(new ID("w", Span(2, 16))),
                    Span(2, 13, 2)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y: Int as z:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new As(
                    NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedAsLHSExprErr()),
                        Comp(
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "y", Span(2, 6)),
                            Comp(OpID::ID, "Int", Span(2, 9, 3)),
                            Span(2, 7)
                        )
                    )),
                    NodePtr(new ID("z", Span(2, 16))),
                    Span(2, 13, 2)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [*args as y]:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new As(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedAsLHSExprErr()),
                            Comp(
                                OpID::UNPACK_ARGS,
                                Comp(OpID::ID, "args", Span(2, 8, 4)),
                                Span(2, 7)
                            )
                        )),
                        NodePtr(new ID("y", Span(2, 16))),
                        Span(2, 13, 2)
                    ))),
                    Span(2, 6, 2, 17)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case T(**kwargs as y):\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", Span(2, 6))),
                    NodePtr(new MatchArgs(
                        vec(NodePtr(new As(
                            NodePtr(new ErrorWithComp(
                                ErrPtr(new ExpectedAsLHSExprErr()),
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::ID, "kwargs", Span(2, 10, 6)),
                                    Span(2, 8, 2)
                                )
                            )),
                            NodePtr(new ID("y", Span(2, 20))),
                            Span(2, 17, 2)
                        ))),
                        {},
                        Span(2, 7, 2, 21)
                    )),
                    Span(2, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (y: Int):\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedExprErr()),
                    Comp(
                        OpID::TYPE_LABEL,
                        Comp(OpID::ID, "y", Span(2, 7)),
                        Comp(OpID::ID, "Int", Span(2, 10, 3)),
                        Span(2, 8)
                    )
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case (3 as y):\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedExprErr()),
                    Comp(
                        OpID::AS,
                        Comp(OpID::PLAIN_INT, "3", Span(2, 7)),
                        Comp(OpID::ID, "y", Span(2, 12)),
                        Span(2, 9, 2)
                    )
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {3 as y}:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new ErrorWithComp(
                        ErrPtr(new ExpectedEntryMatchExprErr()),
                        Comp(
                            OpID::AS,
                            Comp(OpID::PLAIN_INT, "3", Span(2, 7)),
                            Comp(OpID::ID, "y", Span(2, 12)),
                            Span(2, 9, 2)
                        )
                    ))),
                    Span(2, 6, 2, 13)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [*3: Int]:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedIDErr()),
                            Comp(OpID::PLAIN_INT, "3", Span(2, 8))
                        )),
                        NodePtr(new ID("Int", Span(2, 11, 3))),
                        Span(2, 9)
                    ))),
                    Span(2, 6, 2, 14)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case [y as *3]:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchList(
                    vec(NodePtr(new As(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedIDErr()),
                            Comp(OpID::PLAIN_INT, "3", Span(2, 13))
                        )),
                        Span(2, 9, 2)
                    ))),
                    Span(2, 6, 2, 14)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {y as **3}:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new As(
                        NodePtr(new ID("y", Span(2, 7))),
                        NodePtr(new ErrorWithComp(
                            ErrPtr(new ExpectedIDErr()),
                            Comp(OpID::PLAIN_INT, "3", Span(2, 14))
                        )),
                        Span(2, 9, 2)
                    ))),
                    Span(2, 6, 2, 15)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingPredicateErr()),
                    Span(2, 5)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ID("y", Span(2, 6))),
                nullptr,
                NodePtr(new ErrorNode(
                    ErrPtr(new MissingBodyErr()),
                    Span(2, 7)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "elif y:\n"
            "    return y"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedCaseErr()),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", Span(2, 6)),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", Span(3, 12)),
                                Span(3, 5, 6)
                            )),
                            Span(3, 5, 3, 12)
                        ),
                        Span(2, 7)
                    ),
                    Span(2, 1, 4)
                ),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case break:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new ErrorNode(
                    ErrPtr(new ExpectedValueErr()),
                    Span(2, 6, 5)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "match x\n"
            "case {break}:\n"
            "    return 0"
        ).node() == Match(
            NodePtr(new ID("x", Span(1, 7))),
            vec(NodePtr(new MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new ErrorNode(
                        ErrPtr(new ExpectedValueErr()),
                        Span(2, 7, 5)
                    ))),
                    Span(2, 6, 7)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, Span(3, 12))),
                        Span(3, 5, 6)
                    ))),
                    Span(3, 5, 3, 12)
                )),
                Span(2, 1, 4)
            ))),
            Span(1, 1, 5)
        ));
    }

    SECTION("NEG") {
        REQUIRE(*capture(
            "-a"
        ).node() == Negate(
            NodePtr(new ID("a", Span(1, 2))),
            Span(1, 1)
        ));
    }

    SECTION("NEQ") {
        REQUIRE(*capture(
            "a != b"
        ).node() == NotEquals(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("None") {
        REQUIRE(*capture(
            "none"
        ).node() == None(Span(1, 1, 4)));
    }

    SECTION("Null") {
        REQUIRE(*capture(
            "null"
        ).node() == Null(Span(1, 1, 4)));
    }

    SECTION("Or") {
        REQUIRE(*capture(
            "a or b"
        ).node() == Or(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("POW") {
        REQUIRE(*capture(
            "a ** b"
        ).node() == Power(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("RSH") {
        REQUIRE(*capture(
            "a >> b"
        ).node() == RightShift(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 6))),
            Span(1, 3, 2)
        ));
    }

    SECTION("Raise") {
        REQUIRE(*capture(
            "raise"
        ).node() == Raise(
            nullptr,
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "raise a"
        ).node() == Raise(
            NodePtr(new ID("a", Span(1, 7))),
            Span(1, 1, 5)
        ));
    }

    SECTION("Return") {
        REQUIRE(*capture(
            "return"
        ).node() == Return(
            nullptr,
            Span(1, 1, 6)
        ));

        REQUIRE(*capture(
            "return a"
        ).node() == Return(
            NodePtr(new ID("a", Span(1, 8))),
            Span(1, 1, 6)
        ));
    }

    SECTION("SUB") {
        REQUIRE(*capture(
            "a - b"
        ).node() == Subtract(
            NodePtr(new ID("a", Span(1, 1))),
            NodePtr(new ID("b", Span(1, 5))),
            Span(1, 3)
        ));
    }

    SECTION("Set") {
        REQUIRE(*capture(
            "{a}"
        ).node() == Set(
            vec(NodePtr(new ID("a", Span(1, 2)))),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "{a, b, c}"
        ).node() == Set(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new ID("b", Span(1, 5))),
                NodePtr(new ID("c", Span(1, 8)))
            ),
            Span(1, 1, 9)
        ));

        REQUIRE(*capture(
            "{a, *args, b}"
        ).node() == Set(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", Span(1, 6, 4))),
                    Span(1, 5)
                )),
                NodePtr(new ID("b", Span(1, 12)))
            ),
            Span(1, 1, 13)
        ));

        REQUIRE(*capture(
            "{a,}"
        ).node() == Set(
            vec(NodePtr(new ID("a", Span(1, 2)))),
            Span(1, 1, 4)
        ));
    }

    SECTION("Slice") {
        REQUIRE(*capture(
            "(:)"
        ).node() == Slice(
            nullptr,
            nullptr,
            nullptr,
            Span(1, 2)
        ));

        REQUIRE(*capture(
            "(:5)"
        ).node() == Slice(
            nullptr,
            NodePtr(new Int32(5, Span(1, 3))),
            nullptr,
            Span(1, 2, 2)
        ));

        REQUIRE(*capture(
            "(3:)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            nullptr,
            nullptr,
            Span(1, 2, 2)
        ));

        REQUIRE(*capture(
            "(3:5)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            NodePtr(new Int32(5, Span(1, 4))),
            nullptr,
            Span(1, 2, 3)
        ));

        REQUIRE(*capture(
            "(::)"
        ).node() == Slice(
            nullptr,
            nullptr,
            nullptr,
            Span(1, 2, 2)
        ));

        REQUIRE(*capture(
            "(::2)"
        ).node() == Slice(
            nullptr,
            nullptr,
            NodePtr(new Int32(2, Span(1, 4))),
            Span(1, 2, 3)
        ));

        REQUIRE(*capture(
            "(:5:)"
        ).node() == Slice(
            nullptr,
            NodePtr(new Int32(5, Span(1, 3))),
            nullptr,
            Span(1, 2, 3)
        ));

        REQUIRE(*capture(
            "(3::)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            nullptr,
            nullptr,
            Span(1, 2, 3)
        ));

        REQUIRE(*capture(
            "(:5:2)"
        ).node() == Slice(
            nullptr,
            NodePtr(new Int32(5, Span(1, 3))),
            NodePtr(new Int32(2, Span(1, 5))),
            Span(1, 2, 4)
        ));

        REQUIRE(*capture(
            "(3::2)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            nullptr,
            NodePtr(new Int32(2, Span(1, 5))),
            Span(1, 2, 4)
        ));

        REQUIRE(*capture(
            "(3:5:)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            NodePtr(new Int32(5, Span(1, 4))),
            nullptr,
            Span(1, 2, 4)
        ));

        REQUIRE(*capture(
            "(3:5:2)"
        ).node() == Slice(
            NodePtr(new Int32(3, Span(1, 2))),
            NodePtr(new Int32(5, Span(1, 4))),
            NodePtr(new Int32(2, Span(1, 6))),
            Span(1, 2, 5)
        ));

        REQUIRE(*capture(
            "(1:2:3:4)"
        ).node() == ErrorWithComp(
            ErrPtr(new TooManySliceComponentsErr()),
            Comp(
                OpID::TYPE_LABEL,
                Comp(
                    OpID::TYPE_LABEL,
                    Comp(
                        OpID::TYPE_LABEL,
                        Comp(OpID::PLAIN_INT, "1", Span(1, 2)),
                        Comp(OpID::PLAIN_INT, "2", Span(1, 4)),
                        Span(1, 3)
                    ),
                    Comp(OpID::PLAIN_INT, "3", Span(1, 6)),
                    Span(1, 5)
                ),
                Comp(OpID::PLAIN_INT, "4", Span(1, 8)),
                Span(1, 7)
            )
        ));
    }

    SECTION("Symbol") {
        REQUIRE(*capture(
            "`x"
        ).node() == Symbol(
            NodePtr(new ID("x", Span(1, 2))),
            Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "`3"
        ).node() == Symbol(
            NodePtr(new NumID(3, Span(1, 2))),
            Span(1, 1, 2)
        ));
    }

    SECTION("Ternary") {
        REQUIRE(*capture(
            "x if y else z"
        ).node() == Ternary(
            NodePtr(new ID("y", Span(1, 6))),
            NodePtr(new ID("x", Span(1, 1))),
            NodePtr(new ID("z", Span(1, 13))),
            Span(1, 3, 2)
        ));

        REQUIRE(*capture(
            "x if y"
        ).node() == ErrorWithComp(
            ErrPtr(new ExpectedExprErr()),
            Comp(
                OpID::TERNARY_IF,
                Comp(OpID::ID, "x", Span(1, 1)),
                Comp(OpID::ID, "y", Span(1, 6)),
                Span(1, 3, 2)
            )
        ));

        REQUIRE(*capture(
            "x else y"
        ).node() == ErrorWithComp(
            ErrPtr(new ExpectedExprErr()),
            Comp(
                OpID::TERNARY_ELSE,
                Comp(OpID::ID, "x", Span(1, 1)),
                Comp(OpID::ID, "y", Span(1, 8)),
                Span(1, 3, 4)
            )
        ));
    }

    SECTION("This") {
        REQUIRE(*capture(
            "this"
        ).node() == This(Span(1, 1, 4)));
    }

    SECTION("Try") {
        REQUIRE(*capture(
            "try:\n"
            "    f()\n"
            "except e: E:\n"
            "    g()"
        ).node() == Try(
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 5))),
                    NodePtr(new Args({}, {}, Span(2, 6, 2))),
                    Span(2, 6)
                ))),
                Span(1, 1, 3)
            )),
            vec(NodePtr(new MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("e", Span(3, 8))),
                    NodePtr(new ID("E", Span(3, 11))),
                    Span(3, 9)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Call(
                        NodePtr(new ID("g", Span(4, 5))),
                        NodePtr(new Args({}, {}, Span(4, 6, 2))),
                        Span(4, 6)
                    ))),
                    Span(4, 5, 3)
                )),
                Span(3, 1, 6)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "try:\n"
            "    f()\n"
            "except e: E:\n"
            "    g()\n"
            "finally:\n"
            "    h()"
        ).node() == Try(
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 5))),
                    NodePtr(new Args({}, {}, Span(2, 6, 2))),
                    Span(2, 6)
                ))),
                Span(1, 1, 3)
            )),
            vec(NodePtr(new MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("e", Span(3, 8))),
                    NodePtr(new ID("E", Span(3, 11))),
                    Span(3, 9)
                )),
                nullptr,
                NodePtr(new Block(
                    vec(NodePtr(new Call(
                        NodePtr(new ID("g", Span(4, 5))),
                        NodePtr(new Args({}, {}, Span(4, 6, 2))),
                        Span(4, 6)
                    ))),
                    Span(4, 5, 3)
                )),
                Span(3, 1, 6)
            ))),
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("h", Span(6, 5))),
                    NodePtr(new Args({}, {}, Span(6, 6, 2))),
                    Span(6, 6)
                ))),
                Span(5, 1, 7)
            )),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "try:\n"
            "    f()\n"
            "finally:\n"
            "    g()"
        ).node() == Try(
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 5))),
                    NodePtr(new Args({}, {}, Span(2, 6, 2))),
                    Span(2, 6)
                ))),
                Span(1, 1, 3)
            )),
            {},
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("g", Span(4, 5))),
                    NodePtr(new Args({}, {}, Span(4, 6, 2))),
                    Span(4, 6)
                ))),
                Span(3, 1, 7)
            )),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "try:\n"
            "    f()\n"
            "except e: E1:\n"
            "    g()\n"
            "except e: E2:\n"
            "    h()"
        ).node() == Try(
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 5))),
                    NodePtr(new Args({}, {}, Span(2, 6, 2))),
                    Span(2, 6)
                ))),
                Span(1, 1, 3)
            )),
            vec(
                NodePtr(new MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("e", Span(3, 8))),
                        NodePtr(new ID("E1", Span(3, 11, 2))),
                        Span(3, 9)
                    )),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Call(
                            NodePtr(new ID("g", Span(4, 5))),
                            NodePtr(new Args({}, {}, Span(4, 6, 2))),
                            Span(4, 6)
                        ))),
                        Span(4, 5, 3)
                    )),
                    Span(3, 1, 6)
                )),
                NodePtr(new MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("e", Span(5, 8))),
                        NodePtr(new ID("E2", Span(5, 11, 2))),
                        Span(5, 9)
                    )),
                    nullptr,
                    NodePtr(new Block(
                        vec(NodePtr(new Call(
                            NodePtr(new ID("h", Span(6, 5))),
                            NodePtr(new Args({}, {}, Span(6, 6, 2))),
                            Span(6, 6)
                        ))),
                        Span(6, 5, 3)
                    )),
                    Span(5, 1, 6)
                ))
            ),
            nullptr,
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "try:\n"
            "    f()"
        ).node() == ErrorWithComp(
            ErrPtr(new NoExceptsOrFinallyErr()),
            Comp(
                OpID::TRY,
                Comp(
                    OpID::BODY,
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "f", Span(2, 5)),
                            Comp(
                                OpID::GROUP,
                                Comp(OpID::NOTHING, Span(2, 7)),
                                Span(2, 6, 2)
                            ),
                            Span(2, 6)
                        )),
                        Span(2, 5, 3)
                    ),
                    Span(1, 4)
                ),
                Span(1, 1, 3)
            ),
            Span(1, 1, 3)
        ));

        REQUIRE(*capture(
            "try:\n"
            "    f()\n"
            "elif e: E1:\n"
            "    g()"
        ).node() == Try(
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("f", Span(2, 5))),
                    NodePtr(new Args({}, {}, Span(2, 6, 2))),
                    Span(2, 6)
                ))),
                Span(1, 1, 3)
            )),
            vec(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedExceptOrFinallyErr()),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", Span(3, 6)),
                            Comp(OpID::ID, "E1", Span(3, 9, 2)),
                            Span(3, 7)
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", Span(4, 5)),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, Span(4, 7)),
                                    Span(4, 6, 2)
                                ),
                                Span(4, 6)
                            )),
                            Span(4, 5, 3)
                        ),
                        Span(3, 11)
                    ),
                    Span(3, 1, 4)
                ),
                Span(3, 1, 4)
            ))),
            nullptr,
            Span(1, 1, 3)
        ));
    }

    SECTION("Tuple") {
        REQUIRE(*capture(
            "()"
        ).node() == Tuple(
            {}, Span(1, 1, 2)
        ));

        REQUIRE(*capture(
            "(a, b, c)"
        ).node() == Tuple(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new ID("b", Span(1, 5))),
                NodePtr(new ID("c", Span(1, 8)))
            ),
            Span(1, 1, 9)
        ));

        REQUIRE(*capture(
            "a, b, c"
        ).node() == Tuple(
            vec(
                NodePtr(new ID("a", Span(1, 1))),
                NodePtr(new ID("b", Span(1, 4))),
                NodePtr(new ID("c", Span(1, 7)))
            ),
            Span(1, 1, 7)
        ));

        REQUIRE(*capture(
            "(a, *args, b)"
        ).node() == Tuple(
            vec(
                NodePtr(new ID("a", Span(1, 2))),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", Span(1, 6, 4))),
                    Span(1, 5)
                )),
                NodePtr(new ID("b", Span(1, 12)))
            ),
            Span(1, 1, 13)
        ));

        REQUIRE(*capture(
            "a, *args, b"
        ).node() == Tuple(
            vec(
                NodePtr(new ID("a", Span(1, 1))),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", Span(1, 5, 4))),
                    Span(1, 4)
                )),
                NodePtr(new ID("b", Span(1, 11)))
            ),
            Span(1, 1, 11)
        ));

        REQUIRE(*capture(
            "(*args)"
        ).node() == Tuple(
            vec(NodePtr(new Expansion(
                NodePtr(new ID("args", Span(1, 3, 4))),
                Span(1, 2)
            ))),
            Span(1, 1, 7)
        ));

        REQUIRE(*capture(
            "(a,)"
        ).node() == Tuple(
            vec(NodePtr(new ID("a", Span(1, 2)))),
            Span(1, 1, 4)
        ));
    }

    SECTION("Var") {
        REQUIRE(*capture(
            ".1"
        ).node() == Var(
            1,
            NodePtr(new NumID(1, Span(1, 2))),
            Span(1, 1)
        ));

        REQUIRE(*capture(
            ".a"
        ).node() == Var(
            1,
            NodePtr(new ID("a", Span(1, 2))),
            Span(1, 1)
        ));

        REQUIRE(*capture(
            "...a"
        ).node() == Var(
            3,
            NodePtr(new ID("a", Span(1, 4))),
            Span(1, 1, 3)
        ));
    }

    SECTION("While") {
        REQUIRE(*capture(
            "while a:\n"
            "    f()"
        ).node() == While(
            NodePtr(new Case(
                NodePtr(new ID("a", Span(1, 7))),
                NodePtr(new Block(
                    vec(NodePtr(new Call(
                        NodePtr(new ID("f", Span(2, 5))),
                        NodePtr(new Args({}, {}, Span(2, 6, 2))),
                        Span(2, 6)
                    ))),
                    Span(2, 5, 3)
                )),
                Span(1, 1, 5)
            )),
            nullptr,
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "while a:\n"
            "    f()\n"
            "else:\n"
            "    g()"
        ).node() == While(
            NodePtr(new Case(
                NodePtr(new ID("a", Span(1, 7))),
                NodePtr(new Block(
                    vec(NodePtr(new Call(
                        NodePtr(new ID("f", Span(2, 5))),
                        NodePtr(new Args({}, {}, Span(2, 6, 2))),
                        Span(2, 6)
                    ))),
                    Span(2, 5, 3)
                )),
                Span(1, 1, 5)
            )),
            NodePtr(new Block(
                vec(NodePtr(new Call(
                    NodePtr(new ID("g", Span(4, 5))),
                    NodePtr(new Args({}, {}, Span(4, 6, 2))),
                    Span(4, 6)
                ))),
                Span(3, 1, 4)
            )),
            Span(1, 1, 5)
        ));

        REQUIRE(*capture(
            "while a:\n"
            "    f()\n"
            "elif b:\n"
            "    g()\n"
            "else:\n"
            "    h()"
        ).node() == While(
            NodePtr(new Case(
                NodePtr(new ID("a", Span(1, 7))),
                NodePtr(new Block(
                    vec(NodePtr(new Call(
                        NodePtr(new ID("f", Span(2, 5))),
                        NodePtr(new Args({}, {}, Span(2, 6, 2))),
                        Span(2, 6)
                    ))),
                    Span(2, 5, 3)
                )),
                Span(1, 1, 5)
            )),
            NodePtr(new ErrorWithComp(
                ErrPtr(new UnexpectedBlocksErr()),
                Comp(
                    OpID::BLOCKS,
                    vec(
                        Comp(
                            OpID::ELIF,
                            Comp(
                                OpID::LABEL,
                                Comp(OpID::ID, "b", Span(3, 6)),
                                Comp(
                                    OpID::BLOCK,
                                    vec(Comp(
                                        OpID::CALL,
                                        Comp(OpID::ID, "g", Span(4, 5)),
                                        Comp(
                                            OpID::GROUP,
                                            Comp(OpID::NOTHING, Span(4, 7)),
                                            Span(4, 6, 2)
                                        ),
                                        Span(4, 6)
                                    )),
                                    Span(4, 5, 3)
                                ),
                                Span(3, 7)
                            ),
                            Span(3, 1, 4)
                        ),
                        Comp(
                            OpID::ELSE,
                            Comp(
                                OpID::BODY,
                                Comp(
                                    OpID::BLOCK,
                                    vec(Comp(
                                        OpID::CALL,
                                        Comp(OpID::ID, "h", Span(6, 5)),
                                        Comp(
                                            OpID::GROUP,
                                            Comp(OpID::NOTHING, Span(6, 7)),
                                            Span(6, 6, 2)
                                        ),
                                        Span(6, 6)
                                    )),
                                    Span(6, 5, 3)
                                ),
                                Span(5, 5)
                            ),
                            Span(5, 1, 4)
                        )
                    ),
                    Span(3, 1, 6, 7)
                )
            )),
            Span(1, 1, 5)
        ));
    }

    SECTION("Unexpected Block") {
        REQUIRE(*capture(
            ":\n"
            "    true"
        ).node() == UnexpectedBlock(
            vec(NodePtr(new Bool(true, Span(2, 5, 4)))),
            Span(1, 1)
        ));
    }

    SECTION("Unexpected Construct") {
        REQUIRE(*capture(
            "elif true"
        ).node() == UnexpectedConstruct(
            OpID::ELIF,
            NodePtr(new Bool(true, Span(1, 6, 4))),
            Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "elif:\n"
            "    false"
        ).node() == UnexpectedConstruct(
            OpID::ELIF,
            NodePtr(new Block(
                vec(NodePtr(new Bool(false, Span(2, 5, 5)))),
                Span(2, 5, 5)
            )),
            Span(1, 1, 4)
        ));

        REQUIRE(*capture(
            "elif true:\n"
            "    false"
        ).node() == UnexpectedConstruct(
            OpID::ELIF,
            NodePtr(new Case(
                NodePtr(new Bool(true, Span(1, 6, 4))),
                NodePtr(new Block(
                    vec(NodePtr(new Bool(false, Span(2, 5, 5)))),
                    Span(2, 5, 5)
                )),
                Span(1, 1, 4)
            )),
            Span(1, 1, 4)
        ));
    }
}
