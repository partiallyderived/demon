#pragma once

#include <iostream>

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>

#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/convert.hpp"
#include "dl/err.hpp"
#include "dl/interpret/args.hpp"
#include "dl/interpret/binary.hpp"
#include "dl/interpret/block.hpp"
#include "dl/interpret/call.hpp"
#include "dl/interpret/case.hpp"
#include "dl/interpret/counted.hpp"
#include "dl/interpret/data.hpp"
#include "dl/interpret/def.hpp"
#include "dl/interpret/defcase.hpp"
#include "dl/interpret/enterobject.hpp"
#include "dl/interpret/errornode.hpp"
#include "dl/interpret/errorwithcomp.hpp"
#include "dl/interpret/for.hpp"
#include "dl/interpret/if.hpp"
#include "dl/interpret/init.hpp"
#include "dl/interpret/interpreter.hpp"
#include "dl/interpret/keywordarg.hpp"
#include "dl/interpret/match.hpp"
#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/matchobject.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nullary.hpp"
#include "dl/interpret/seq.hpp"
#include "dl/interpret/ternary.hpp"
#include "dl/interpret/try.hpp"
#include "dl/interpret/unary.hpp"
#include "dl/interpret/unexpectedblock.hpp"
#include "dl/interpret/unexpectedconstruct.hpp"
#include "dl/interpret/while.hpp"
#include "dl/lex/literalsuffix.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"

namespace dl {

struct ExpectedArgExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedArgExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedArgExprErr";
    }
};

struct ExpectedArgMatchExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedArgMatchExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedArgMatchExprErr";
    }
};

struct ExpectedAsLHSExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedAsLHSExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedAsLHSExprErr";
    }
};

struct ExpectedCallErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedCallErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCallErr";
    }
};

struct ExpectedCaseErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedCaseErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCaseErr";
    }
};

struct ExpectedCompoundIDErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedCompoundIDErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCompoundIDErr";
    }
};

struct ExpectedDefaultableExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedDefaultableExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedDefaultableExprErr";
    }
};

struct ExpectedElementExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedElementExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElementExprErr";
    }
};

struct ExpectedElementMatchExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedElementMatchExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElementMatchExprErr";
    }
};

struct ExpectedElifOrElseErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedElifOrElseErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElifOrElseErr";
    }
};

struct ExpectedElseErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedElseErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElseErr";
    }
};

struct ExpectedEntryExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedEntryExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedEntryExprErr";
    }
};

struct ExpectedEntryMatchExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedEntryMatchExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedEntryMatchExprErr";
    }
};

struct ExpectedExceptOrFinallyErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedExceptOrFinallyErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExceptOrFinallyErr";
    }
};

struct ExpectedExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExprErr";
    }
};

struct ExpectedGeneralIDErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedGeneralIDErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedGeneralIDErr";
    }
};

struct ExpectedIDErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedIDErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDErr";
    }
};

struct ExpectedIDOrCallErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedIDOrCallErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrCallErr";
    }
};

struct ExpectedIDOrVarArgsErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedIDOrVarArgsErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrVarArgsErr";
    }
};

struct ExpectedInErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedInErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedInErr";
    }
};

struct ExpectedKeywordArgExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedKeywordArgExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedKeywordArgExprErr";
    }
};

struct ExpectedKeywordArgMatchExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedKeywordArgMatchExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedKeywordArgMatchExprErr";
    }
};

struct ExpectedMatchExprErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedMatchExprErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedMatchExprErr";
    }
};

struct ExpectedParentheticalErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedParentheticalErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedParentheticalErr";
    }
};

struct ExpectedStringErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ExpectedStringErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedStringErr";
    }
};

struct MissingArgSpecErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new MissingArgSpecErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingArgSpecErr";
    }
};

struct MissingBodyErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new MissingBodyErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingBodyErr";
    }
};

struct MissingIDErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new MissingIDErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingIDErr";
    }
};

struct MissingPredicateErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new MissingPredicateErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingPredicateErr";
    }
};

struct NoExceptsOrFinallyErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new NoExceptsOrFinallyErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "NoExceptsOrFinallyErr";
    }
};

struct NoMatchCasesErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new NoMatchCasesErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "NoMatchCasesErr";
    }
};

struct UnexpectedBlocksErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new UnexpectedBlocksErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedBlocksErr";
    }
};

struct UnexpectedPredicateErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new UnexpectedPredicateErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedPredicateErr";
    }
};

struct ZeroUsedAsIDErr final: Err {
    virtual ErrPtr copy() const override {
        return ErrPtr(new ZeroUsedAsIDErr(*this));
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ZeroUsedAsIDErr";
    }
};

enum class MatchKind {
    DEF,
    OBJECT,
    PLAIN,
    MAP,
    SEQ
};

struct InterpreterImpl final: Interpreter {
    static inline NodePtr interpret_(Comp&& comp);
    NodePtr interpret(Comp&& comp) override {
        return interpret_(std::move(comp));
    }

