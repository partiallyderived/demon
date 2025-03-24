#include <catch2/catch_test_macros.hpp>

#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/interpret/interpreterimpl.hpp"
#include "dl/parse/opid.hpp"

#include "util.hpp"

using namespace dl;

TEST_CASE("interpret", "[interpret]") {
    auto interpreter = InterpreterImpl();

    SECTION("ADD") {
        // a + b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ADD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__add__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Addr") {
        // @a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ADDR, Comp(OpID::ID, "a", s2), s1
        )) == Addr(
            NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("AddrType") {
        // Int@
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ADDR_TYPE, Comp(OpID::ID, "Int", s1), s2
        )) == AddrType(
            NodePtr(new ID("Int", s1)), s2
        ));
    }

    SECTION("And") {
        // true and false
        REQUIRE(*interpreter.interpret(Comp(
            OpID::AND, Comp(OpID::TRUE, s1), Comp(OpID::FALSE, s3), s2
        )) == And(
            NodePtr(new Bool(true, s1)),
            NodePtr(new Bool(false, s3)),
            s2
        ));
    }

    SECTION("Assign") {
        // a = b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET, Comp(OpID::ID, "a", s1), Comp(OpID::ID, "b", s3), s2
        )) == Assign(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("b", s3)),
            s2
        ));
    }

    SECTION("BAND") {
        // a & b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BAND,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__band__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("BNOT") {
        // ~a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BNOT,
            Comp(OpID::ID, "a", s2),
            s1
        )) == CallAttr(
            NodePtr(new ID("a", s2)),
            NodePtr(new ID("__bnot__", s1)),
            Args({}, {}, s1),
            s1
        ));
    }

    SECTION("BOR") {
        // a | b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__bor__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("BXOR") {
        // a ^ b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BXOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__bxor__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Break") {
        // break
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BREAK, s1
        )) == Break(s1));
    }

    SECTION("Cached Call") {
        // Vector[Int]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CALL,
            Comp(OpID::ID, "Vector", s1),
            Comp(OpID::LIST, Comp(OpID::ID, "Int", s3), s2),
            s2
        )) == CachedCall(
            NodePtr(new ID("Vector", s1)),
            Args(vec(NodePtr(new ID("Int", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Cached CallAttr") {
        // obj.attr[T]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CALL,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "obj", s1),
                Comp(OpID::ID, "attr", s3),
                s2
            ),
            Comp(OpID::LIST, Comp(OpID::ID, "T", s5), s4),
            s4
        )) == CachedCallAttr(
            NodePtr(new ID("obj", s1)),
            NodePtr(new ID("attr", s3)),
            Args(vec(NodePtr(new ID("T", s5))), {}, s4),
            s4
        ));
    }

    SECTION("Call") {
        // fn()
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CALL,
            Comp(OpID::ID, "fn", s1),
            Comp(OpID::GROUP, Comp(OpID::NOTHING, s3), s2),
            s2
        )) == Call(
            NodePtr(new ID("fn", s1)),
            Args({}, {}, s2),
            s2
        ));

        // fn(1, 2)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Call(
            NodePtr(new ID("fn", s1)),
            Args(
                vec(NodePtr(new Int32(1, s3)), NodePtr(new Int32(2, s5))),
                {},
                s2
            ),
            s2
        ));

        // fn(1, 2, *args)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Call(
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Expansion(
                        NodePtr(new ID("args", s8)), s7
                    ))
                ),
                {},
                s2
            ),
            s2
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Call(
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Expansion(NodePtr(new ID("args", s8)), s7))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        ID("kw1", s10), NodePtr(new String("yes", s12)), s11
                    )),
                    NodePtr(new KeywordArg{
                        ID("kw2", s14), NodePtr(new Bool(true, s16)), s15
                    })
                ),
                s2
            ),
            s2
        ));

        // fn(1, 2, *args, kw1="yes", kw2=true, **kwargs)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Call(
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Expansion(NodePtr(new ID("args", s8)), s7))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        ID("kw1", s10), NodePtr(new String("yes", s12)), s11
                    )),
                    NodePtr(new KeywordArg(
                        ID("kw2", s14), NodePtr(new Bool(true, s16)), s15
                    )),
                    NodePtr(new Expansion(NodePtr(new ID("kwargs", s19)), s18))
                ),
                s2
            ),
            s2
        ));

        // fn(1, kw1="yes", 2)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == PosAfterKeywordErr(s9));
    }

    SECTION("CallAttr") {
        // thing.do(arg1, arg2, setting=true)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == CallAttr(
            NodePtr(new ID("thing", s1)),
            NodePtr(new ID("do", s3)),
            Args(
                vec(NodePtr(new ID("arg1", s5)), NodePtr(new ID("arg2", s7))),
                vec(
                    NodePtr(new KeywordArg(
                        ID("setting", s9), NodePtr(new Bool(true, s11)), s10
                    ))
                ),
                s4
            ),
            s4
        ));

        // a.3
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new NumID(3, s3)),
            Args({}, {}, s4),
            s4
        ));

        // a.true()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedGeneralIDErr(s3));
    }

    SECTION("Continue") {
        // continue
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONTINUE, s1
        )) == Continue(s1));
    }

    SECTION("Declare") {
        // a: Int
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "Int", s3),
            s2
        )) == Declare(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("Int", s3)),
            s2
        ));

        // 3: Int
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::PLAIN_INT, "3", s1),
            Comp(OpID::ID, "Int", s3),
            s2
        )) == Declare(
            NodePtr(new NumID(3, s1)),
            NodePtr(new ID("Int", s3)),
            s2
        ));

        // true: Int
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::TRUE, s1),
            Comp(OpID::ID, "Int", s3),
            s2
        )) == ExpectedGeneralIDErr(s1));
    }

    SECTION("Def") {
        // def f():
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs({}, {}, s3),
                nullptr,
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s9)), s8
                ))),
                s1
            )),
            s1
        ));

        // def f() -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs({}, {}, s3),
                nullptr,
                NodePtr(new ID("Int", s6)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s11)), s10
                ))),
                s1
            )),
            s1
        ));

        // def f(arg) -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(NodePtr(new ID("arg", s4))),
                    {},
                    s3
                ),
                nullptr,
                NodePtr(new ID("Int", s7)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s12)), s11
                ))),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int) -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", s4)),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", s6)),
                            NodePtr(new ID("Int", s8)),
                            s7
                        ))
                    ),
                    {},
                    s3
                ),
                nullptr,
                NodePtr(new ID("Int", s11)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s16)), s15
                ))),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int, *args) -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", s4)),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", s6)),
                            NodePtr(new ID("Int", s8)),
                            s7
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s11)), s10
                        ))
                    ),
                    {},
                    s3
                ),
                nullptr,
                NodePtr(new ID("Int", s14)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s19)), s18
                ))),
                s1
            )),
            s1
        ));

        // def f(arg1, arg2: Int, *args, *, kw1=1, kw2: Bool) -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                        OpID::POS_KW_SEP,
                                        s13
                                    ),
                                    Comp(
                                        OpID::BIND,
                                        Comp(OpID::ID, "kw1", s15),
                                        Comp(OpID::PLAIN_INT, "1", s17),
                                        s16
                                    ),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "kw2", s19),
                                        Comp(OpID::ID, "Bool", s21),
                                        s20
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s24),
                        s23
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s29),
                            s28
                        )),
                        s27
                    ),
                    s25
                ),
                s1
            )),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", s4)),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", s6)),
                            NodePtr(new ID("Int", s8)),
                            s7
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s11)), s10
                        ))
                    ),
                    vec(
                        NodePtr(new Defaulted(
                            NodePtr(new ID("kw1", s15)),
                            NodePtr(new Int32(1, s17)),
                            s16
                        )),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("kw2", s19)),
                            NodePtr(new ID("Bool", s21)),
                            s20
                        ))
                    ),
                    s3
                ),
                nullptr,
                NodePtr(new ID("Int", s24)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s29)), s28
                ))),
                s1
            )),
            s1
        ));

        // def f(
        //     arg1, arg2: Int, *args, *, kw1=1, kw2: Bool, **kwargs
        // ) -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                        OpID::POS_KW_SEP,
                                        s13
                                    ),
                                    Comp(
                                        OpID::BIND,
                                        Comp(OpID::ID, "kw1", s15),
                                        Comp(OpID::PLAIN_INT, "1", s17),
                                        s16
                                    ),
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "kw2", s19),
                                        Comp(OpID::ID, "Bool", s21),
                                        s20
                                    ),
                                    Comp(
                                        OpID::UNPACK_KWARGS,
                                        Comp(OpID::ID, "kwargs", s24),
                                        s23
                                    )
                                ),
                                s3
                            ),
                            s3
                        ),
                        Comp(OpID::ID, "Int", s27),
                        s26
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::RETURN,
                            Comp(OpID::PLAIN_INT, "0", s32),
                            s31
                        )),
                        s30
                    ),
                    s28
                ),
                s1
            )),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(
                        NodePtr(new ID("arg1", s4)),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("arg2", s6)),
                            NodePtr(new ID("Int", s8)),
                            s7
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s11)), s10
                        ))
                    ),
                    vec(
                        NodePtr(new Defaulted(
                            NodePtr(new ID("kw1", s15)),
                            NodePtr(new Int32(1, s17)),
                            s16
                        )),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("kw2", s19)),
                            NodePtr(new ID("Bool", s21)),
                            s20
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", s24)), s23
                        ))
                    ),
                    s3
                ),
                nullptr,
                NodePtr(new ID("Int", s27)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s32)), s31
                ))),
                s1
            )),
            s1
        ));

        // def f(a: Int, b: String):
        //     return 0
        // case (c: Float32, true) -> Int64:
        //     return 3s64
        // case (1, *, 2 as kw):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                    OpID::AS,
                                    Comp(OpID::PLAIN_INT, "2", s40),
                                    Comp(OpID::ID, "kw", s42),
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(
                DefCase(
                    MatchArgs(
                        vec(
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("a", s4)),
                                NodePtr(new ID("Int", s6)),
                                s5
                            )),
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("b", s8)),
                                NodePtr(new ID("String", s10)),
                                s9
                            ))
                        ),
                        {},
                        s3
                    ),
                    nullptr,
                    nullptr,
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s16)), s15
                    ))),
                    s1
                ),
                DefCase(
                    MatchArgs(
                        vec(
                            NodePtr(new TypeMatch(
                                NodePtr(new ID("c", s20)),
                                NodePtr(new ID("Float32", s22)),
                                s21
                            )),
                            NodePtr(new Bool(true, s24))
                        ),
                        {},
                        s19
                    ),
                    nullptr,
                    NodePtr(new ID("Int64", s27)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int64(3, s32)), s31
                    ))),
                    s18
                ),
                DefCase(
                    MatchArgs(
                        vec(NodePtr(new Int32(1, s36))),
                        vec(NodePtr(new As(
                            NodePtr(new Int32(2, s40)),
                            NodePtr(new ID("kw", s42)),
                            s41
                        ))),
                        s35
                    ),
                    nullptr,
                    nullptr,
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s48)), s47
                    ))),
                    s34
                )
            ),
            s1
        ));

        // def 3():
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Def(
            NodePtr(new NumID(3, s2)),
            vec(DefCase(
                MatchArgs({}, {}, s3),
                nullptr,
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s9)), s8
                ))),
                s1
            )),
            s1
        ));

        // def f(x: Int = 1):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                    OpID::TYPE_LABEL,
                                    Comp(OpID::ID, "x", s4),
                                    Comp(OpID::ID, "Int", s6),
                                    s5
                                ),
                                Comp(OpID::PLAIN_INT, "1", s8),
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("x", s4)),
                            NodePtr(new ID("Int", s6)),
                            s5
                        )),
                        NodePtr(new Int32(1, s8)),
                        s7
                    ))),
                    {},
                    s3
                ),
                nullptr,
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s14)), s13
                ))),
                s1
            )),
            s1
        ));

        // def f(x: Int) if x > 2 -> Int:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::DEF,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::RETURNS,
                        Comp(
                            OpID::TERNARY_IF,
                            Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s2),
                                Comp(
                                    OpID::GROUP,
                                    Comp(
                                        OpID::TYPE_LABEL,
                                        Comp(OpID::ID, "x", s4),
                                        Comp(OpID::ID, "Int", s6),
                                        s5
                                    ),
                                    s3
                                ),
                                s3
                            ),
                            Comp(
                                OpID::GT,
                                Comp(OpID::ID, "x", s9),
                                Comp(OpID::PLAIN_INT, "2", s11),
                                s10
                            ),
                            s8
                        ),
                        Comp(OpID::ID, "Int", s13),
                        s12
                    ),
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
                s1
            )),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new ID("x", s4)),
                        NodePtr(new ID("Int", s6)),
                        s5
                    ))),
                    {},
                    s3
                ),
                NodePtr(new CallAttr(
                    NodePtr(new ID("x", s9)),
                    NodePtr(new ID("__gt__", s10)),
                    Args(
                        vec(NodePtr(new Int32(2, s11))),
                        {},
                        s10
                    ),
                    s10
                )),
                NodePtr(new ID("Int", s13)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s18)),
                    s17
                ))),
                s1
            )),
            s1
        ));

        // def f((3, x) as y = (3, 4)):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                    OpID::AS,
                                    Comp(
                                        OpID::GROUP,
                                        csv(
                                            Comp(OpID::PLAIN_INT, "3", s5),
                                            Comp(OpID::ID, "x", s7)
                                        ),
                                        s4
                                    ),
                                    Comp(OpID::ID, "y", s10),
                                    s9
                                ),
                                Comp(
                                    OpID::GROUP,
                                    csv(
                                        Comp(OpID::PLAIN_INT, "3", s13),
                                        Comp(OpID::PLAIN_INT, "4", s15)
                                    ),
                                    s12
                                ),
                                s11
                            ),
                            s3
                        ),
                        s3
                    ),
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
                s1
            )),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                MatchArgs(
                    vec(NodePtr(new Defaulted(
                        NodePtr(new As(
                            NodePtr(new MatchTuple(
                                vec(
                                    NodePtr(new Int32(3, s5)),
                                    NodePtr(new ID("x", s7))
                                ),
                                s4
                            )),
                            NodePtr(new ID("y", s10)),
                            s9
                        )),
                        NodePtr(new Tuple(
                            vec(
                                NodePtr(new Int32(3, s13)),
                                NodePtr(new Int32(4, s15))
                            ),
                            s12
                        )),
                        s11
                    ))),
                    {},
                    s3
                ),
                nullptr,
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s22)), s21
                ))),
                s1
            )),
            s1
        ));

        // def f()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == MissingBodyErr(s3));

        // def f:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedCallErr(s2));

        // def true():
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedGeneralIDErr(s2));

        // def f[]:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedParentheticalErr(s3));

        // def f(x, *, *, y):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(OpID::ID, "x", s4),
                                Comp(OpID::POS_KW_SEP, s6),
                                Comp(OpID::POS_KW_SEP, s8),
                                Comp(OpID::ID, "y", s10)
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
        )) == ArgSepWhereKeywordArgExpectedErr(s8));

        // def f(*, *args):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == VarArgsWhereKeywordArgExpectedErr(s6));

        // def f(a=1=2):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == TwiceDefaultedErr(s7));

        // def f(*args=1):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == DefaultedVarArgsErr(s6));

        // def f(**kwargs=1):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == DefaultedVarKeywordArgsErr(s6));

        // def f(3: Int):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedIDOrVarArgsErr(s4));

        // def f():
        //     return 0
        // elif true:
        //     return 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedCaseErr(s11));
    }

    SECTION("DIV") {
        // a / b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::DIV,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__div__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("For") {
        // for x in c:
        //     a += x
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == For(
            vec(NodePtr(new ID("x", s2))),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s8)),
                NodePtr(new ID("__iadd__", s9)),
                Args(vec(NodePtr(new ID("x", s10))), {}, s9),
                s9
            ))),
            {},
            s1
        ));

        // for x, y in c:
        //     a += x + y
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == For(
            vec(NodePtr(new ID("x", s2)), NodePtr(new ID("y", s4))),
            NodePtr(new ID("c", s6)),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s10)),
                NodePtr(new ID("__iadd__", s11)),
                Args(
                    vec(NodePtr(new CallAttr(
                        NodePtr(new ID("x", s12)),
                        NodePtr(new ID("__add__", s13)),
                        Args(vec(NodePtr(new ID("y", s14))), {}, s13),
                        s13
                    ))),
                    {},
                    s11
                ),
                s11
            ))),
            {},
            s1
        ));
        
        // for x in c:
        //     break
        // else:
        //     a += 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == For(
            vec(NodePtr(new ID("x", s2))),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new Break(s8))),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s14)),
                NodePtr(new ID("__iadd__", s15)),
                Args(vec(NodePtr(new Int32(1, s16))), {}, s15),
                s15
            ))),
            s1
        ));

        // for _ in c:
        //     print("hi")
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::FOR,
                Comp(
                    OpID::LABEL,
                    Comp(
                        OpID::IN,
                        Comp(OpID::PLACEHOLDER, s2),
                        Comp(OpID::ID, "c", s4),
                        s3
                    ),
                    Comp(
                        OpID::BLOCK,
                        vec(Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "print", s8),
                            Comp(
                                OpID::GROUP,
                                Comp(OpID::STRING, "hi", s10),
                                s9
                            ),
                            s9
                        )),
                        s7
                    ),
                    s5
                ),
                s1
            )),
            s1
        )) == For(
            vec(NodePtr(new Placeholder(s2))),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new Call(
                NodePtr(new ID("print", s8)),
                Args(
                    vec(NodePtr(new String("hi", s10))),
                    {},
                    s9
                ),
                s9
            ))),
            {},
            s1
        ));

        // for x in c
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == MissingBodyErr(s3));

        // for x:
        //     a += x
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedInErr(s2));

        // for 3 in c:
        //     a += 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedCommaSeparatedLoopVarsErr(s2));

        // for x in c:
        //     a += x
        // elif true:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(OpID::PLAIN_INT, 0, s18),
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
        )) == ExpectedElseErr(s12));
    }

    SECTION("GetAttr") {
        // a.b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == GetAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("b", s3)),
            s2
        ));

        // a.3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::PLAIN_INT, "3", s3),
            s2
        )) == GetAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new NumID(3, s3)),
            s2
        ));

        // a.true
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::TRUE, s3),
            s2
        )) == ExpectedGeneralIDErr(s3));
    }

    SECTION("IADD") {
        // a += b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IADD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__iadd__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IBAND") {
        // a &= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IBAND,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__iband__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IBOR") {
        // a |= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IBOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__ibor__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IBXOR") {
        // a ^= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IBXOR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__ibxor__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("ID") {
        // a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ID, "a", s1
        )) == ID(
            "a", s1
        ));
    }

    SECTION("IDIV") {
        // a /= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IDIV,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__idiv__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("ILSH") {
        // a <<= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ILSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__ilsh__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IMOD") {
        // a %= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IMOD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__imod__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IMUL") {
        // a *= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IMUL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__imul__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IPOW") {
        // a **= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IPOW,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__ipow__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("IRSH") {
        // a >>= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::IRSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__irsh__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("ISUB") {
        // a -= b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ISUB,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__isub__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("If") {
        // if x:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(Case(
                NodePtr(new ID("x", s2)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s7)), s6
                ))),
                s1
            )),
            {},
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(
                Case(
                    NodePtr(new ID("x", s2)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s7)), s6
                    ))),
                    s1
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(1, s15)), s14
                    ))),
                    s9
                )
            ),
            {},
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        // else:
        //     return 2
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(
                Case(
                    NodePtr(new ID("x", s2)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s7)), s6
                    ))),
                    s1
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(1, s15)), s14
                    ))),
                    s9
                )
            ),
            vec(NodePtr(new Return(
                NodePtr(new Int32(2, s22)), s21
            ))),
            s1
        ));

        // if x:
        //     return 0
        // else:
        //     return 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(Case(
                NodePtr(new ID("x", s2)),
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s7)), s6
                ))),
                s1
            )),
            vec(NodePtr(new Return(
                NodePtr(new Int32(1, s14)), s13
            ))),
            s1
        ));

        // if x:
        //     return 0
        // elif y:
        //     return 1
        // elif z:
        //     return 2
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(
                Case(
                    NodePtr(new ID("x", s2)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s7)), s6
                    ))),
                    s1
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(1, s15)), s14
                    ))),
                    s9
                ),
                Case(
                    NodePtr(new ID("z", s18)),
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(2, s23)), s22
                    ))),
                    s17
                )
            ),
            {},
            s1
        ));

        // if x
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::IF,
                Comp(OpID::ID, "x", s2),
                s1
            )),
            s1
        )) == MissingBodyErr(s2));

        // if x:
        //     return 0
        // elif y
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == MissingBodyErr(s10));

        // if x:
        //     return 0
        // else y:
        //     return 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == UnexpectedPredicateErr(s11));

        // if x:
        //     return 0
        // case y:
        //     return 1
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedElifOrElseErr(s9));
    }

    SECTION("Init") {
        // a: Int = 3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        )) == Init(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("Int", s3)),
            NodePtr(new Int32(3, s5)),
            s4
        ));

        // 3: Int = 3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::PLAIN_INT, "3", s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        )) == Init(
            NodePtr(new NumID(3, s1)),
            NodePtr(new ID("Int", s3)),
            NodePtr(new Int32(3, s5)),
            s4
        ));

        // true: Int = 3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::TRUE, s1),
                Comp(OpID::ID, "Int", s3),
                s2
            ),
            Comp(OpID::PLAIN_INT, "3", s5),
            s4
        )) == ExpectedGeneralIDErr(s1));
    }

    SECTION("LSH") {
        // a << b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__lsh__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Lambda Args") {
        // %*
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA_ARGS, s1
        )) == LambdaArgs(
            1, s1
        ));

        // %%%*
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA,
            Comp(
                OpID::LAMBDA,
                Comp(
                    OpID::LAMBDA_ARGS, s3
                ),
                s2
            ),
            s1
        )) == LambdaArgs(
            3, s1
        ));
    }

    SECTION("Lambda Expression") {
        // %(%1 + %2)
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Lambda(
            NodePtr(new CallAttr(
                NodePtr(new LambdaVar(1, NodePtr(new NumID(1, s4)), s3)),
                NodePtr(new ID("__add__", s5)),
                Args(
                    vec(NodePtr(new LambdaVar(
                        1,
                        NodePtr(new NumID(2, s7)),
                        s6
                    ))),
                    {},
                    s5
                ),
                s5
            )),
            s1
        ));

        // %%(%1 + %2)
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA,
            Comp(
                OpID::LAMBDA,
                Comp(
                    OpID::GROUP,
                    Comp(
                        OpID::ADD,
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "1", s5),
                            s4
                        ),
                        Comp(
                            OpID::LAMBDA,
                            Comp(OpID::PLAIN_INT, "2", s8),
                            s7
                        ),
                        s6
                    ),
                    s3
                ),
                s2
            ),
            s1
        )) == ExpectedGeneralIDErr(s3));

        // %[%1 + %2]
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedGeneralIDErr(s2));
    }

    SECTION("Lambda Keyword Args") {
        // %**
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA_KWARGS, s1
        )) == LambdaKeywordArgs(
            1, s1
        ));

        // %%%**
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA,
            Comp(
                OpID::LAMBDA,
                Comp(
                    OpID::LAMBDA_KWARGS, s3
                ),
                s2
            ),
            s1
        )) == LambdaKeywordArgs(
            3, s1
        ));
    }

    SECTION("Lambda Var") {
        // %1
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA, Comp(OpID::PLAIN_INT, "1", s2), s1
        )) == LambdaVar(
            1, NodePtr(new NumID(1, s2)), s1
        ));

        // %a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA, Comp(OpID::ID, "a", s2), s1
        )) == LambdaVar(
            1, NodePtr(new ID("a", s2)), s1
        ));

        // %%%1
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA,
            Comp(
                OpID::LAMBDA,
                Comp(OpID::LAMBDA, Comp(OpID::PLAIN_INT, "1", s4), s3),
                s2
            ),
            s1
        )) == LambdaVar(3, NodePtr(new NumID(1, s4)), s1));
    }

    SECTION("List") {
        // []
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST, Comp(OpID::NOTHING, s2), s1
        )) == List(
            {}, s1
        ));

        // [a]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST, Comp(OpID::ID, "a", s2), s1
        )) == List(
            vec(NodePtr(new ID("a", s2))), s1
        ));

        // [a, b, c]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        )) == List(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new ID("b", s4)),
                NodePtr(new ID("c", s6))
            ),
            s1
        ));

        // [a, *args, b]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        )) == List(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", s5)), s4
                )),
                NodePtr(new ID("b", s7))
            ),
            s1
        ));
    }

    SECTION("Literals") {
        // true
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TRUE, s1
        )) == Bool(
            true, s1
        ));

        // false
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FALSE, s1
        )) == Bool(
            false, s1
        ));

        // 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::PLAIN_INT, "0", s1
        )) == Int32(
            0, s1
        ));

        // 0s8
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::int8_t(0), s1
        )) == Int8(
            0, s1
        ));

        // 0s16
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::int16_t(0), s1
        )) == Int16(
            0, s1
        ));

        // 0s32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::int32_t(0), s1
        )) == Int32(
            0, s1
        ));

        // 0s64
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::int64_t(0), s1
        )) == Int64(
            0, s1
        ));

        // 0u8
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::uint8_t(0), s1
        )) == UInt8(
            0, s1
        ));

        // 0u16
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::uint16_t(0), s1
        )) == UInt16(
            0, s1
        ));

        // 0u32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::uint32_t(0), s1
        )) == UInt32(
            0, s1
        ));

        // 0u64
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NUMBER, std::uint64_t(0), s1
        )) == UInt64(0, s1));

        // 0f32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "0f32", s1
        )) == Float32(
            0, s1
        ));

        // 0f64
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "0f64", s1
        )) == Float64(
            0, s1
        ));

        // 1e7
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "1e7", s1
        )) == Float64(
            1e7, s1
        ));

        // 1e-7
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "1e-7", s1
        )) == Float64(
            1e-7, s1
        ));

        // 12.34
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::PLAIN_INT, "34", s3),
            s2
        )) == Float64(12.34, s1));

        // 12.34f32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34f32", s3),
            s2
        )) == Float32(12.34f, s1));

        // 12.34e5
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34e5", s3),
            s2
        )) == Float64(12.34e5, s1));

        // 12.34e5f32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GET,
            Comp(OpID::PLAIN_INT, "12", s1),
            Comp(OpID::FLOAT_TAIL, "34e5f32", s3),
            s2
        )) == Float32(12.34e5f, s1));

        // 1e46f32
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "1e46f32", s1
        )) == OutOfRangeErr(s1));

        // 1e309
        REQUIRE(*interpreter.interpret(Comp(
            OpID::FLOAT_TAIL, "1e309", s1
        )) == OutOfRangeErr(s1));

        // '\0'
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CHAR, std::int32_t(0), s1
        )) == Char(
            0, s1
        ));

        // "asdf"
        REQUIRE(*interpreter.interpret(Comp(
            OpID::STRING, "asdf", s1
        )) == String(
            "asdf", s1
        ));
    }

    SECTION("MOD") {
        // a % b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::MOD,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__mod__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("MUL") {
        // a * b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::MUL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__mul__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Map") {
        // {}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE, Comp(OpID::NOTHING, s2), s1
        )) == Map(
            {}, s1
        ));

        // {a = 1}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            Comp(
                OpID::BIND,
                Comp(OpID::ID, "a", s2),
                Comp(OpID::PLAIN_INT, "1", s4),
                s3
            ),
            s1
        )) == Map(
            vec(NodePtr(new Entry(
                NodePtr(new ID("a", s2)),
                NodePtr(new Int32(1, s4)),
                s3
            ))),
            s1
        ));

        // {a = 1, b = 2, c = 3}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(
                    OpID::BIND,
                    Comp(OpID::ID, "a", s2),
                    Comp(OpID::PLAIN_INT, "1", s4),
                    s3
                ),
                Comp(
                    OpID::BIND,
                    Comp(OpID::ID, "b", s6),
                    Comp(OpID::PLAIN_INT, "2", s8),
                    s7
                ),
                Comp(
                    OpID::BIND,
                    Comp(OpID::ID, "c", s10),
                    Comp(OpID::PLAIN_INT, "3", s12),
                    s11
                )
            ),
            s1
        )) == Map(
            vec(
                NodePtr(new Entry(
                    NodePtr(new ID("a", s2)),
                    NodePtr(new Int32(1, s4)),
                    s3
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("b", s6)),
                    NodePtr(new Int32(2, s8)),
                    s7
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("c", s10)),
                    NodePtr(new Int32(3, s12)),
                    s11
                ))
            ),
            s1
        ));

        // {a = 1, **kwargs, b = 2}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(
                    OpID::BIND,
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
                    OpID::BIND,
                    Comp(OpID::ID, "b", s9),
                    Comp(OpID::PLAIN_INT, "2", s11),
                    s10
                )
            ),
            s1
        )) == Map(
            vec(
                NodePtr(new Entry(
                    NodePtr(new ID("a", s2)),
                    NodePtr(new Int32(1, s4)),
                    s3
                )),
                NodePtr(new Expansion(
                    NodePtr(new ID("kwargs", s7)), s6
                )),
                NodePtr(new Entry(
                    NodePtr(new ID("b", s9)),
                    NodePtr(new Int32(2, s11)),
                    s10
                ))
            ),
            s1
        ));
    }

    SECTION("Match") {
        // match x
        // case y:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s5),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s10),
                                s9
                            )),
                            s8
                        ),
                        s6
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new ID("y", s5)),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s10)), s9
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case y: Int:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "y", s5),
                            Comp(OpID::ID, "Int", s7),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s12),
                                s11
                            )),
                            s10
                        ),
                        s8
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("y", s5)), NodePtr(new ID("Int", s7)), s6
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s12)), s11
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case y if y > 0:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::TERNARY_IF,
                            Comp(OpID::ID, "y", s5),
                            Comp(
                                OpID::GT,
                                Comp(OpID::ID, "y", s7),
                                Comp(OpID::PLAIN_INT, "0", s9),
                                s8
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new ID("y", s5)),
                NodePtr(new CallAttr(
                    NodePtr(new ID("y", s7)),
                    NodePtr(new ID("__gt__", s8)),
                    Args(vec(NodePtr(new Int32(0, s9))), {}, s8),
                    s8
                )),
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s14)), s13
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case y: Int:
        //     return y
        // case _:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "y", s5),
                            Comp(OpID::ID, "Int", s7),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s12),
                                s11
                            )),
                            s10
                        ),
                        s8
                    ),
                    s4
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::PLACEHOLDER, s15),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::PLAIN_INT, "0", s20),
                                s19
                            )),
                            s18
                        ),
                        s16
                    ),
                    s14
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(
                MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("y", s5)), NodePtr(new ID("Int", s7)), s6
                    )),
                    nullptr,
                    vec(NodePtr(new Return(
                        NodePtr(new ID("y", s12)), s11
                    ))),
                    s4
                ),
                MatchCase(
                    NodePtr(new Placeholder(s15)),
                    nullptr,
                    vec(NodePtr(new Return(
                        NodePtr(new Int32(0, s20)), s19
                    ))),
                    s14
                )
            ),
            s1
        ));

        // match x
        // case (y):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(OpID::ID, "y", s6),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s12),
                                s11
                            )),
                            s10
                        ),
                        s8
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new ID("y", s6)),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s12)), s11
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case (y, z):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(OpID::ID, "y", s6),
                                Comp(OpID::ID, "z", s8)
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchTuple(
                    vec(
                        NodePtr(new ID("y", s6)),
                        NodePtr(new ID("z", s8))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s14)), s13
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case (y, *args, z):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            csv(
                                Comp(OpID::ID, "y", s6),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s9),
                                    s8
                                ),
                                Comp(OpID::ID, "z", s11)
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s17),
                                s16
                            )),
                            s15
                        ),
                        s13
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchTuple(
                    vec(
                        NodePtr(new ID("y", s6)),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s9)), s8
                        )),
                        NodePtr(new ID("z", s11))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s17)), s16
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case (*args):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::UNPACK_ARGS,
                                Comp(OpID::ID, "args", s7),
                                s6
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new Expansion(
                        NodePtr(new ID("args", s7)), s6
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s13)), s12
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case [y, z]:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            csv(
                                Comp(OpID::ID, "y", s6),
                                Comp(OpID::ID, "z", s8)
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchList(vec(
                        NodePtr(new ID("y", s6)),
                        NodePtr(new ID("z", s8))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s14)), s13
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case [y, *args, z]:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            csv(
                                Comp(OpID::ID, "y", s6),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s9),
                                    s8
                                ),
                                Comp(OpID::ID, "z", s11)
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s17),
                                s16
                            )),
                            s15
                        ),
                        s13
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchList(
                    vec(
                        NodePtr(new ID("y", s6)),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s9)), s8
                        )),
                        NodePtr(new ID("z", s11))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s17)), s16
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case {3 = y, 4 = z}:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            csv(
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::PLAIN_INT, "3", s6),
                                    Comp(OpID::ID, "y", s8),
                                    s7
                                ),
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::PLAIN_INT, "4", s10),
                                    Comp(OpID::ID, "z", s12),
                                    s11
                                )
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s18),
                                s17
                            )),
                            s16
                        ),
                        s14
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchMap(
                    vec(
                        NodePtr(new Entry(
                            NodePtr(new Int32(3, s6)),
                            NodePtr(new ID("y", s8)),
                            s7
                        )),
                        NodePtr(new Entry(
                            NodePtr(new Int32(4, s10)),
                            NodePtr(new ID("z", s12)),
                            s11
                        ))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s18)), s17
                ))),
                s4
            )),
            s1
        ));

        // match x:
        // case {3 = y: Int}:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::BIND,
                                Comp(OpID::PLAIN_INT, "3", s6),
                                Comp(
                                    OpID::TYPE_LABEL,
                                    Comp(OpID::ID, "y", s8),
                                    Comp(OpID::ID, "Int", s10),
                                    s9
                                ),
                                s7
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s16),
                                s15
                            )),
                            s14
                        ),
                        s12
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new Entry(
                        NodePtr(new Int32(3, s6)),
                        NodePtr(new TypeMatch(
                            NodePtr(new ID("y", s8)),
                            NodePtr(new ID("Int", s10)),
                            s9
                        )),
                        s7
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s16)), s15
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case {3 = y, 4 = z, **kwargs}:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            csv(
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::PLAIN_INT, "3", s6),
                                    Comp(OpID::ID, "y", s8),
                                    s7
                                ),
                                Comp(
                                    OpID::BIND,
                                    Comp(OpID::PLAIN_INT, "4", s10),
                                    Comp(OpID::ID, "z", s12),
                                    s11
                                ),
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::ID, "kwargs", s15),
                                    s14
                                )
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s21),
                                s20
                            )),
                            s19
                        ),
                        s17
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchMap(
                    vec(
                        NodePtr(new Entry(
                            NodePtr(new Int32(3, s6)),
                            NodePtr(new ID("y", s8)),
                            s7
                        )),
                        NodePtr(new Entry(
                            NodePtr(new Int32(4, s10)),
                            NodePtr(new ID("z", s12)),
                            s11
                        )),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", s15)), s14
                        ))
                    ),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s21)), s20
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case T(y, w=z):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "T", s5),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "y", s7),
                                    Comp(
                                        OpID::BIND,
                                        Comp(OpID::ID, "w", s9),
                                        Comp(OpID::ID, "z", s11),
                                        s10
                                    )
                                ),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s17),
                                s16
                            )),
                            s15
                        ),
                        s13
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", s5)),
                    MatchArgs(
                        vec(NodePtr(new ID("y", s7))),
                        vec(NodePtr(new MatchKeywordArg(
                            ID("w", s9), NodePtr(new ID("z", s11)), s10
                        ))),
                        s6
                    ),
                    s6
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s17)), s16
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case T(y, *args, **kwargs):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "T", s5),
                            Comp(
                                OpID::GROUP,
                                csv(
                                    Comp(OpID::ID, "y", s7),
                                    Comp(
                                        OpID::UNPACK_ARGS,
                                        Comp(OpID::ID, "args", s10),
                                        s9
                                    ),
                                    Comp(
                                        OpID::UNPACK_KWARGS,
                                        Comp(OpID::ID, "kwargs", s13),
                                        s12
                                    )
                                ),
                                s6
                            ),
                            s6
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s19),
                                s18
                            )),
                            s17
                        ),
                        s15
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchObject(
                    NodePtr(new ID("T", s5)),
                    MatchArgs(
                        vec(
                            NodePtr(new ID("y", s7)),
                            NodePtr(new Expansion(
                                NodePtr(new ID("args", s10)), s9
                            ))
                        ),
                        vec(NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", s13)), s12
                        ))),
                        s6
                    ),
                    s6
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s19)), s18
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case 1 || 2:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::MATCH_OR,
                            Comp(OpID::PLAIN_INT, "1", s5),
                            Comp(OpID::PLAIN_INT, "2", s7),
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchOptions(
                    vec(
                        NodePtr(new Int32(1, s5)),
                        NodePtr(new Int32(2, s7))
                    ),
                    s6
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s12)), s11
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case Int(y) || String(y):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::MATCH_OR,
                            Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "Int", s5),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::ID, "y", s7),
                                    s6
                                ),
                                s6
                            ),
                            Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "String", s10),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::ID, "y", s12),
                                    s11
                                ),
                                s11
                            ),
                            s9
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s18),
                                s17
                            )),
                            s16
                        ),
                        s14
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchOptions(
                    vec(
                        NodePtr(new MatchObject(
                            NodePtr(new ID("Int", s5)),
                            MatchArgs(
                                vec(NodePtr(new ID("y", s7))),
                                {},
                                s6
                            ),
                            s6
                        )),
                        NodePtr(new MatchObject(
                            NodePtr(new ID("String", s10)),
                            MatchArgs(
                                vec(NodePtr(new ID("y", s12))),
                                {},
                                s11
                            ),
                            s11
                        ))
                    ),
                    s9
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s18)), s17
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case 1 || 2 as y:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::AS,
                            Comp(
                                OpID::MATCH_OR,
                                Comp(OpID::PLAIN_INT, "1", s5),
                                Comp(OpID::PLAIN_INT, "2", s7),
                                s6
                            ),
                            Comp(OpID::ID, "y", s9),
                            s8
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new As(
                    NodePtr(new MatchOptions(
                        vec(
                            NodePtr(new Int32(1, s5)),
                            NodePtr(new Int32(2, s7))
                        ),
                        s6
                    )),
                    NodePtr(new ID("y", s9)),
                    s8
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s14)), s13
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case (3 as *args):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::AS,
                                Comp(OpID::PLAIN_INT, "3", s6),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s9),
                                    s8
                                ),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new As(
                        NodePtr(new Int32(3, s6)),
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s9)), s8
                        )),
                        s7
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s15)), s14
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case (*args: Int):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s7),
                                    s6
                                ),
                                Comp(OpID::ID, "Int", s9),
                                s8
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchTuple(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new Expansion(
                            NodePtr(new ID("args", s7)),
                            s6
                        )),
                        NodePtr(new ID("Int", s9)),
                        s8
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s15)), s14
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case {3 as **kwargs}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::AS,
                                Comp(OpID::PLAIN_INT, "3", s6),
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::ID, "kwargs", s9),
                                    s8
                                ),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new As(
                        NodePtr(new Int32(3, s6)),
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", s9)), s8
                        )),
                        s7
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s15)), s14
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case {**kwargs: Int}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::ID, "kwargs", s7),
                                    s6
                                ),
                                Comp(OpID::ID, "Int", s9),
                                s8
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new MatchMap(
                    vec(NodePtr(new TypeMatch(
                        NodePtr(new Expansion(
                            NodePtr(new ID("kwargs", s7)),
                            s6
                        )),
                        NodePtr(new ID("Int", s9)),
                        s8
                    ))),
                    s5
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new Int32(0, s15)), s14
                ))),
                s4
            )),
            s1
        ));

        // match x
        // case |f(y):
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LITERALLY,
                            Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "f", s6),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::ID, "y", s8),
                                    s7
                                ),
                                s7
                            ),
                            s5
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(MatchCase(
                NodePtr(new Call(
                    NodePtr(new ID("f", s6)),
                    Args(
                        vec(NodePtr(new ID("y", s8))),
                        {},
                        s7
                    ),
                    s7
                )),
                nullptr,
                vec(NodePtr(new Return(
                    NodePtr(new ID("y", s14)), s13
                ))),
                s4
            )),
            s1
        ));

        // match x
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::MATCH, Comp(OpID::ID, "x", s2), s1
            )),
            s1
        )) == NoMatchCasesErr(s1));

        // match x
        // case *args:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::UNPACK_ARGS,
                            Comp(OpID::ID, "args", s6),
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
                    s4
                )
            ),
            s1
        )) == VarArgsNotAllowedHereErr(s5));

        // match x
        // case **kwargs:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::UNPACK_KWARGS,
                            Comp(OpID::ID, "kwargs", s6),
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
                    s4
                )
            ),
            s1
        )) == VarKeywordArgsNotAllowedHereErr(s5));

        // match x
        // case [**kwargs]:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            Comp(
                                OpID::UNPACK_KWARGS,
                                Comp(OpID::ID, "kwargs", s7),
                                s6
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == VarKeywordArgsNotAllowedHereErr(s6));

        // match x
        // case {*args}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::UNPACK_ARGS,
                                Comp(OpID::ID, "args", s7),
                                s6
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedEntryOrVarKeywordArgsErr(s6));

        // match x
        // case [y=2]:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            Comp(
                                OpID::BIND,
                                Comp(OpID::ID, "y", s6),
                                Comp(OpID::PLAIN_INT, "2", s8),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == UnexpectedOpErr(OpID::BIND, s7));

        // match x
        // case y as z as w:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::AS,
                            Comp(
                                OpID::AS,
                                Comp(OpID::ID, "y", s5),
                                Comp(OpID::ID, "z", s7),
                                s6
                            ),
                            Comp(OpID::ID, "w", s9),
                            s8
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == DoubleAsErr(s8));

        // match x
        // case y: Int as z:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::AS,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(OpID::ID, "y", s5),
                                Comp(OpID::ID, "Int", s7),
                                s6
                            ),
                            Comp(OpID::ID, "z", s9),
                            s8
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s14),
                                s13
                            )),
                            s12
                        ),
                        s10
                    ),
                    s4
                )
            ),
            s1
        )) == TypeMatchWithAsErr(s6));

        // match x
        // case [*args as y]:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            Comp(
                                OpID::AS,
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::ID, "args", s7),
                                    s6
                                ),
                                Comp(OpID::ID, "y", s9),
                                s8
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == VarArgsWithAsErr(s6));

        // match x
        // case T(**kwargs as y):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::CALL,
                            Comp(OpID::ID, "T", s5),
                            Comp(
                                OpID::GROUP,
                                Comp(
                                    OpID::AS,
                                    Comp(
                                        OpID::UNPACK_KWARGS,
                                        Comp(OpID::ID, "kwargs", s8),
                                        s7
                                    ),
                                    Comp(OpID::ID, "y", s10),
                                    s9
                                ),
                                s6
                            ),
                            s6
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
                    s4
                )
            ),
            s1
        )) == VarArgsWithAsErr(s7));

        // match x
        // case (y: Int):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(OpID::ID, "y", s6),
                                Comp(OpID::ID, "Int", s8),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedExprErr(s7));

        // match x
        // case (3 as y):
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::GROUP,
                            Comp(
                                OpID::AS,
                                Comp(OpID::PLAIN_INT, "3", s6),
                                Comp(OpID::ID, "y", s8),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedExprErr(s7));

        // match x
        // case {y: Int}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(OpID::ID, "y", s6),
                                Comp(OpID::ID, "Int", s8),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedEntryOrVarKeywordArgsErr(s7));

        // match x
        // case {3 as y}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::AS,
                                Comp(OpID::PLAIN_INT, "3", s6),
                                Comp(OpID::ID, "y", s8),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedEntryOrVarKeywordArgsErr(s7));

        // match x
        // case [*3: Int]
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::PLAIN_INT, "3", s7),
                                    s6
                                ),
                                Comp(OpID::ID, "Int", s9),
                                s8
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedIDErr(s7));

        // match x
        // case {**3: Int}
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::TYPE_LABEL,
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::PLAIN_INT, "3", s7),
                                    s6
                                ),
                                Comp(OpID::ID, "Int", s9),
                                s8
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedIDErr(s7));


        // match x
        // case [y as *3]:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::LIST,
                            Comp(
                                OpID::AS,
                                Comp(OpID::ID, "y", s6),
                                Comp(
                                    OpID::UNPACK_ARGS,
                                    Comp(OpID::PLAIN_INT, "3", s9),
                                    s8
                                ),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedIDErr(s9));

        // match x
        // case {y as **3}:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::ENCLOSURE,
                            Comp(
                                OpID::AS,
                                Comp(OpID::ID, "y", s6),
                                Comp(
                                    OpID::UNPACK_KWARGS,
                                    Comp(OpID::PLAIN_INT, "3", s9),
                                    s8
                                ),
                                s7
                            ),
                            s5
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
                    s4
                )
            ),
            s1
        )) == ExpectedIDErr(s9));

        // match x
        // case y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::CASE,
                    Comp(OpID::ID, "y", s5),
                    s4
                )
            ),
            s1
        )) == MissingBodyErr(s5));

        // match x
        // elif y:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(
                Comp(
                    OpID::MATCH,
                    Comp(OpID::ID, "x", s2),
                    s1
                ),
                Comp(
                    OpID::ELIF,
                    Comp(
                        OpID::LABEL,
                        Comp(OpID::ID, "y", s5),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::RETURN,
                                Comp(OpID::ID, "y", s10),
                                s9
                            )),
                            s8
                        ),
                        s6
                    ),
                    s4
                )
            ),
            s1
        )) == ExpectedCaseErr(s4));
    }

    SECTION("NEG") {
        // -a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::NEG,
            Comp(OpID::ID, "a", s2),
            s1
        )) == CallAttr(
            NodePtr(new ID("a", s2)),
            NodePtr(new ID("__neg__", s1)),
            Args({}, {}, s1),
            s1
        ));
    }

    SECTION("Or") {
        // a or b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::OR,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == Or(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("b", s3)),
            s2
        ));
    }

    SECTION("POW") {
        // a ** b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::POW,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__pow__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("RSH") {
        // a >> b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RSH,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__rsh__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Raise") {
        // raise
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RAISE, Comp(OpID::NOTHING, s_end), s1
        )) == Raise(
            nullptr, s1
        ));

        // raise a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RAISE, Comp(OpID::ID, "a", s2), s1
        )) == Raise(
            NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("Return") {
        // return
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RETURN, Comp(OpID::NOTHING, s_end), s1
        )) == Return(
            nullptr, s1
        ));

        // return a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RETURN, Comp(OpID::ID, "a", s2), s1
        )) == Return(
            NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("SUB") {
        // a - b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SUB,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            s2
        )) == CallAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("__sub__", s2)),
            Args(vec(NodePtr(new ID("b", s3))), {}, s2),
            s2
        ));
    }

    SECTION("Set") {
        // {a}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            Comp(OpID::ID, "a", s2),
            s1
        )) == Set(vec(NodePtr(new ID("a", s2))), s1));

        // {a, b, c}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        )) == Set(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new ID("b", s4)),
                NodePtr(new ID("c", s6))
            ),
            s1
        ));

        // {a, *args, b}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        )) == Set(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", s5)), s4
                )),
                NodePtr(new ID("b", s7))
            ),
            s1
        ));
    }

    SECTION("SetAttr") {
        // a.b = c
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::ID, "b", s3),
                s2
            ),
            Comp(OpID::ID, "c", s5),
            s4
        )) == SetAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("b", s3)),
            NodePtr(new ID("c", s5)),
            s4
        ));

        // a.3 = b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::PLAIN_INT, "3", s3),
                s2
            ),
            Comp(OpID::ID, "b", s5),
            s4
        )) == SetAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new NumID(3, s3)),
            NodePtr(new ID("b", s5)),
            s4
        ));

        // a.true = b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::GET,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::TRUE, s3),
                s2
            ),
            Comp(OpID::ID, "b", s5),
            s4
        )) == ExpectedGeneralIDErr(s3));
    }

    SECTION("Symbol") {
        // :x
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SYMBOL, Comp(OpID::ID, "x", s2), s1
        )) == Symbol(NodePtr(new ID("x", s2)), s1));

        // :3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SYMBOL,
            Comp(OpID::PLAIN_INT, "3", s2),
            s1
        )) == Symbol(NodePtr(new NumID(3, s2)), s1));
    }

    SECTION("Ternary") {
        // x if y else z
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TERNARY_ELSE,
            Comp(
                OpID::TERNARY_IF,
                Comp(OpID::ID, "x", s1),
                Comp(OpID::ID, "y", s3),
                s2
            ),
            Comp(OpID::ID, "z", s5),
            s4
        )) == Ternary(
            NodePtr(new ID("y", s3)),
            NodePtr(new ID("x", s1)),
            NodePtr(new ID("z", s5)),
            s4
        ));

        // x if y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TERNARY_IF,
            Comp(OpID::ID, "x", s1),
            Comp(OpID::ID, "y", s3),
            s2
        )) == ExpectedExprErr(s2));

        // x else y
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TERNARY_ELSE,
            Comp(OpID::ID, "x", s1),
            Comp(OpID::ID, "y", s3),
            s2
        )) == ExpectedTernaryIfErr(s1));
    }

    SECTION("This") {
        // this
        REQUIRE(*interpreter.interpret(Comp(
            OpID::THIS, s1
        )) == This(s1));
    }

    SECTION("Try") {
        // try:
        //     f()
        // except e: E:
        //     g()
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s7),
                                    s6
                                ),
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
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", s10),
                            Comp(OpID::ID, "E", s12),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s16),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s18),
                                    s17
                                ),
                                s17
                            )),
                            s15
                        ),
                        s13
                    ),
                    s9
                )
            ),
            s1
        )) == Try(
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s5)),
                Args({}, {}, s6),
                s6
            ))),
            vec(MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("e", s10)),
                    NodePtr(new ID("E", s12)),
                    s11
                )),
                nullptr,
                vec(NodePtr(new Call(
                    NodePtr(new ID("g", s16)),
                    Args({}, {}, s17),
                    s17
                ))),
                s9
            )),
            {},
            s1
        ));

        // try:
        //     f()
        // except e: E:
        //     g()
        // finally:
        //     h()
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s7),
                                    s6
                                ),
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
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", s10),
                            Comp(OpID::ID, "E", s12),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s16),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s18),
                                    s17
                                ),
                                s17
                            )),
                            s15
                        ),
                        s13
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
                                Comp(OpID::ID, "h", s24),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s26),
                                    s25
                                ),
                                s25
                            )),
                            s23
                        ),
                        s21
                    ),
                    s20
                )
            ),
            s1
        )) == Try(
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s5)),
                Args({}, {}, s6),
                s6
            ))),
            vec(MatchCase(
                NodePtr(new TypeMatch(
                    NodePtr(new ID("e", s10)),
                    NodePtr(new ID("E", s12)),
                    s11
                )),
                nullptr,
                vec(NodePtr(new Call(
                    NodePtr(new ID("g", s16)),
                    Args({}, {}, s17),
                    s17
                ))),
                s9
            )),
            vec(NodePtr(new Call(
                NodePtr(new ID("h", s24)),
                Args({}, {}, s25),
                s25
            ))),
            s1
        ));

        // try:
        //     f()
        // finally:
        //     g()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Try(
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s5)), Args({}, {}, s6), s6
            ))),
            {},
            vec(NodePtr(new Call(
                NodePtr(new ID("g", s13)), Args({}, {}, s14), s14
            ))),
            s1
        ));

        // try:
        //     f()
        // except e: E1:
        //     g()
        // except e: E2:
        //     h()
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s7),
                                    s6
                                ),
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
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", s10),
                            Comp(OpID::ID, "E1", s12),
                            s11
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s16),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s18),
                                    s17
                                ),
                                s17
                            )),
                            s15
                        ),
                        s13
                    ),
                    s9
                ),
                Comp(
                    OpID::EXCEPT,
                    Comp(
                        OpID::LABEL,
                        Comp(
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", s21),
                            Comp(OpID::ID, "E2", s23),
                            s22
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "h", s27),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s29),
                                    s28
                                ),
                                s28
                            )),
                            s26
                        ),
                        s24
                    ),
                    s20
                )
            ),
            s1
        )) == Try(
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s5)),
                Args({}, {}, s6),
                s6
            ))),
            vec(
                MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("e", s10)),
                        NodePtr(new ID("E1", s12)),
                        s11
                    )),
                    nullptr,
                    vec(NodePtr(new Call(
                        NodePtr(new ID("g", s16)),
                        Args({}, {}, s17),
                        s17
                    ))),
                    s9
                ),
                MatchCase(
                    NodePtr(new TypeMatch(
                        NodePtr(new ID("e", s21)),
                        NodePtr(new ID("E2", s23)),
                        s22
                    )),
                    nullptr,
                    vec(NodePtr(new Call(
                        NodePtr(new ID("h", s27)),
                        Args({}, {}, s28),
                        s28
                    ))),
                    s20
                )
            ),
            {},
            s1
        ));

        // try:
        //     f()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == IsolatedTryErr(s1));

        // try a:
        //     f()
        // except e: E:
        //     g()
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s8),
                                    s7
                                ),
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
                            OpID::TYPE_LABEL,
                            Comp(OpID::ID, "e", s11),
                            Comp(OpID::ID, "E", s13),
                            s12
                        ),
                        Comp(
                            OpID::BLOCK,
                            vec(Comp(
                                OpID::CALL,
                                Comp(OpID::ID, "g", s17),
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s19),
                                    s18
                                ),
                                s18
                            )),
                            s16
                        ),
                        s14
                    ),
                    s10
                )
            ),
            s1
        )) == UnexpectedPredicateErr(s3));

        // try:
        //     f()
        // finally a:
        //     g()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == UnexpectedPredicateErr(s11));

        // try:
        //     f()
        // except e: E
        REQUIRE(*interpreter.interpret(Comp(
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
                                Comp(
                                    OpID::GROUP,
                                    Comp(OpID::NOTHING, s7),
                                    s6
                                ),
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
                        OpID::TYPE_LABEL,
                        Comp(OpID::ID, "e", s10),
                        Comp(OpID::ID, "E", s12),
                        s11
                    ),
                    s9
                )
            ),
            s1
        )) == MissingBodyErr(s11));
    }

    SECTION("Tuple") {
        // ()
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GROUP, Comp(OpID::NOTHING, s2), s1
        )) == Tuple(
            {}, s1
        ));

        // (a, b, c)
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GROUP,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        )) == Tuple(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new ID("b", s4)),
                NodePtr(new ID("c", s6))
            ),
            s1
        ));

        // a, b, c
        REQUIRE(*interpreter.interpret(csv(
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "b", s3),
            Comp(OpID::ID, "c", s5)
        )) == Tuple(
            vec(
                NodePtr(new ID("a", s1)),
                NodePtr(new ID("b", s3)),
                NodePtr(new ID("c", s5))
            ),
            s1
        ));


        // (a, *args, b)
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GROUP,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s5), s4),
                Comp(OpID::ID, "b", s7)
            ),
            s1
        )) == Tuple(
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", s5)), s4
                )),
                NodePtr(new ID("b", s7))
            ),
            s1
        ));

        // a, *args, b
        REQUIRE(*interpreter.interpret(csv(
            Comp(OpID::ID, "a", s1),
            Comp(OpID::UNPACK_ARGS, Comp(OpID::ID, "args", s4), s3),
            Comp(OpID::ID, "b", s6)
        )) == Tuple(
            vec(
                NodePtr(new ID("a", s1)),
                NodePtr(new Expansion(
                    NodePtr(new ID("args", s4)), s3
                )),
                NodePtr(new ID("b", s6))
            ),
            s1
        ));

        // (*args)
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GROUP,
            Comp(
                OpID::UNPACK_ARGS,
                Comp(OpID::ID, "args", s3),
                s2
            ),
            s1
        )) == Tuple(
            vec(NodePtr(new Expansion(
                NodePtr(new ID("args", s3)), s2
            ))),
            s1
        ));
    }

    SECTION("Update") {
        // a(i) = b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET,
            Comp(
                OpID::CALL,
                Comp(OpID::ID, "a", s1),
                Comp(OpID::GROUP, Comp(OpID::ID, "i", s3), s2),
                s2
            ),
            Comp(OpID::ID, "b", s6),
            s5
        )) == Update(
            NodePtr(new ID("a", s1)),
            Args(vec(NodePtr(new ID("i", s3))), {}, s2),
            NodePtr(new ID("b", s6)),
            s5
        ));
    }

    SECTION("UpdateAttr") {
        // a.b(i) = c
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == UpdateAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new ID("b", s3)),
            Args(vec(NodePtr(new ID("i", s5))), {}, s4),
            NodePtr(new ID("c", s8)),
            s7
        ));

        // a.3(i) = c
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == UpdateAttr(
            NodePtr(new ID("a", s1)),
            NodePtr(new NumID(3, s3)),
            Args(vec(NodePtr(new ID("i", s5))), {}, s4),
            NodePtr(new ID("c", s8)),
            s7
        ));

        // a.true(i) = c
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedGeneralIDErr(s3));
    }

    SECTION("Var") {
        // .1
        REQUIRE(*interpreter.interpret(Comp(
            OpID::UP, Comp(OpID::PLAIN_INT, "1", s2), s1
        )) == Var(1, NodePtr(new NumID(1, s2)), s1));

        // .a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::UP, Comp(OpID::ID, "a", s2), s1
        )) == Var(1, NodePtr(new ID("a", s2)), s1));

        // ...a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::UP,
            Comp(
                OpID::UP,
                Comp(
                    OpID::UP,
                    Comp(
                        OpID::ID, "a", s4
                    ),
                    s3
                ),
                s2
            ),
            s1
        )) == Var(3, NodePtr(new ID("a", s4)), s1));
    }

    SECTION("While") {
        // while a:
        //     f()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == While(
            NodePtr(new ID("a", s2)),
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s6)), Args({}, {}, s7), s7
            ))),
            {},
            s1
        ));

        // while a:
        //     f()
        // else:
        //     g()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == While(
            NodePtr(new ID("a", s2)),
            vec(NodePtr(new Call(
                NodePtr(new ID("f", s6)), Args{{}, {}, s7}, s7
            ))),
            vec(NodePtr(new Call(
                NodePtr(new ID("g", s14)), Args({}, {}, s15), s15
            ))),
            s1
        ));

        // while a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CONSTRUCT,
            vec(Comp(
                OpID::WHILE,
                Comp(OpID::ID, "a", s2),
                s1
            )),
            s1
        )) == MissingBodyErr(s2));
    }
}
