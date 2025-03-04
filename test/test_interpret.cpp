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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("Addr") {
        // @a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ADDR, Comp(OpID::ID, "a", s2), s1
        )) == Unary(
            UnaryKind::ADDR, NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("AddrType") {
        // Int@
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ADDR_TYPE, Comp(OpID::ID, "Int", s1), s2
        )) == Unary(
            UnaryKind::ADDR_TYPE, NodePtr(new ID("Int", s1)), s2
        ));
    }

    SECTION("And") {
        // true and false
        REQUIRE(*interpreter.interpret(Comp(
            OpID::AND, Comp(OpID::TRUE, s1), Comp(OpID::FALSE, s3), s2
        )) == Binary(
            BinaryKind::AND,
            NodePtr(new Bool(true, s1)),
            NodePtr(new Bool(false, s3)),
            s2
        ));
    }

    SECTION("Assign") {
        // a = b
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SET, Comp(OpID::ID, "a", s1), Comp(OpID::ID, "b", s3), s2
        )) == Binary(
            BinaryKind::SET,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args({}, {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("Break") {
        // break
        REQUIRE(*interpreter.interpret(Comp(
            OpID::BREAK, s1
        )) == Nullary(
            NullaryKind::BREAK, s1
        ));
    }

    SECTION("Cached Call") {
        // Vector[Int]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::CALL,
            Comp(OpID::ID, "Vector", s1),
            Comp(OpID::LIST, Comp(OpID::ID, "Int", s3), s2),
            s2
        )) == Call(
            CallKind::CACHE,
            NodePtr(new ID("Vector", s1)),
            Args(vec(NodePtr(new ID("Int", s3))), {}),
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
        )) == CallAttr(
            NodePtr(new ID("obj", s1)),
            NodePtr(new ID("attr", s3)),
            Args(vec(NodePtr(new ID("T", s5))), {}),
            true,
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
            CallKind::CALL,
            NodePtr(new ID("fn", s1)),
            Args({}, {}),
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
            CallKind::CALL,
            NodePtr(new ID("fn", s1)),
            Args(
                vec(NodePtr(new Int32(1, s3)), NodePtr(new Int32(2, s5))), {}
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
            CallKind::CALL,
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Unary(
                        UnaryKind::EXPANSION, NodePtr(new ID("args", s8)), s7
                    ))
                ),
                {}
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
            CallKind::CALL,
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Unary(
                        UnaryKind::EXPANSION, NodePtr(new ID("args", s8)), s7
                    ))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        ID("kw1", s10), NodePtr(new String("yes", s12)), s11
                    )),
                    NodePtr(new KeywordArg{
                        ID("kw2", s14), NodePtr(new Bool(true, s16)), s15
                    })
                )
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
            CallKind::CALL,
            NodePtr(new ID("fn", s1)),
            Args(
                vec(
                    NodePtr(new Int32(1, s3)),
                    NodePtr(new Int32(2, s5)),
                    NodePtr(new Unary(
                        UnaryKind::EXPANSION, NodePtr(new ID("args", s8)), s7
                    ))
                ),
                vec(
                    NodePtr(new KeywordArg(
                        ID("kw1", s10), NodePtr(new String("yes", s12)), s11
                    )),
                    NodePtr(new KeywordArg(
                        ID("kw2", s14), NodePtr(new Bool(true, s16)), s15
                    )),
                    NodePtr(new Unary(
                        UnaryKind::EXPANSION,
                        NodePtr(new ID("kwargs", s19)),
                        s18
                    ))
                )
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

        // fn(1, kw1="yes", kw1=true)
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
        )) == DuplicateKeywordErr("kw1", s9));
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
                )
            ),
            false,
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
            Args({}, {}),
            false,
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
        )) == Nullary(
            NullaryKind::CONTINUE, s1
        ));
    }

    SECTION("Declare") {
        // a: Int
        REQUIRE(*interpreter.interpret(Comp(
            OpID::TYPE_LABEL,
            Comp(OpID::ID, "a", s1),
            Comp(OpID::ID, "Int", s3),
            s2
        )) == Binary(
            BinaryKind::DECLARE,
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
        )) == Binary(
            BinaryKind::DECLARE,
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
        // def f(): return 0
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
                        OpID::RETURN,
                        Comp(OpID::PLAIN_INT, "0", s7),
                        s6
                    ),
                    s5
                ),
                s1
            )),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                ArgSpec(
                    {},
                    {},
                    ArgDef(),
                    ArgDef()
                ),
                nullptr,
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                )))
            )),
            s1
        ));

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
                ArgSpec(
                    {},
                    {},
                    ArgDef(),
                    ArgDef()
                ),
                nullptr,
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s9)), s8
                )))
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
                ArgSpec(
                    {},
                    {},
                    ArgDef(),
                    ArgDef()
                ),
                NodePtr(new ID("Int", s6)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s11)), s10
                )))
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
                ArgSpec(
                    vec(ArgDef(ID("arg", s4), nullptr, nullptr, false)),
                    {},
                    ArgDef(),
                    ArgDef()
                ),
                NodePtr(new ID("Int", s7)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s12)), s11
                )))
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
                ArgSpec(
                    vec(
                        ArgDef(ID("arg1", s4), nullptr, nullptr, false),
                        ArgDef(
                            ID("arg2", s6),
                            NodePtr(new ID("Int", s8)),
                            nullptr,
                            false
                        )
                    ),
                    {},
                    ArgDef(),
                    ArgDef()
                ),
                NodePtr(new ID("Int", s11)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s16)), s15
                )))
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
                ArgSpec(
                    vec(
                        ArgDef(ID("arg1", s4), nullptr, nullptr, false),
                        ArgDef(
                            ID("arg2", s6),
                            NodePtr(new ID("Int", s8)),
                            nullptr,
                            false
                        )
                    ),
                    {},
                    ArgDef(ID("args", s11), nullptr, nullptr, false),
                    ArgDef()
                ),
                NodePtr(new ID("Int", s14)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s19)), s18
                )))
            )),
            s1
        ));

        // def f(arg1, arg2: Int, *args, kw1=1, kw2: Bool) -> Int:
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                ArgSpec(
                    vec(
                        ArgDef(ID("arg1", s4), nullptr, nullptr, false),
                        ArgDef(
                            ID("arg2", s6),
                            NodePtr(new ID("Int", s8)),
                            nullptr,
                            false
                        )
                    ),
                    vec(
                        ArgDef(
                            ID("kw1", s13),
                            nullptr,
                            NodePtr(new Int32(1, s15)),
                            false
                        ),
                        ArgDef(
                            ID("kw2", s17),
                            NodePtr(new ID("Bool", s19)),
                            nullptr,
                            false
                        )
                    ),
                    ArgDef(ID("args", s11), nullptr, nullptr, false),
                    ArgDef()
                ),
                NodePtr(new ID("Int", s22)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s27)), s26
                )))
            )),
            s1
        ));

        // def f(
        //     arg1, arg2: Int, *args, kw1=1, kw2: Bool, **kwargs
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
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(DefCase(
                ArgSpec(
                    vec(
                        ArgDef(ID("arg1", s4), nullptr, nullptr, false),
                        ArgDef(
                            ID("arg2", s6),
                            NodePtr(new ID("Int", s8)),
                            nullptr,
                            false
                        )
                    ),
                    vec(
                        ArgDef(
                            ID("kw1", s13),
                            nullptr,
                            NodePtr(new Int32(1, s15)),
                            false
                        ),
                        ArgDef(
                            ID("kw2", s17),
                            NodePtr(new ID("Bool", s19)),
                            nullptr,
                            false
                        )
                    ),
                    ArgDef(ID("args", s11), nullptr, nullptr, false),
                    ArgDef(ID("kwargs", s22), nullptr, nullptr, false)
                ),
                NodePtr(new ID("Int", s25)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s30)), s29
                )))
            )),
            s1
        ));

        // def f(a: Int, b: String):
        //     return 0
        // case (c: Float32, true) -> Int64:
        //     return 3s64
        // case (1, *, kw:: 2):
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
                        s28
                    ),
                    s34
                )
            ),
            s1
        )) == Def(
            NodePtr(new ID("f", s2)),
            vec(
                DefCase(
                    ArgSpec(
                        vec(
                            ArgDef(
                                ID("a", s4),
                                NodePtr(new ID("Int", s6)),
                                nullptr,
                                false
                            ),
                            ArgDef(
                                ID("b", s8),
                                NodePtr(new ID("String", s10)),
                                nullptr,
                                false
                            )
                        ),
                        {},
                        ArgDef(),
                        ArgDef()
                    ),
                    nullptr,
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s16)), s15
                    )))        
                ),
                DefCase(
                    ArgSpec(
                        vec(
                            ArgDef(
                                ID("c", s20),
                                NodePtr(new ID("Float32", s22)),
                                nullptr,
                                false
                            ),
                            ArgDef(
                                ID("", Pos(0, 0)),
                                NodePtr(new Bool(true, s24)),
                                nullptr,
                                true
                            )
                        ),
                        {},
                        ArgDef(),
                        ArgDef()
                    ),
                    NodePtr(new ID("Int64", s27)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int64(3, s32)), s31
                    )))
                ),
                DefCase(
                    ArgSpec(
                        vec(ArgDef(
                            ID("", Pos(0, 0)),
                            NodePtr(new Int32(1, s36)),
                            nullptr,
                            true
                        )),
                        vec(ArgDef(
                            ID("kw", s40),
                            NodePtr(new Int32(2, s42)),
                            nullptr,
                            true
                        )),
                        ArgDef(),
                        ArgDef()
                    ),
                    nullptr,
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s48)), s47
                    )))
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
                ArgSpec({}, {}, ArgDef(), ArgDef()),
                nullptr,
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s9)), s8
                )))
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

        // def f(a=1, b):
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
        )) == NonDefaultFollowsDefaultErr(s8));

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

        // def f(a=1, *args):
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
        )) == VarArgsFollowsDefaultErr(s8));

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

        // def f(*3):
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
        )) == ExpectedIDErr(s5));

        // def f(**3):
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
        )) == ExpectedIDErr(s5));

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

        // def f(a: Int, a: Float64):
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
        )) == DuplicateArgErr("a", s8));

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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("For") {
        // for x in c: a += x
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
        )) == For(
            vec(ID("x", s2)),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s6)),
                NodePtr(new ID("__iadd__", s7)),
                Args(vec(NodePtr(new ID("x", s8))), {}),
                false,
                s7
            ))),
            {},
            s1
        ));


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
            vec(ID("x", s2)),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s8)),
                NodePtr(new ID("__iadd__", s9)),
                Args(vec(NodePtr(new ID("x", s10))), {}),
                false,
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
            vec(ID("x", s2), ID("y", s4)),
            NodePtr(new ID("c", s6)),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s10)),
                NodePtr(new ID("__iadd__", s11)),
                Args(
                    vec(NodePtr(new CallAttr(
                        NodePtr(new ID("x", s12)),
                        NodePtr(new ID("__add__", s13)),
                        Args(vec(NodePtr(new ID("y", s14))), {}),
                        false,
                        s13
                    ))),
                    {}
                ),
                false,
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
            vec(ID("x", s2)),
            NodePtr(new ID("c", s4)),
            vec(NodePtr(new Nullary(NullaryKind::BREAK, s8))),
            vec(NodePtr(new CallAttr(
                NodePtr(new ID("a", s14)),
                NodePtr(new ID("__iadd__", s15)),
                Args(vec(NodePtr(new Int32(1, s16))), {}),
                false,
                s15
            ))),
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
        )) == Binary(
            BinaryKind::GET_ATTR,
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
        )) == Binary(
            BinaryKind::GET_ATTR,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("If") {
        // if x: return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == If(
            vec(Case(
                NodePtr(new ID("x", s2)),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s5)), s4
                )))
            )),
            {},
            s1
        ));

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
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                )))
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
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                    )))
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(1, s15)), s14
                    )))
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
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                    )))
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(1, s15)), s14
                    )))
                )
            ),
            vec(NodePtr(new Unary(
                UnaryKind::RETURN, NodePtr(new Int32(2, s22)), s21
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
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                )))
            )),
            vec(NodePtr(new Unary(
                UnaryKind::RETURN, NodePtr(new Int32(1, s14)), s13
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
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s7)), s6
                    )))
                ),
                Case(
                    NodePtr(new ID("y", s10)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(1, s15)), s14
                    )))
                ),
                Case(
                    NodePtr(new ID("z", s18)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(2, s23)), s22
                    )))
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
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
        )) == Unary(
            UnaryKind::LAMBDA,
            NodePtr(new CallAttr(
                NodePtr(new Unary(
                    UnaryKind::LAMBDA_VAR,
                    NodePtr(new NumID(1, s4)),
                    s3
                )),
                NodePtr(new ID("__add__", s5)),
                Args(
                    vec(NodePtr(new Unary(
                        UnaryKind::LAMBDA_VAR,
                        NodePtr(new NumID(2, s7)),
                        s6
                    ))),
                    {}
                ),
                false,
                s5
            )),
            s1
        ));

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

    SECTION("Lambda Var") {
        // %1
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA, Comp(OpID::PLAIN_INT, "1", s2), s1
        )) == Unary(
            UnaryKind::LAMBDA_VAR, NodePtr(new NumID(1, s2)), s1
        ));

        // %a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LAMBDA, Comp(OpID::ID, "a", s2), s1
        )) == Unary(
            UnaryKind::LAMBDA_VAR, NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("List") {
        // []
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST, Comp(OpID::NOTHING, s2), s1
        )) == Seq(
            SeqKind::LIST, {}, s1
        ));

        // [a]
        REQUIRE(*interpreter.interpret(Comp(
            OpID::LIST, Comp(OpID::ID, "a", s2), s1
        )) == Seq(
            SeqKind::LIST, vec(NodePtr(new ID("a", s2))), s1
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
        )) == Seq(
            SeqKind::LIST,
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
        )) == Seq(
            SeqKind::LIST,
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Unary(
                    UnaryKind::EXPANSION, NodePtr(new ID("args", s5)), s4
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("Map") {
        // {}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE, Comp(OpID::NOTHING, s2), s1
        )) == Seq(
            SeqKind::MAP, {}, s1
        ));

        // {a: 1}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            Comp(
                OpID::TYPE_LABEL,
                Comp(OpID::ID, "a", s2),
                Comp(OpID::PLAIN_INT, "1", s4),
                s3
            ),
            s1
        )) == Seq(
            SeqKind::MAP,
            vec(NodePtr(new Binary(
                BinaryKind::ENTRY,
                NodePtr(new ID("a", s2)),
                NodePtr(new Int32(1, s4)),
                s3
            ))),
            s1
        ));

        // {a: 1, b: 2, c: 3}
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Seq(
            SeqKind::MAP,
            vec(
                NodePtr(new Binary(
                    BinaryKind::ENTRY,
                    NodePtr(new ID("a", s2)),
                    NodePtr(new Int32(1, s4)),
                    s3
                )),
                NodePtr(new Binary(
                    BinaryKind::ENTRY,
                    NodePtr(new ID("b", s6)),
                    NodePtr(new Int32(2, s8)),
                    s7
                )),
                NodePtr(new Binary(
                    BinaryKind::ENTRY,
                    NodePtr(new ID("c", s10)),
                    NodePtr(new Int32(3, s12)),
                    s11
                ))
            ),
            s1
        ));

        // {a: 1, **kwargs, b: 2}
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Seq(
            SeqKind::MAP,
            vec(
                NodePtr(new Binary(
                    BinaryKind::ENTRY,
                    NodePtr(new ID("a", s2)),
                    NodePtr(new Int32(1, s4)),
                    s3
                )),
                NodePtr(new Unary(
                    UnaryKind::EXPANSION, NodePtr(new ID("kwargs", s7)), s6
                )),
                NodePtr(new Binary(
                    BinaryKind::ENTRY,
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
        // case Int{%y}: return y
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(Case(
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("Int", s5)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("y", s8)),
                            s7
                        ))),
                        {}
                    ),
                    s6
                )),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new ID("y", s12)), s11
                )))
            )),
            {},
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(Case(
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("Int", s5)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("y", s8)),
                            s7
                        ))),
                        {}
                    ),
                    s6
                )),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new ID("y", s14)), s13
                )))
            )),
            {},
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        // else:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(Case(
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("Int", s5)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("y", s8)),
                            s7
                        ))),
                        {}
                    ),
                    s6
                )),
                vec(NodePtr(new Unary(
                    UnaryKind::RETURN, NodePtr(new ID("y", s14)), s13
                )))
            )),
            vec(NodePtr(new Unary(
                UnaryKind::RETURN, NodePtr(new Int32(0, s21)), s20
            ))),
            s1
        ));

        // match x
        // case Int{%y}:
        //     return y
        // case false:
        //     return 0
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Match(
            NodePtr(new ID("x", s2)),
            vec(
                Case(
                    NodePtr(new Call(
                        CallKind::MATCH,
                        NodePtr(new ID("Int", s5)),
                        Args(
                            vec(NodePtr(new Unary(
                                UnaryKind::LAMBDA_VAR,
                                NodePtr(new ID("y", s8)),
                                s7
                            ))),
                            {}
                        ),
                        s6
                    )),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new ID("y", s14)), s13
                    )))
                ),
                Case(
                    NodePtr(new Bool(false, s17)),
                    vec(NodePtr(new Unary(
                        UnaryKind::RETURN, NodePtr(new Int32(0, s22)), s21
                    )))
                )
            ),
            {},
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
        // case Int{%y}
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == MissingBodyErr(s6));

        // match x
        // elif Int{%y}:
        //     return y
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == ExpectedCaseOrElseErr(s4));
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
            Args({}, {}),
            false,
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
        )) == Binary(
            BinaryKind::OR,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("Raise") {
        // raise
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RAISE, Comp(OpID::NOTHING, s_end), s1
        )) == Unary(
            UnaryKind::RAISE, nullptr, s1
        ));

        // raise a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RAISE, Comp(OpID::ID, "a", s2), s1
        )) == Unary(
            UnaryKind::RAISE, NodePtr(new ID("a", s2)), s1
        ));
    }

    SECTION("Return") {
        // return
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RETURN, Comp(OpID::NOTHING, s_end), s1
        )) == Unary(
            UnaryKind::RETURN, nullptr, s1
        ));

        // return a
        REQUIRE(*interpreter.interpret(Comp(
            OpID::RETURN, Comp(OpID::ID, "a", s2), s1
        )) == Unary(
            UnaryKind::RETURN, NodePtr(new ID("a", s2)), s1
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
            Args(vec(NodePtr(new ID("b", s3))), {}),
            false,
            s2
        ));
    }

    SECTION("Set") {
        // {a}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            Comp(OpID::ID, "a", s2),
            s1
        )) == Seq(SeqKind::SET, vec(NodePtr(new ID("a", s2))), s1));

        // {a, b, c}
        REQUIRE(*interpreter.interpret(Comp(
            OpID::ENCLOSURE,
            csv(
                Comp(OpID::ID, "a", s2),
                Comp(OpID::ID, "b", s4),
                Comp(OpID::ID, "c", s6)
            ),
            s1
        )) == Seq(
            SeqKind::SET,
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
        )) == Seq(
            SeqKind::SET,
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Unary(
                    UnaryKind::EXPANSION, NodePtr(new ID("args", s5)), s4
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
        )) == Unary(UnaryKind::SYMBOL, NodePtr(new ID("x", s2)), s1));

        // :3
        REQUIRE(*interpreter.interpret(Comp(
            OpID::SYMBOL,
            Comp(OpID::PLAIN_INT, "3", s2),
            s1
        )) == Unary(UnaryKind::SYMBOL, NodePtr(new NumID(3, s2)), s1));
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
        )) == Nullary(NullaryKind::THIS, s1));
    }

    SECTION("Try") {
        // try: f()
        // except E{%e}: g()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == Try(
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("f", s3)), Args({}, {}), s4
            ))),
            vec(Case{
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("E", s8)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("e", s11)),
                            s10
                        ))),
                        {}
                    ),
                    s9
                )),
                vec(NodePtr(new Call(
                    CallKind::CALL,
                    NodePtr(new ID("g", s14)),
                    Args({}, {}),
                    s15
                )))
            }),
            {},
            s1
        ));

        // try:
        //     f()
        // except E{%e}:
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
        )) == Try(
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("f", s5)), Args({}, {}), s6
            ))),
            vec(Case{
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("E", s10)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("e", s13)),
                            s12
                        ))),
                        {}
                    ),
                    s11
                )),
                vec(NodePtr(new Call(
                    CallKind::CALL, NodePtr(new ID("g", s18)), Args({}, {}), s19
                )))
            }),
            {},
            s1
        ));

        // try:
        //     f()
        // except E{%e}:
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
        )) == Try(
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("f", s5)), Args({}, {}), s6
            ))),
            vec(Case{
                NodePtr(new Call(
                    CallKind::MATCH,
                    NodePtr(new ID("E", s10)),
                    Args(
                        vec(NodePtr(new Unary(
                            UnaryKind::LAMBDA_VAR,
                            NodePtr(new ID("e", s13)),
                            s12
                        ))),
                        {}
                    ),
                    s11
                )),
                vec(NodePtr(new Call(
                    CallKind::CALL, NodePtr(new ID("g", s18)), Args({}, {}), s19
                )))
            }),
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("h", s26)), Args({}, {}), s27
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
                CallKind::CALL, NodePtr(new ID("f", s5)), Args({}, {}), s6
            ))),
            {},
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("g", s13)), Args({}, {}), s14
            ))),
            s1
        ));

        // try:
        //     f()
        // except E1{%e}:
        //     g()
        // except E2{%e}:
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
        )) == Try(
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("f", s5)), Args({}, {}), s6
            ))),
            vec(
                Case(
                    NodePtr(new Call(
                        CallKind::MATCH,
                        NodePtr(new ID("E1", s10)),
                        Args(
                            vec(NodePtr(new Unary(
                                UnaryKind::LAMBDA_VAR,
                                NodePtr(new ID("e", s13)),
                                s12
                            ))),
                            {}
                        ),
                        s11
                    )),
                    vec(NodePtr(new Call(
                        CallKind::CALL,
                        NodePtr(new ID("g", s18)),
                        Args({}, {}),
                        s19
                    )))
                ),
                Case(
                    NodePtr(new Call(
                        CallKind::MATCH,
                        NodePtr(new ID("E2", s23)),
                        Args(
                            vec(NodePtr(new Unary(
                                UnaryKind::LAMBDA_VAR,
                                NodePtr(new ID("e", s26)),
                                s25
                            ))),
                            {}
                        ),
                        s24
                    )),
                    vec(NodePtr(new Call(
                        CallKind::CALL,
                        NodePtr(new ID("h", s31)),
                        Args({}, {}),
                        s32
                    )))
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
        // except E{%e}:
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
        // except E{%e}
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
        )) == MissingBodyErr(s11));
    }

    SECTION("Tuple") {
        // ()
        REQUIRE(*interpreter.interpret(Comp(
            OpID::GROUP, Comp(OpID::NOTHING, s2), s1
        )) == Seq(
            SeqKind::TUPLE, {}, s1
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
        )) == Seq(
            SeqKind::TUPLE,
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
        )) == Seq(
            SeqKind::TUPLE,
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
        )) == Seq(
            SeqKind::TUPLE,
            vec(
                NodePtr(new ID("a", s2)),
                NodePtr(new Unary(
                    UnaryKind::EXPANSION, NodePtr(new ID("args", s5)), s4
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
        )) == Seq(
            SeqKind::TUPLE,
            vec(
                NodePtr(new ID("a", s1)),
                NodePtr(new Unary(
                    UnaryKind::EXPANSION, NodePtr(new ID("args", s4)), s3
                )),
                NodePtr(new ID("b", s6))
            ),
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
            Args(vec(NodePtr(new ID("i", s3))), {}),
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
            Args(vec(NodePtr(new ID("i", s5))), {}),
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
            Args(vec(NodePtr(new ID("i", s5))), {}),
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

    SECTION("While") {
        // while a: f()
        REQUIRE(*interpreter.interpret(Comp(
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
        )) == While(
            NodePtr(new ID("a", s2)),
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("f", s4)), Args({}, {}), s5
            ))),
            {},
            s1
        ));

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
                CallKind::CALL, NodePtr(new ID("f", s6)), Args({}, {}), s7
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
                CallKind::CALL, NodePtr(new ID("f", s6)), Args{{}, {}}, s7
            ))),
            vec(NodePtr(new Call(
                CallKind::CALL, NodePtr(new ID("g", s14)), Args({}, {}), s15
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