    static inline std::tuple<std::uint32_t, Comp, Span>
        count_op(Comp&& comp, OpID op);
    static inline NodePtr expect_body_only(Comp&& comp);
    static inline NodePtr expect_id(Comp&& comp);
    static inline NodePtr interpret_assign(Comp&& comp);
    static inline NodePtr interpret_arg(Comp&& comp);
    static inline NodePtr interpret_arg_matcher(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_args(Comp&& comp);
    static inline Args interpret_args_recurse(
        Comp&& comp, bool& in_kw_part
    );

    template<typename ToType>
    static inline NodePtr interpret_binop(Comp&& comp);

    static inline NodePtr interpret_binop_method(
        Comp&& comp, std::string fn_name
    );
    static inline NodePtr interpret_block(Comp&& comp);
    static inline NodePtr interpret_block(Comp&& comp, Span label_src);
    static inline NodePtr interpret_body(Comp&& comp);
    static inline NodePtr interpret_break(Comp&& comp);
    static inline NodePtr interpret_call(Comp&& comp);
    static inline NodePtr interpret_case(Comp&& comp);
    static inline Nodes interpret_code(Comp&& comp);
    static inline NodePtr interpret_concat(Comp&& comp);
    static inline NodePtr interpret_construct(std::vector<Comp>&& comps);
    static inline NodePtr interpret_continue(Comp&& comp);
    static inline Nodes interpret_csv(Comp&& comp);

    template<typename ToType>
    static inline NodePtr interpret_data(Comp&& comp);

    static inline NodePtr interpret_declare(Comp&& comp);
    static inline NodePtr interpret_def(std::vector<Comp>&& comps);
    static inline NodePtr interpret_def_case(Comp&& comp);
    static inline std::pair<NodePtr, NodePtr> interpret_def_case_post(
        Comp&& comp, Comp&& args_comp
    );
    static inline std::tuple<Comp*, NodePtr, NodePtr>
        interpret_def_case_pre(Comp& comp);
    static inline NodePtr interpret_definable(Comp&& comp);
    static inline NodePtr interpret_dflt(Comp&& comp);
    static inline NodePtr interpret_enclosure(Comp&& comp);
    static inline Nodes interpret_enclosure_recurse(Comp&& comp);
    static inline NodePtr interpret_entry(Comp&& comp);
    static inline NodePtr interpret_error(Comp&& comp);
    static inline NodePtr interpret_expr(Comp&& comp);
    static inline std::pair<NodePtr, NodePtr> interpret_first_def_case(
        Comp&& comp
    );
    static inline NodePtr interpret_float_tail(Comp&& comp);
    static inline NodePtr interpret_for(std::vector<Comp>&& comps);
    static inline std::tuple<Nodes, NodePtr> interpret_for_pred(Comp&& comp);
    static inline NodePtr interpret_general_id(Comp&& comp);
    static inline NodePtr interpret_get(Comp&& comp);
    static inline NodePtr interpret_group(Comp&& comp);
    static inline NodePtr interpret_id(Comp&& comp);
    static inline NodePtr interpret_if(std::vector<Comp>&& comps);
    static inline NodePtr interpret_kwarg(Comp&& comp);
    static inline NodePtr interpret_label(Comp&& comp);
    static inline NodePtr interpret_lambda(Comp&& comp);

    template<typename NodeType>
    static inline NodePtr interpret_lambda_var_args(Comp&& comp);

    static inline NodePtr interpret_literally(Comp&& comp);
    static inline Nodes interpret_loop_vars(Comp&& comp);
    static inline NodePtr interpret_map_element(Comp&& comp);
    static inline NodePtr interpret_match(std::vector<Comp>&& comps);
    static inline NodePtr interpret_match_args(Comp&& comp, bool is_def);
    static inline MatchArgs interpret_match_args_recurse(
        Comp&& comp, bool& in_kw_part, bool is_def
    );
    static inline NodePtr interpret_match_as(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_match_bind(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_match_case(Comp&& comp);
    static inline Nodes interpret_match_cases(
        std::vector<Comp>&& comps, std::uint32_t end, OpID op
    );
    static inline std::tuple<NodePtr, NodePtr> interpret_match_case_pred(
        Comp&& comp
    );
    static inline NodePtr interpret_match_general_var(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_match_kwarg(Comp&& comp);
    static inline NodePtr interpret_match_list(Comp&& comp);
    static inline NodePtr interpret_match_map(Comp&& comp);
    static inline Nodes interpret_match_map_recurse(Comp&& comp);
    static inline NodePtr interpret_match_obj(Comp&& comp);
    static inline NodePtr interpret_match_options(Comp&& comp);
    static inline Nodes interpret_match_options_recurse(Comp&& comp);
    static inline Nodes interpret_match_seq_recurse(Comp&& comp);
    static inline NodePtr interpret_match_tuple(Comp&& comp);
    static inline NodePtr interpret_match_var_args(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_match_var_kwargs(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_matcher(Comp&& comp, MatchKind kind);
    static inline NodePtr interpret_not_in(Comp&& comp);
    static inline NodePtr interpret_num_id(Comp&& comp);
    static inline NodePtr interpret_number(Comp&& comp);

    template<typename ToType>
    static inline NodePtr interpret_optional(Comp&& comp);

    static inline NodePtr interpret_optional_else(std::vector<Comp>&& comps);
    static inline NodePtr interpret_plain_int(Comp&& comp);
    static inline NodePtr interpret_sep(Comp&& comp);

    template<typename ToType>
    static inline NodePtr interpret_seq(Comp&& comp);

    static inline NodePtr interpret_symbol(Comp&& comp);
    static inline NodePtr interpret_ternary(Comp&& comp);
    static inline NodePtr interpret_try(std::vector<Comp>&& comps);
    static inline NodePtr interpret_type(Comp&& comp);
    static inline NodePtr interpret_type_match(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodePtr interpret_unexpected_construct(Comp&& comp);

    template<typename ToType>
    static inline NodePtr interpret_unop(Comp&& comp);

    static inline NodePtr interpret_unop_method(
        Comp&& comp, std::string fn_name
    );
    static inline NodePtr interpret_up(Comp&& comp);
    static inline NodePtr interpret_while(std::vector<Comp>&& comps);
    static inline bool is_map(const Nodes& nodes);
    static inline NodePtr parse_float(
        const std::string& str, Comp&& comp, Span src
    );
};

std::tuple<std::uint32_t, Comp, Span> InterpreterImpl::count_op(
    Comp&& comp, OpID op
) {
    std::uint32_t count = 0;
    Span start = comp.src;
    Span end = start;
    Comp* c = &comp;
    while (c->op == op) {
        count++;
        end = c->src;
        c = c->comp;
    }
    return {count, std::move(*c), Span(start, end)};
}

NodePtr InterpreterImpl::expect_body_only(Comp&& comp) {
    if (comp.comp->op != OpID::BODY) {
        if (comp.comp->op == OpID::LABEL) {
            return NodePtr(new Case(
                NodePtr(new ErrorWithComp(
                    ErrPtr(new UnexpectedPredicateErr()),
                    std::move(comp.comp->bin->lhs)
                )),
                interpret_block(std::move(comp.comp->bin->rhs)),
                comp.src
            ));
        }
        return NodePtr(new Case(
            NodePtr(new ErrorWithComp(
                ErrPtr(new UnexpectedPredicateErr()),
                std::move(*comp.comp)
            )),
            NodePtr(new ErrorWithComp(
                ErrPtr(new MissingBodyErr()),
                Comp(OpID::MISSING, Span::after(comp.comp->span()))
            )),
            comp.src
        ));
    }
    return interpret_block(std::move(*comp.comp->comp), comp.src);
}

NodePtr InterpreterImpl::expect_id(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ID:
        return interpret_id(std::move(comp));
    case ERROR:
        return interpret_error(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedIDErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BODY:
        return interpret_body(std::move(comp));
    case BREAK:
        return interpret_break(std::move(comp));
    case CONSTRUCT:
        return interpret_construct(std::move(comp.comps));
    case CONTINUE:
        return interpret_continue(std::move(comp));
    case IADD:
        return interpret_binop<IAdd>(std::move(comp));
    case IBAND:
        return interpret_binop<IBitAnd>(std::move(comp));
    case IBOR:
        return interpret_binop<IBitOr>(std::move(comp));
    case IBXOR:
        return interpret_binop<IBitXor>(std::move(comp));
    case IDIV:
        return interpret_binop<IDivide>(std::move(comp));
    case ILSH:
        return interpret_binop<ILeftShift>(std::move(comp));
    case IMOD:
        return interpret_binop<IMod>(std::move(comp));
    case IMUL:
        return interpret_binop<IMultiply>(std::move(comp));
    case IPOW:
        return interpret_binop<IPower>(std::move(comp));
    case IRSH:
        return interpret_binop<IRightShift>(std::move(comp));
    case ISUB:
        return interpret_binop<ISubtract>(std::move(comp));
    case LABEL:
        return interpret_label(std::move(comp));
    case RAISE:
        return interpret_optional<Raise>(std::move(comp));
    case RETURN:
        return interpret_optional<Return>(std::move(comp));
    case SET:
        return interpret_assign(std::move(comp));
    case TYPE_LABEL:
        return interpret_declare(std::move(comp));
    case CASE:
    case ELIF:
    case ELSE:
    case EXCEPT:
    case FINALLY:
        return interpret_unexpected_construct(std::move(comp));
    default:
        return interpret_expr(std::move(comp));
    }
}

NodePtr InterpreterImpl::interpret_arg(Comp&& comp) {
    if (comp.op == OpID::UNPACK_ARGS)
        return interpret_unop<Expansion>(std::move(comp));
    return interpret_expr(std::move(comp));
}

NodePtr InterpreterImpl::interpret_arg_matcher(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    using enum OpID;
    switch(comp.op) {
    case AS:
        return interpret_match_as(std::move(comp), kind, in_kw_part);
    case BIND:
        return interpret_match_bind(std::move(comp), kind, in_kw_part);
    case CALL:
        return interpret_match_obj(std::move(comp));
    case ENCLOSURE:
        return interpret_match_map(std::move(comp));
    case GROUP:
        return interpret_match_tuple(std::move(comp));
    case LIST:
        return interpret_match_list(std::move(comp));
    case LITERALLY:
        return interpret_expr(std::move(*comp.comp));
    case MATCH_OR:
        return interpret_match_options(std::move(comp));
    case PLACEHOLDER:
        return NodePtr(new Placeholder(comp.src));
    case TYPE_LABEL:
        return interpret_type_match(std::move(comp), kind, in_kw_part);
    case UNPACK_ARGS:
        return interpret_match_var_args(std::move(comp), kind, in_kw_part);
    case UNPACK_KWARGS:
        return interpret_match_var_kwargs(std::move(comp), kind, in_kw_part);
    default:
        return interpret_expr(std::move(comp));
    }
}

NodePtr InterpreterImpl::interpret_args(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new Args({}, {}, comp.src));

    bool in_kw_part = false;
    Args args = interpret_args_recurse(std::move(*comp.comp), in_kw_part);
    args.src = comp.src;
    return NodePtr(new Args(std::move(args)));
}

Args InterpreterImpl::interpret_args_recurse(Comp&& comp, bool& in_kw_part) {
    auto args = Args({}, {}, Span(0, 0, 0, 0));
    Comp* c = &comp;
    if (comp.op == OpID::SEP) {
        args = interpret_args_recurse(std::move(comp.bin->lhs), in_kw_part);
        c = &comp.bin->rhs;
    }

    in_kw_part =
        in_kw_part || c->op == OpID::BIND || c->op == OpID::UNPACK_KWARGS;
    if (in_kw_part)
        args.kwargs.push_back(interpret_kwarg(std::move(*c)));
    else
        args.args.push_back(interpret_arg(std::move(*c)));
    return args;
}

NodePtr InterpreterImpl::interpret_assign(Comp&& comp) {
    NodePtr rhs = interpret_expr(std::move(comp.bin->rhs));

    if (comp.bin->lhs.op == OpID::TYPE_LABEL) {
        // Variable initialization
        return NodePtr(new Init(
            interpret_definable(std::move(comp.bin->lhs.bin->lhs)),
            interpret_expr(std::move(comp.bin->lhs.bin->rhs)),
            std::move(rhs),
            comp.src
        ));
    }
    NodePtr lhs = interpret_expr(std::move(comp.bin->lhs));
    return NodePtr(
        new Assign(std::move(lhs), std::move(rhs), comp.src)
    );
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_binop(Comp&& comp) {
    return NodePtr(new ToType(
        interpret_expr(std::move(comp.bin->lhs)),
        interpret_expr(std::move(comp.bin->rhs)),
        comp.src
    ));
}

NodePtr InterpreterImpl::interpret_block(Comp&& comp) {
    return interpret_block(std::move(comp), comp.src);
}

NodePtr InterpreterImpl::interpret_block(Comp&& comp, Span src) {
    return NodePtr(new Block(interpret_code(std::move(comp)), src));
}

NodePtr InterpreterImpl::interpret_body(Comp&& comp) {
    return NodePtr(new UnexpectedBlock(
        interpret_code(std::move(*comp.comp)), comp.src
    ));
}

NodePtr InterpreterImpl::interpret_break(Comp&& comp) {
    return NodePtr(new Break(comp.src));
}

NodePtr InterpreterImpl::interpret_call(Comp&& comp) {
    if (comp.bin->rhs.op == OpID::ENCLOSURE) {
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedParentheticalErr()),
            std::move(comp)
        ));
    }
    NodePtr callee = interpret_expr(std::move(comp.bin->lhs));
    NodePtr args = interpret_args(std::move(comp.bin->rhs));

    if (comp.bin->rhs.op == OpID::LIST)
        return NodePtr(new CachedCall(
            std::move(callee), std::move(args), comp.src
        ));

    return NodePtr(new Call(
        std::move(callee), std::move(args), comp.src
    ));
}

NodePtr InterpreterImpl::interpret_case(Comp&& comp) {
    using enum OpID;
    switch(comp.comp->op) {
    case BODY:
        return NodePtr(new Case(
            NodePtr(new ErrorWithComp(
                ErrPtr(new MissingPredicateErr()),
                Comp(OpID::MISSING, comp.comp->src)
            )),
            interpret_block(std::move(*comp.comp->comp)),
            comp.src
        ));
    case LABEL:
        return NodePtr(new Case(
            interpret_expr(std::move(comp.comp->bin->lhs)),
            interpret_block(std::move(comp.comp->bin->rhs)),
            comp.src
        ));
    default: {
        Span after_pred = Span::after(comp.comp->span());
        return NodePtr(new Case(
            interpret_expr(std::move(*comp.comp)),
            NodePtr(new ErrorWithComp(
                ErrPtr(new MissingBodyErr()),
                Comp(OpID::MISSING, after_pred)
            )),
            comp.src
        ));
    }}
}

Nodes InterpreterImpl::interpret_code(Comp&& comp) {
    Nodes code;
    for(Comp& comp: comp.comps)
        code.push_back(interpret_(std::move(comp)));
    return code;
}

NodePtr InterpreterImpl::interpret_construct(std::vector<Comp>&& comps) {
    using enum OpID;
    switch(comps[0].op) {
    case DEF:
        return interpret_def(std::move(comps));
    case FOR:
        return interpret_for(std::move(comps));
    case IF:
        return interpret_if(std::move(comps));
    case MATCH:
        return interpret_match(std::move(comps));
    case TRY:
        return interpret_try(std::move(comps));
    case WHILE:
        return interpret_while(std::move(comps));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new AssertionFailedErr("Expected Construct")),
            std::move(comps[0])
        ));
    }
}

NodePtr InterpreterImpl::interpret_continue(Comp&& comp) {
    return NodePtr(new Continue(comp.src));
}

Nodes InterpreterImpl::interpret_csv(Comp&& comp) {
    Nodes nodes;
    Comp* next;

    if (comp.op == OpID::SEP) {
        nodes = interpret_csv(std::move(comp.bin->lhs));
        next = &comp.bin->rhs;
    } else
        next = &comp;

    nodes.push_back(interpret_arg(std::move(*next)));
    return nodes;
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_data(Comp&& comp) {
    return NodePtr(new ToType(
        std::get<typename ToType::Type>(std::move(comp.data)), comp.src
    ));
}

NodePtr InterpreterImpl::interpret_declare(Comp&& comp) {
    return NodePtr(new Declare(
        interpret_definable(std::move(comp.bin->lhs)),
        interpret_expr(std::move(comp.bin->rhs)),
        comp.src
    ));
}

NodePtr InterpreterImpl::interpret_def(std::vector<Comp>&& comps) {
    std::pair<NodePtr, NodePtr> first = interpret_first_def_case(
        std::move(comps[0])
    );

    Nodes cases;
    cases.push_back(std::move(first.second));
    for (std::uint32_t i = 1; i < comps.size(); i++) {
        if (comps[i].op != OpID::CASE)
            cases.push_back(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedCaseErr()),
                std::move(comps[i]),
                comps[i].src
            )));
        else
            cases.push_back(interpret_def_case(std::move(comps[i])));
    }
    return NodePtr(new Def(
        std::move(first.first), std::move(cases), comps[0].src
    ));
}

NodePtr InterpreterImpl::interpret_def_case(Comp&& comp) {
    using enum OpID;
    switch(comp.comp->op) {
    case LABEL: {
        auto [pred_comp, returns, guard] =
            interpret_def_case_pre(comp.comp->bin->lhs);
        return NodePtr(new DefCase(
            interpret_match_args(std::move(*pred_comp), true),
            std::move(guard),
            std::move(returns),
            interpret_block(std::move(comp.comp->bin->rhs)),
            comp.src
        ));
    }
    case BODY:
        return NodePtr(new DefCase(
            NodePtr(new ErrorWithComp(
                ErrPtr(new MissingArgSpecErr()),
                Comp(OpID::MISSING, comp.comp->src)
            )),
            nullptr,
            nullptr,
            interpret_block(std::move(*comp.comp->comp)),
            comp.src
        ));
    default: {
        Span after_pred = Span::after(comp.comp->span());
        auto [pred_comp, returns, guard] = interpret_def_case_pre(*comp.comp);
        return NodePtr(new DefCase(
            interpret_match_args(std::move(*pred_comp), true),
            std::move(guard),
            std::move(returns),
            NodePtr(new ErrorWithComp(
                ErrPtr(new MissingBodyErr()),
                Comp(OpID::MISSING, after_pred)
            )),
            comp.src
        ));
    }}
}

std::tuple<Comp*, NodePtr, NodePtr> InterpreterImpl::interpret_def_case_pre(
    Comp& comp
) {
    NodePtr returns = nullptr;
    Comp* pred_comp = &comp;
    if (pred_comp->op == OpID::RETURNS) {
        returns = interpret_expr(std::move(pred_comp->bin->rhs));
        pred_comp = &pred_comp->bin->lhs;
    }

    NodePtr guard = nullptr;
    if (pred_comp->op == OpID::TERNARY_IF) {
        guard = interpret_expr(std::move(pred_comp->bin->rhs));
        pred_comp = &pred_comp->bin->lhs;
    }

    return std::tuple(pred_comp, std::move(returns), std::move(guard));
}

NodePtr InterpreterImpl::interpret_definable(Comp&& comp) {
    if (comp.op == OpID::GET) {
        NodePtr lhs = comp.bin->lhs.op == OpID::PLAIN_INT ?
            interpret_num_id(std::move(comp)):
            interpret_expr(std::move(comp));

        NodePtr rhs = interpret_general_id(std::move(comp));

        return NodePtr(new GetAttr(
            std::move(lhs), std::move(rhs), comp.src
        ));
    }
    return interpret_general_id(std::move(comp));
}

NodePtr InterpreterImpl::interpret_dflt(Comp&& comp) {
    NodePtr lhs;

    using enum OpID;
    switch(comp.bin->lhs.op) {
    case BIND:
    case UNPACK_ARGS:
    case UNPACK_KWARGS:
        lhs = NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedDefaultableExprErr()),
            std::move(comp.bin->lhs)
        ));
        break;
    default:;
        lhs = interpret_matcher(std::move(comp.bin->lhs), MatchKind::DEF);
    }
    NodePtr rhs = interpret_expr(std::move(comp.bin->rhs));
    return NodePtr(new Defaulted(std::move(lhs), std::move(rhs), comp.src));
}

NodePtr InterpreterImpl::interpret_enclosure(Comp&& comp) {
    Nodes elements = interpret_enclosure_recurse(
        std::move(*comp.comp)
    );
    if (is_map(elements))
        return NodePtr(new Map(std::move(elements), comp.src));
    return NodePtr(new Set(std::move(elements), comp.src));
}

Nodes InterpreterImpl::interpret_enclosure_recurse(Comp&& comp) {
    using enum OpID;

    auto elements = Nodes();
    NodePtr element;
    switch(comp.op) {
    case SEP:
        elements = interpret_enclosure_recurse(std::move(comp.bin->lhs));
        if (is_map(elements))
            element = interpret_map_element(std::move(comp.bin->rhs));
        else
            element = interpret_arg(std::move(comp.bin->rhs));
        break;
    case ENTRY:
    case UNPACK_KWARGS:
        element = interpret_map_element(std::move(comp));
        break;
    case NOTHING:
        return Nodes();
    case UNPACK_ARGS:
    default:
        element = interpret_arg(std::move(comp));
        break;
    }
    elements.push_back(std::move(element));
    return elements;
}

NodePtr InterpreterImpl::interpret_error(Comp&& comp) {
    return NodePtr(new ErrorNode(
        std::move(std::get<ErrPtr>(comp.data)), comp.src
    ));
}

NodePtr InterpreterImpl::interpret_expr(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ADD:
        return interpret_binop<Add>(std::move(comp));
    case ADDR:
        return interpret_unop<Addr>(std::move(comp));
    case ADDR_TYPE:
        return interpret_unop<AddrType>(std::move(comp));
    case AND:
        return interpret_binop<And>(std::move(comp));
    case AS:
        return interpret_binop<As>(std::move(comp));
    case BAND:
        return interpret_binop<BitAnd>(std::move(comp));
    case BNOT:
        return interpret_unop<BitNot>(std::move(comp));
    case BOR:
        return interpret_binop<BitOr>(std::move(comp));
    case BXOR:
        return interpret_binop<BitXor>(std::move(comp));
    case CALL:
        return interpret_call(std::move(comp));
    case CHAR:
        return interpret_data<Char>(std::move(comp));
    case DIV:
        return interpret_binop<Divide>(std::move(comp));
    case ENCLOSURE:
        return interpret_enclosure(std::move(comp));
    case EQ:
        return interpret_binop<Equals>(std::move(comp));
    case ERROR:
        return interpret_error(std::move(comp));
    case FALSE:
        return NodePtr(new Bool(false, comp.src));
    case FLOAT_TAIL:
        return interpret_float_tail(std::move(comp));
    case GET:
        return interpret_get(std::move(comp));
    case GROUP:
        return interpret_group(std::move(comp));
    case GT:
        return interpret_binop<GreaterThan>(std::move(comp));
    case GTE:
        return interpret_binop<GreaterThanOrEqual>(std::move(comp));
    case ID:
        return interpret_id(std::move(comp));
    case IN:
        return interpret_binop<In>(std::move(comp));
    case LAMBDA:
        return interpret_lambda(std::move(comp));
    case LAMBDA_ARGS:
        return interpret_lambda_var_args<LambdaArgs>(std::move(comp));
    case LAMBDA_KWARGS:
        return
            interpret_lambda_var_args<LambdaKeywordArgs>(std::move(comp));
    case LIST:
        return interpret_seq<List>(std::move(comp)); 
    case LSH:
        return interpret_binop<LeftShift>(std::move(comp));
    case LT:
        return interpret_binop<LessThan>(std::move(comp));
    case LTE:
        return interpret_binop<LessThanOrEqual>(std::move(comp));
    case MOD:
        return interpret_binop<Mod>(std::move(comp));
    case MUL:
        return interpret_binop<Multiply>(std::move(comp));
    case NEG:
        return interpret_unop<Negate>(std::move(comp));
    case NEQ:
        return interpret_binop<NotEquals>(std::move(comp));
    case NONE:
        return NodePtr(new None(comp.src));
    case NOT:
        return interpret_unop<Not>(std::move(comp));
    case NOT_IN:
        return interpret_not_in(std::move(comp));
    case NUMBER:
        return interpret_number(std::move(comp));
    case NULL_:
        return NodePtr(new Null(comp.src));
    case OR:
        return interpret_binop<Or>(std::move(comp));
    case PLAIN_INT:
        return interpret_plain_int(std::move(comp));
    case POW:
        return interpret_binop<Power>(std::move(comp));
    case RSH:
        return interpret_binop<RightShift>(std::move(comp));
    case SEP:
        return interpret_sep(std::move(comp));
    case STRING:
        return NodePtr(new String(
            std::move(std::get<std::string>(comp.data)), comp.src
        ));
    case SUB:
        return interpret_binop<Subtract>(std::move(comp));
    case SYMBOL:
        return interpret_symbol(std::move(comp));
    case TERNARY_ELSE:
        return interpret_ternary(std::move(comp));
    case THIS:
        return NodePtr(new This(comp.src));
    case TRUE:
        return NodePtr(new Bool(true, comp.src));
    case UP:
        return interpret_up(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedExprErr()),
            std::move(comp)
        ));
    }
}

std::pair<NodePtr, NodePtr> InterpreterImpl::interpret_first_def_case(
    Comp&& comp
) {
    NodePtr id;
    NodePtr spec;
    NodePtr guard;
    NodePtr returns;
    NodePtr body;
    Comp* pred_comp = nullptr;

    using enum OpID;
    switch(comp.comp->op) {
    case LABEL:
        std::tie(pred_comp, returns, guard) = interpret_def_case_pre(
            comp.comp->bin->lhs
        );
        body = interpret_block(std::move(comp.comp->bin->rhs));
        break;
    case BODY:
        id = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingIDErr()),
            Comp(OpID::MISSING, comp.comp->src)
        ));
        spec = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingArgSpecErr()),
            Comp(OpID::MISSING, comp.comp->src)
        ));
        body = interpret_block(std::move(*comp.comp->comp));
        break;
    default: {
        Span after_pred = Span::after(comp.comp->span());
        std::tie(pred_comp, returns, guard) = interpret_def_case_pre(
            *comp.comp
        );
        body = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingBodyErr()),
            Comp(OpID::MISSING, after_pred)
        ));
    }}

    if (pred_comp) {
        switch(pred_comp->op) {
        case CALL:
            id = interpret_definable(std::move(pred_comp->bin->lhs));
            spec = interpret_match_args(std::move(pred_comp->bin->rhs), true);
            break;
        case GROUP:
            id = NodePtr(new ErrorWithComp(
                ErrPtr(new MissingIDErr()),
                Comp(OpID::MISSING, Span(pred_comp->src.start()))
            ));
            spec = interpret_match_args(std::move(*pred_comp), true);
            break;
        case ERROR:
            id = interpret_error(std::move(*pred_comp));
            spec = NodePtr(new ErrorWithComp(
                ErrPtr(new MissingArgSpecErr()),
                Comp(OpID::MISSING, Span::after(id->span()))
            ));
            break;
        default:
            id = interpret_definable(std::move(*pred_comp));
            spec = NodePtr(new ErrorWithComp(
                ErrPtr(new MissingArgSpecErr()),
                Comp(OpID::MISSING, Span::after(id->span()))
            ));
        }
    }

    return {
        std::move(id),
        NodePtr(new DefCase(
            std::move(spec),
            std::move(guard), 
            std::move(returns),
            std::move(body),
            comp.src
        ))
    };
}

NodePtr InterpreterImpl::interpret_float_tail(Comp&& comp) {
    return parse_float(
        std::get<std::string>(comp.data), std::move(comp), comp.src
    );
}

NodePtr InterpreterImpl::interpret_for(std::vector<Comp>&& comps) {
    Nodes vars;
    NodePtr iterable, body;

    using enum OpID;
    switch(comps[0].comp->op) {
    case LABEL:
        std::tie(vars, iterable) =
            interpret_for_pred(std::move(comps[0].comp->bin->lhs));
        body = interpret_block(
            std::move(comps[0].comp->bin->rhs), comps[0].src
        );
        break;
    case BODY:
        iterable = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingPredicateErr()),
            Comp(OpID::MISSING, comps[0].comp->src)
        ));
        body = interpret_block(std::move(*comps[0].comp->comp), comps[0].src);
        break;
    default:
        Span after_pred = Span::after(comps[0].comp->span());
        std::tie(vars, iterable) = interpret_for_pred(
            std::move(*comps[0].comp)
        );
        body = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingBodyErr()),
            Comp(OpID::MISSING, after_pred)
        ));
        break;
    }

    NodePtr orelse = interpret_optional_else(std::move(comps));

    return NodePtr(new For(
        std::move(vars),
        std::move(iterable),
        std::move(body),
        std::move(orelse),
        comps[0].src
    ));
}

std::tuple<Nodes, NodePtr> InterpreterImpl::interpret_for_pred(Comp&& comp) {
    if (comp.op != OpID::IN)
        return {
            Nodes(),
            NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedInErr()),
                std::move(comp)
            ))
        };
    return {
        interpret_loop_vars(std::move(comp.bin->lhs)),
        interpret_expr(std::move(comp.bin->rhs))
    };
}

NodePtr InterpreterImpl::interpret_general_id(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ERROR:
        return interpret_error(std::move(comp));
    case ID:
        return interpret_id(std::move(comp));
    case PLAIN_INT:
        return interpret_num_id(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedGeneralIDErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_get(Comp&& comp) {
    if (
        comp.bin->lhs.op == OpID::PLAIN_INT && (
            comp.bin->rhs.op == OpID::PLAIN_INT || 
            comp.bin->rhs.op == OpID::FLOAT_TAIL
        )
    ) {
        // Two parts of a float, combine to a string and use standard library to
        // read the string into a float.
        std::string s = std::move(std::get<std::string>(comp.bin->lhs.data));
        s += '.';
        s += std::get<std::string>(comp.bin->rhs.data);
        return parse_float(
            s, std::move(comp), Span(comp.bin->lhs.src, comp.bin->rhs.src)
        );
    }
    NodePtr obj = interpret_expr(std::move(comp.bin->lhs));
    NodePtr attr = interpret_general_id(std::move(comp.bin->rhs));
    return NodePtr(new GetAttr(std::move(obj), std::move(attr), comp.src));
}

NodePtr InterpreterImpl::interpret_group(Comp&& comp) {
    if (
        comp.comp->op != OpID::SEP &&
        comp.comp->op != OpID::NOTHING &&
        comp.comp->op != OpID::UNPACK_ARGS
    )
        // Parenthesized value, not a tuple.
        return interpret_expr(std::move(*comp.comp));
    return interpret_seq<Tuple>(std::move(comp));
}

NodePtr InterpreterImpl::interpret_id(Comp&& comp) {
    return interpret_data<ID>(std::move(comp));
}

NodePtr InterpreterImpl::interpret_if(std::vector<Comp>&& comps) {
    auto cases = Nodes();
    cases.push_back(interpret_case(std::move(comps[0])));

    std::uint32_t end = comps.size();
    NodePtr body;
    if (comps.back().op == OpID::ELSE) {
        body = expect_body_only(std::move(comps.back()));
        --end;
    }
    for (std::uint32_t i = 1; i < end; i++) {
        if (comps[i].op != OpID::ELIF)
            cases.push_back(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedElifOrElseErr()),
                std::move(comps[i]),
                comps[i].src
            )));
        else
            cases.push_back(interpret_case(std::move(comps[i])));
    }

    return NodePtr(new If(
        std::move(cases), std::move(body), comps[0].src
    ));
}

NodePtr InterpreterImpl::interpret_kwarg(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BIND: {
        NodePtr bind_id = expect_id(std::move(comp.bin->lhs));
        NodePtr value = interpret_expr(std::move(comp.bin->rhs));
        return NodePtr(new KeywordArg(
            std::move(bind_id), std::move(value), comp.src
        ));
    }
    case ERROR:
        return interpret_error(std::move(comp));
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedKeywordArgExprErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_label(Comp&& comp) {
    return NodePtr(new EnterObject(
        interpret_expr(std::move(comp.bin->lhs)),
        interpret_code(std::move(comp.bin->rhs)),
        comp.src
    ));
}

NodePtr InterpreterImpl::interpret_lambda(Comp&& comp) {
    // Compute this before moving.
    Span total_span = comp.span();
    auto [count, arg, span] = count_op(std::move(comp), OpID::LAMBDA);

    if (arg.op == OpID::GROUP && count == 1) {
        NodePtr inner = interpret_expr(std::move(arg));
        return NodePtr(new Lambda(std::move(inner), total_span));
    }

    using enum OpID;
    switch(arg.op) {
    case LAMBDA_ARGS:
        return NodePtr(new LambdaArgs(count + 1, total_span));
    case LAMBDA_KWARGS:
        return NodePtr(new LambdaKeywordArgs(count + 1, total_span));
    default: {
        NodePtr var = interpret_general_id(std::move(arg));
        return NodePtr(new LambdaVar(count, std::move(var), span));
    }}
}

template<typename NodeType>
NodePtr InterpreterImpl::interpret_lambda_var_args(Comp&& comp) {
    return NodePtr(new NodeType(1, comp.src));
}

Nodes InterpreterImpl::interpret_loop_vars(Comp&& comp) {
    auto vars = Nodes();

    Comp* c = &comp;
    if (comp.op == OpID::LOOP_VAR_SEP) {
        vars = interpret_loop_vars(std::move(comp.bin->lhs));
        c = &comp.bin->rhs;
    }

    using enum OpID;
    switch(c->op) {
    case ID:
        vars.push_back(interpret_id(std::move(*c)));
        return vars;
    case PLACEHOLDER:
        vars.push_back(NodePtr(new Placeholder(c->src)));
        return vars;
    default:
        vars.push_back(NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedIDErr()),
            std::move(*c)
        )));
        return vars;
    }
}

NodePtr InterpreterImpl::interpret_map_element(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ENTRY:
        return interpret_binop<Entry>(std::move(comp));
    case ERROR:
        return interpret_error(std::move(comp));
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedEntryExprErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_match(std::vector<Comp>&& comps) {
    if (comps.size() == 1)
        return NodePtr(new ErrorWithComp(
            ErrPtr(new NoMatchCasesErr()),
            std::move(comps[0]),
            comps[0].src
        ));
    NodePtr matchee = interpret_expr(std::move(*comps[0].comp));
    Nodes cases = interpret_match_cases(
        std::move(comps), comps.size(), OpID::CASE
    );

    return NodePtr(new Match(
        std::move(matchee), std::move(cases), comps[0].src
    ));
}

NodePtr InterpreterImpl::interpret_match_args(Comp&& comp, bool is_def) {
    if (comp.op != OpID::GROUP)
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedParentheticalErr()),
            std::move(comp)
        ));

    bool in_kw_part = false;
    MatchArgs args = interpret_match_args_recurse(
        std::move(*comp.comp), in_kw_part, is_def
    );
    args.src = comp.src;
    return NodePtr(new MatchArgs(std::move(args)));
}

MatchArgs InterpreterImpl::interpret_match_args_recurse(
    Comp&& comp, bool& in_kw_part, bool is_def
) {
    auto args = MatchArgs({}, {}, Span(0, 0));
    if (comp.op == OpID::NOTHING)
        return args;
    Comp* c = &comp;
    if (comp.op == OpID::SEP) {
        args = interpret_match_args_recurse(
            std::move(comp.bin->lhs), in_kw_part, is_def
        );
        c = &comp.bin->rhs;
    }

    if (c->op == OpID::POS_KW_SEP) {
        if (in_kw_part)
            args.kwargs.push_back(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedKeywordArgMatchExprErr()),
                std::move(*c)
            )));
        else if (!is_def)
            args.args.push_back(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedArgMatchExprErr()),
                std::move(*c)
            )));
        else
            in_kw_part = true;
        return args;
    }

    MatchKind kind = is_def ? MatchKind::DEF: MatchKind::OBJECT;
    NodePtr arg = interpret_arg_matcher(std::move(*c), kind, in_kw_part);

    if (in_kw_part)
        args.kwargs.push_back(std::move(arg));
    else
        args.args.push_back(std::move(arg));
    return args;
}

NodePtr InterpreterImpl::interpret_match_as(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    NodePtr lhs;

    using enum OpID;
    switch(comp.bin->lhs.op) {
    case AS:
    case TYPE_LABEL:
    case UNPACK_ARGS:
    case UNPACK_KWARGS:
        lhs = NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedAsLHSExprErr()),
            std::move(comp.bin->lhs)
        ));
        break;
    default:;
        lhs = interpret_arg_matcher(
            std::move(comp.bin->lhs), kind, in_kw_part
        );
    }

    NodePtr rhs = interpret_match_general_var(
        std::move(comp.bin->rhs), kind, in_kw_part
    );
    return NodePtr(new As(std::move(lhs), std::move(rhs), comp.src));
}

NodePtr InterpreterImpl::interpret_match_bind(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    switch(kind) {
    case MatchKind::DEF:
        return interpret_dflt(std::move(comp));
    case MatchKind::OBJECT:
        in_kw_part = true;
        return interpret_match_kwarg(std::move(comp));
    case MatchKind::MAP:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedEntryMatchExprErr()),
            std::move(comp)
        ));
    case MatchKind::PLAIN:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedMatchExprErr()),
            std::move(comp)
        ));
    case MatchKind::SEQ:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedElementMatchExprErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_match_case(Comp&& comp) {
    NodePtr matcher, guard, body;

    using enum OpID;
    switch(comp.comp->op) {
    case LABEL:
        std::tie(matcher, guard) = interpret_match_case_pred(
            std::move(comp.comp->bin->lhs)
        );
        body = interpret_block(std::move(comp.comp->bin->rhs));
        break;
    case BODY:
        matcher = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingPredicateErr()),
            Comp(OpID::MISSING, comp.comp->src)
        ));
        body = interpret_block(std::move(*comp.comp->comp));
        break;
    default: {
        Span after_pred = Span::after(comp.comp->span());
        std::tie(matcher, guard) = interpret_match_case_pred(
            std::move(*comp.comp)
        );
        body = NodePtr(new ErrorWithComp(
            ErrPtr(new MissingBodyErr()),
            Comp(OpID::MISSING, after_pred)
        ));
    }}
    return NodePtr(new MatchCase(
        std::move(matcher), std::move(guard), std::move(body), comp.src
    ));
}

std::tuple<NodePtr, NodePtr> InterpreterImpl::interpret_match_case_pred(
    Comp&& comp
) {
    if (comp.op == OpID::TERNARY_IF)
        return {
            interpret_matcher(std::move(comp.bin->lhs), MatchKind::PLAIN),
            interpret_expr(std::move(comp.bin->rhs))
        };
    return {
        interpret_matcher(std::move(comp), MatchKind::PLAIN),
        nullptr
    };
}

Nodes InterpreterImpl::interpret_match_cases(
    std::vector<Comp>&& comps, std::uint32_t end, OpID op
) {
    auto cases = Nodes();
    for (std::uint32_t i = 1; i < end; i++) {
        if (comps[i].op != op) {
            if (op == OpID::CASE)
                cases.push_back(NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedCaseErr()),
                    std::move(comps[i]),
                    comps[i].src
                )));
            else
                cases.push_back(NodePtr(new ErrorWithComp(
                    ErrPtr(new ExpectedExceptOrFinallyErr()),
                    std::move(comps[i]),
                    comps[i].src
                )));
        } else
            cases.push_back(interpret_match_case(std::move(comps[i])));
    }
    return cases;
}

NodePtr InterpreterImpl::interpret_match_general_var(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    using enum OpID;
    switch(comp.op) {
    case ID:
        return interpret_id(std::move(comp));
    case UNPACK_ARGS:
        if (comp.comp->op != ID)
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedIDErr()),
                std::move(*comp.comp)
            ));
        return interpret_match_var_args(std::move(comp), kind, in_kw_part);
    case UNPACK_KWARGS:
        if (comp.comp->op != ID)
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedIDErr()),
                std::move(*comp.comp)
            ));
        return interpret_match_var_kwargs(std::move(comp), kind, in_kw_part);
    case ERROR:
        return interpret_error(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedIDOrVarArgsErr()),
            std::move(comp)
        ));
    }
}

NodePtr InterpreterImpl::interpret_match_kwarg(Comp&& comp) {
    NodePtr lhs = expect_id(std::move(comp.bin->lhs));
    NodePtr rhs = interpret_matcher(std::move(comp.bin->rhs), MatchKind::PLAIN);
    return NodePtr(new MatchKeywordArg(
        std::move(lhs), std::move(rhs), comp.src
    ));
}

NodePtr InterpreterImpl::interpret_match_list(Comp&& comp) {
    Nodes nodes = interpret_match_seq_recurse(std::move(*comp.comp));
    return NodePtr(new MatchList(std::move(nodes), comp.src));
}

NodePtr InterpreterImpl::interpret_match_map(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new MatchMap({}, comp.src));
    Nodes nodes = interpret_match_map_recurse(std::move(*comp.comp));
    return NodePtr(new MatchMap(std::move(nodes), comp.src));
}

Nodes InterpreterImpl::interpret_match_map_recurse(Comp&& comp) {
    Nodes nodes;
    Comp* c = &comp;
    if (comp.op == OpID::SEP) {
        nodes = interpret_match_map_recurse(
            std::move(comp.bin->lhs)
        );
        c = &comp.bin->rhs;
    }

    using enum OpID;
    switch(c->op) {
    case AS:
        if (c->bin->rhs.op != UNPACK_KWARGS)
            nodes.push_back(NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedEntryMatchExprErr()),
                std::move(*c)
            )));
        else
            nodes.push_back(interpret_matcher(std::move(*c), MatchKind::MAP));
        return nodes;
    case ENTRY: {
        NodePtr key = interpret_expr(std::move(c->bin->lhs));
        NodePtr matcher =
            interpret_matcher(std::move(c->bin->rhs), MatchKind::PLAIN);
        nodes.push_back(NodePtr(new Entry(
            std::move(key), std::move(matcher), c->src
        )));
        return nodes;
    }
    case UNPACK_KWARGS:
        nodes.push_back(interpret_unop<Expansion>(std::move(*c)));
        return nodes;
    case ERROR:
        nodes.push_back(interpret_error(std::move(*c)));
        return nodes;
    default:
        nodes.push_back(NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedEntryMatchExprErr()),
            std::move(*c)
        )));
        return nodes;
    }
}

NodePtr InterpreterImpl::interpret_match_obj(Comp&& comp) {
    NodePtr type = interpret_expr(std::move(comp.bin->lhs));
    NodePtr args = interpret_match_args(std::move(comp.bin->rhs), false);
    return NodePtr(new MatchObject(std::move(type), std::move(args), comp.src));
}

NodePtr InterpreterImpl::interpret_match_options(Comp&& comp) {
    Nodes nodes = interpret_match_options_recurse(std::move(comp));
    auto src = Span(nodes.front()->span(), nodes.back()->span());
    return NodePtr(new MatchOptions(std::move(nodes), src));
}

Nodes InterpreterImpl::interpret_match_options_recurse(Comp&& comp) {
    Nodes nodes;
    if (comp.op == OpID::MATCH_OR) {
        nodes = interpret_match_options_recurse(std::move(comp.bin->lhs));
        nodes.push_back(
            interpret_matcher(std::move(comp.bin->rhs), MatchKind::PLAIN)
        );
    } else
        nodes.push_back(interpret_matcher(std::move(comp), MatchKind::PLAIN));
    return nodes;
}

Nodes InterpreterImpl::interpret_match_seq_recurse(Comp&& comp) {
    Nodes nodes;
    if (comp.op == OpID::NOTHING)
        return nodes;
    if (comp.op == OpID::SEP) {
        nodes = interpret_match_seq_recurse(std::move(comp.bin->lhs));
        nodes.push_back(
            interpret_matcher(std::move(comp.bin->rhs), MatchKind::SEQ)
        );
    } else
        nodes.push_back(interpret_matcher(std::move(comp), MatchKind::SEQ));
    return nodes;
}

NodePtr InterpreterImpl::interpret_match_tuple(Comp&& comp) {
    using enum OpID;
    switch(comp.comp->op) {
    case AS:
        if (comp.comp->bin->rhs.op != UNPACK_ARGS)
            // Because it is a single element that is not var args, treat it as
            // a parenthesized value and as though we tried to interpret this
            // as an expression.
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedExprErr()),
                std::move(*comp.comp)
            ));
        break;
    case NOTHING:
    case SEP:
    case UNPACK_ARGS:
        break;
    case TYPE_LABEL:
        if (comp.comp->bin->lhs.op != UNPACK_ARGS)
            // Again, treat like we tried to interpret this as an expression.
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedExprErr()),
                std::move(*comp.comp)
            ));
        break;
    default:
        // Parenthesized value, not a tuple.
        return interpret_expr(std::move(*comp.comp));
    }
    Nodes nodes = interpret_match_seq_recurse(std::move(*comp.comp));
    return NodePtr(new MatchTuple(std::move(nodes), comp.src));
}

NodePtr InterpreterImpl::interpret_match_var_args(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    switch(kind) {
    case MatchKind::MAP:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedEntryMatchExprErr()),
            std::move(comp)
        ));
    case MatchKind::PLAIN:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedMatchExprErr()),
            std::move(comp)
        ));
    default:
        if (in_kw_part)
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedKeywordArgMatchExprErr()),
                std::move(comp)
            ));
        return interpret_unop<Expansion>(std::move(comp));
    }
}

NodePtr InterpreterImpl::interpret_match_var_kwargs(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    switch(kind) {
    case MatchKind::PLAIN:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedMatchExprErr()),
            std::move(comp)
        ));
    case MatchKind::SEQ:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedElementMatchExprErr()),
            std::move(comp)
        ));
    default:
        in_kw_part = true;
        return interpret_unop<Expansion>(std::move(comp));
    }
}

NodePtr InterpreterImpl::interpret_matcher(Comp&& comp, MatchKind kind) {
    // Not exactly elegant, but passing in in_kw_part even when it's not needed
    // allows much code to be reused.
    bool in_kw_part = false;
    return interpret_arg_matcher(std::move(comp), kind, in_kw_part);
}

NodePtr InterpreterImpl::interpret_not_in(Comp&& comp) {
    NodePtr in = interpret_binop<In>(std::move(comp));
    return NodePtr(new Not(std::move(in), comp.src));
}

NodePtr InterpreterImpl::interpret_num_id(Comp&& comp) {
    std::string& s = std::get<std::string>(comp.data);
    Res<std::int32_t> x =
        read_number<std::int32_t>(&s[0], &s[0] + s.size(), 10);
    if (x.is_err)
        return NodePtr(new ErrorWithComp(
            std::move(x.err),
            std::move(comp)
        ));
    if (x.res == 0)
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ZeroUsedAsIDErr()),
            std::move(comp)
        ));
    return NodePtr(new NumID(std::move(x.res), comp.src));
}

NodePtr InterpreterImpl::interpret_number(Comp&& comp) {
    switch(comp.data.index()) {
    case 1:
        return interpret_data<Int8>(std::move(comp));
    case 2:
        return interpret_data<Int16>(std::move(comp));
    case 3:
        return interpret_data<Int32>(std::move(comp));
    case 4:
        return interpret_data<Int64>(std::move(comp));
    case 5:
        return interpret_data<UInt8>(std::move(comp));
    case 6:
        return interpret_data<UInt16>(std::move(comp));
    case 7:
        return interpret_data<UInt32>(std::move(comp));
    case 8:
        return interpret_data<UInt64>(std::move(comp));
    default:
        return NodePtr(new ErrorWithComp(
            ErrPtr(new AssertionFailedErr("Bad numerical data")),
            std::move(comp)
        ));
    }
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_optional(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new ToType(nullptr, comp.src));
    NodePtr res = interpret_expr(std::move(*comp.comp));
    return NodePtr(new ToType(std::move(res), comp.src));
}

NodePtr InterpreterImpl::interpret_optional_else(std::vector<Comp>&& comps) {
    // This function interprets an optional else clause for "for" and "while".
    if (comps.size() > 2) {
        // for and while may only have at most two parts.
        std::vector<Comp> blocks;
        for (std::uint32_t i = 1; i < comps.size(); i++)
            blocks.push_back(std::move(comps[i]));
        return NodePtr(new ErrorWithComp(
            ErrPtr(new UnexpectedBlocksErr()),
            Comp(
                OpID::BLOCKS,
                std::move(blocks),
                Span(blocks.front().span(), blocks.back().span())
            )
        ));
    }
        
    if (comps.size() == 2) {
        if (comps[1].op != OpID::ELSE)
            return NodePtr(new ErrorWithComp(
                ErrPtr(new ExpectedElseErr()),
                std::move(comps[1]),
                comps[1].src
            ));
        return expect_body_only(std::move(comps[1]));
    }
    // Use nullptr to indicate lack of else and no error.
    return nullptr;
}

NodePtr InterpreterImpl::interpret_plain_int(Comp&& comp) {
    std::string& s = std::get<std::string>(comp.data);
    Res<std::int32_t> x = read_number<std::int32_t>(
        &s[0], &s[0] + s.size(), 10
    );
    if (x.is_err)
        return NodePtr(new ErrorWithComp(std::move(x.err), std::move(comp)));
    return NodePtr(new Int32(std::move(x.res), comp.src));
}

NodePtr InterpreterImpl::interpret_sep(Comp&& comp) {
    Nodes elements = interpret_csv(std::move(comp));
    return NodePtr(new Tuple(std::move(elements), comp.span()));
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_seq(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new ToType({}, comp.src));
    Nodes nodes = interpret_csv(std::move(*comp.comp));
    return NodePtr(new ToType(std::move(nodes), comp.src));
}

NodePtr InterpreterImpl::interpret_symbol(Comp&& comp) {
    NodePtr id = interpret_general_id(std::move(*comp.comp));
    return NodePtr(new Symbol(std::move(id), comp.span()));
}

NodePtr InterpreterImpl::interpret_ternary(Comp&& comp) {
    if (comp.bin->lhs.op != OpID::TERNARY_IF)
        return NodePtr(new ErrorWithComp(
            ErrPtr(new ExpectedExprErr()), std::move(comp)
        ));
    
    NodePtr predicate = interpret_expr(std::move(comp.bin->lhs.bin->rhs));
    NodePtr if_case = interpret_expr(std::move(comp.bin->lhs.bin->lhs));
    NodePtr else_case = interpret_expr(std::move(comp.bin->rhs));

    return NodePtr(new Ternary(
        std::move(predicate),
        std::move(if_case),
        std::move(else_case),
        comp.bin->lhs.src
    ));
}

NodePtr InterpreterImpl::interpret_try(std::vector<Comp>&& comps) {
    if (comps.size() == 1)
        return NodePtr(new ErrorWithComp(
            ErrPtr(new NoExceptsOrFinallyErr()),
            std::move(comps[0]),
            comps[0].src
        ));

    NodePtr body = expect_body_only(std::move(comps[0]));

    NodePtr finally;
    std::uint32_t end = comps.size();
    if (comps.back().op == OpID::FINALLY) {
        finally = expect_body_only(std::move(comps.back()));

        // Exclude last comp from cases.
        end--;
    }
    Nodes cases = interpret_match_cases(std::move(comps), end, OpID::EXCEPT);

    return NodePtr(new Try(
        std::move(body),
        std::move(cases),
        std::move(finally),
        comps[0].src
    ));
}

NodePtr InterpreterImpl::interpret_type_match(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    NodePtr var = interpret_match_general_var(
        std::move(comp.bin->lhs), kind, in_kw_part
    );
    NodePtr type = interpret_expr(std::move(comp.bin->rhs));
    return NodePtr(new TypeMatch(std::move(var), std::move(type), comp.src));
}

NodePtr InterpreterImpl::interpret_unexpected_construct(Comp&& comp) {
    using enum OpID;

    switch(comp.comp->op) {
    case BODY:
        return NodePtr(new UnexpectedConstruct(
            comp.op, interpret_block(std::move(*comp.comp->comp)), comp.src
        ));
    case LABEL:
        return NodePtr(new UnexpectedConstruct(
            comp.op, interpret_case(std::move(comp)), comp.src
        ));
    default:
        return NodePtr(new UnexpectedConstruct(
            comp.op, interpret_expr(std::move(*comp.comp)), comp.src
        ));
    }
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_unop(Comp&& comp) {
    NodePtr arg = interpret_expr(std::move(*comp.comp));
    return NodePtr(new ToType(std::move(arg), comp.src));
}

NodePtr InterpreterImpl::interpret_up(Comp&& comp) {
    auto [count, var, span] = count_op(std::move(comp), OpID::UP);
    NodePtr id = interpret_general_id(std::move(var));
    return NodePtr(new Var(count, std::move(id), span));
}

NodePtr InterpreterImpl::interpret_while(std::vector<Comp>&& comps) {
    NodePtr content = interpret_case(std::move(comps[0]));
    NodePtr orelse = interpret_optional_else(std::move(comps));

    return NodePtr(new While(
        std::move(content), std::move(orelse), comps[0].src
    ));
}

bool InterpreterImpl::is_map(const Nodes& nodes) {
    return
        nodes.empty() ||
        nodes[0]->kind() == NodeKind::ENTRY || (
            nodes[0]->kind() == NodeKind::ERROR && 
            dynamic_cast<const ErrorWithComp&>(*nodes[0]).comp.op == OpID::ENTRY
        );
}

NodePtr InterpreterImpl::parse_float(
    const std::string& str, Comp&& comp, Span src
) {
    const char* begin = &str[0];
    LiteralSuffix ls = lit_suffix(str);
    const char* expected_end = begin + str.size() - lit_suffix_len(ls);
    if (ls == LiteralSuffix::F32) {
        Res<float> res = read_number<float>(begin, expected_end, 10);
        if (res.is_err)
            return NodePtr(new ErrorWithComp(
                std::move(res.err), std::move(comp)
            ));
        return NodePtr(new Float32(std::move(res.res), src));
    }
    Res<double> res = read_number<double>(begin, expected_end, 10);
    if (res.is_err)
        return NodePtr(new ErrorWithComp(
            std::move(res.err), std::move(comp)
        ));
    return NodePtr(new Float64(std::move(res.res), src));
}

}
