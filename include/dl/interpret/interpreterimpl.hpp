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
#include "dl/interpret/while.hpp"
#include "dl/lex/literalsuffix.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"

namespace dl {

struct ArgSepWhereKeywordArgExpectedErr final: SourcedErr {
    ArgSepWhereKeywordArgExpectedErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ArgSepWhereKeywordArgExpectedErr";
    }
};

struct DefaultedVarArgsErr final: SourcedErr {
    DefaultedVarArgsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DefaultedVarArgsErr";
    }
};

struct DefaultedVarKeywordArgsErr final: SourcedErr {
    DefaultedVarKeywordArgsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DefaultedVarKeywordArgsErr";
    }
};

struct DoubleAsErr final: SourcedErr {
    DoubleAsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DoubleAsErr";
    }
};

struct ExpectedCallErr final: SourcedErr {
    ExpectedCallErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCallErr";
    }
};

struct ExpectedCaseErr final: SourcedErr {
    ExpectedCaseErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCaseErr";
    }
};

struct ExpectedColonErr final: SourcedErr {
    ExpectedColonErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedColonErr";
    }
};

struct ExpectedCommaSeparatedLoopVarsErr final: SourcedErr {
    ExpectedCommaSeparatedLoopVarsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCommaSeparatedLoopVarsErr";
    }
};

struct ExpectedCompoundIDErr final: SourcedErr {
    ExpectedCompoundIDErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCompoundIDErr";
    }
};

struct ExpectedElifOrElseErr final: SourcedErr {
    ExpectedElifOrElseErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElifOrElseErr";
    }
};

struct ExpectedElseErr final: SourcedErr {
    ExpectedElseErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElseErr";
    }
};

struct ExpectedEntryOrVarKeywordArgsErr final: SourcedErr {
    ExpectedEntryOrVarKeywordArgsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedEntryOrVarKeywordArgsErr";
    }
};

struct ExpectedExceptOrFinallyErr final: SourcedErr {
    ExpectedExceptOrFinallyErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExceptOrFinallyErr";
    }
};

struct ExpectedExprErr final: SourcedErr {
    ExpectedExprErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExprErr";
    }
};

struct ExpectedIDErr final: SourcedErr {
    ExpectedIDErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDErr";
    }
};

struct ExpectedIDOrCallErr final: SourcedErr {
    ExpectedIDOrCallErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrCallErr";
    }
};

struct ExpectedIDOrVarArgsErr final: SourcedErr {
    ExpectedIDOrVarArgsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrVarArgsErr";
    }
};

struct ExpectedGeneralIDErr final: SourcedErr {
    ExpectedGeneralIDErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedGeneralIDErr";
    }
};

struct ExpectedInErr final: SourcedErr {
    ExpectedInErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedInErr";
    }
};

struct ExpectedParentheticalErr final: SourcedErr {
    ExpectedParentheticalErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedParentheticalErr";
    }
};

struct ExpectedStringErr final: SourcedErr {
    ExpectedStringErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedStringErr";
    }
};

struct ExpectedTernaryIfErr final: SourcedErr {
    ExpectedTernaryIfErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedTernaryIfErr";
    }
};

struct IsolatedTryErr final: SourcedErr {
    IsolatedTryErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "IsolatedTryErr";
    }
};

struct MissingBodyErr final: SourcedErr {
    MissingBodyErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingBodyErr";
    }
};

struct MissingPredicateErr final: SourcedErr {
    MissingPredicateErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingPredicateErr";
    }
};

struct NoMatchCasesErr final: SourcedErr {
    NoMatchCasesErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "NoMatchCasesErr";
    }
};

struct PosAfterKeywordErr final: SourcedErr {
    PosAfterKeywordErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "PosAfterKeywordErr";
    }
};

struct TwiceDefaultedErr final: SourcedErr {
    TwiceDefaultedErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "TwiceDefaultedErr";
    }
};

struct TypeMatchWithAsErr final: SourcedErr {
    TypeMatchWithAsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "TypeMatchWithAsErr";
    }
};

struct UnexpectedOpErr final: SourcedErr {
    OpID op;

    UnexpectedOpErr(OpID op, Span src) noexcept:
    SourcedErr(src), op(op) {}

    UnexpectedOpErr(const Comp& comp) noexcept:
    UnexpectedOpErr(comp.op, comp.src) {}

    bool equals(const Err& err) const noexcept override {
        return
            SourcedErr::equals(err) &&
            op == dynamic_cast<const UnexpectedOpErr&>(err).op;
    }

    std::ostream& out_data(std::ostream& os) const override {
        return os << op;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedOpErr";
    }
};

struct UnexpectedContinuationErr final: SourcedErr {
    UnexpectedContinuationErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedContinuationErr";
    }
};

struct UnexpectedPredicateErr final: SourcedErr {
    UnexpectedPredicateErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedPredicateErr";
    }
};

struct VarArgsNotAllowedHereErr final: SourcedErr {
    VarArgsNotAllowedHereErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarArgsNotAllowedHereErr";
    }
};

struct VarArgsWithAsErr final: SourcedErr {
    VarArgsWithAsErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarArgsWithAsErr";
    }
};

struct VarArgsWhereKeywordArgExpectedErr final: SourcedErr {
    VarArgsWhereKeywordArgExpectedErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarArgsWhereKeywordArgExpectedErr";
    }
};

struct VarKeywordArgsNotAllowedHereErr final: SourcedErr {
    VarKeywordArgsNotAllowedHereErr(Span src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarKeywordArgsNotAllowedHereErr";
    }
};

struct ZeroUsedAsIDErr final: SourcedErr {
    ZeroUsedAsIDErr(Span src) noexcept: SourcedErr(src) {}

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
    static inline NodeRes interpret_(Comp&& comp);
    NodeRes interpret(Comp&& comp) override {
        return interpret_(std::move(comp));
    }

    static inline ID as_id(Comp&& comp);
    static inline std::tuple<std::uint32_t, Comp, Span>
        count_op(Comp&& comp, OpID op);
    static inline Res<Nodes> expect_body_only(Comp&& comp );
    static inline Res<ID> expect_id(Comp&& comp);
    static inline NodeRes interpret_assign(Comp&& comp);
    static inline NodeRes interpret_arg(Comp&& comp);
    static inline NodeRes interpret_arg_matcher(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline Res<Args> interpret_args(Comp&& comp);
    static inline Res<Args> interpret_args_recurse(
        Comp&& comp, bool& in_kw_part
    );

    template<typename ToType>
    static inline NodeRes interpret_binop(Comp&& comp);

    static inline NodeRes interpret_binop_method(
        Comp&& comp, std::string fn_name
    );
    static inline Res<Nodes> interpret_body(Comp&& comp);
    static inline Res<Nodes> interpret_block(Comp&& comp);
    static inline NodeRes interpret_break(Comp&& comp);
    static inline NodeRes interpret_call(Comp&& comp);
    static inline Res<Case> interpret_case(Comp&& comp);
    static inline NodeRes interpret_concat(Comp&& comp);
    static inline NodeRes interpret_construct(std::vector<Comp>&& comps);
    static inline NodeRes interpret_continue(Comp&& comp);
    static inline Res<Nodes> interpret_csv(Comp&& comp);

    template<typename ToType>
    static inline NodePtr interpret_data(Comp&& comp);

    static inline NodeRes interpret_declare(Comp&& comp);
    static inline NodeRes interpret_def(std::vector<Comp>&& comps);
    static inline Res<DefCase> interpret_def_case(Comp&& comp);
    static inline Res<std::pair<MatchArgs, Nodes>> interpret_def_case_post(
        Comp&& comp, Comp&& args_comp
    );
    static inline Res<std::tuple<Comp*, NodePtr, NodePtr>>
        interpret_def_case_pre(Comp& comp);
    static inline NodeRes interpret_definable(Comp&& comp);
    static inline NodeRes interpret_dflt(Comp&& comp);
    static inline NodeRes interpret_enclosure(Comp&& comp);
    static inline Res<Nodes> interpret_enclosure_recurse(Comp&& comp);
    static inline NodeRes interpret_entry(Comp&& comp);
    static inline NodeRes interpret_expr(Comp&& comp);
    static inline Res<std::pair<NodePtr, DefCase>> interpret_first_def_case(
        Comp&& comp
    );
    static inline NodeRes interpret_float_tail(Comp&& comp);
    static inline NodeRes interpret_for(std::vector<Comp>&& comps);
    static inline NodeRes interpret_general_id(Comp&& comp);
    static inline NodeRes interpret_get(Comp&& comp);
    static inline NodeRes interpret_group(Comp&& comp);
    static inline NodePtr interpret_id(Comp&& comp);
    static inline NodeRes interpret_if(std::vector<Comp>&& comps);
    static inline NodeRes interpret_kwarg(Comp&& comp);
    static inline NodeRes interpret_lambda(Comp&& comp);

    template<typename NodeType>
    static inline NodeRes interpret_lambda_var_args(Comp&& comp);

    static inline NodeRes interpret_literally(Comp&& comp);
    static inline Res<Nodes> interpret_loop_vars(Comp&& comp);
    static inline NodeRes interpret_map_element(Comp&& comp);
    static inline NodeRes interpret_match(std::vector<Comp>&& comps);
    static inline Res<MatchArgs> interpret_match_args(Comp&& comp, bool is_def);
    static inline Res<MatchArgs> interpret_match_args_recurse(
        Comp&& comp, bool& in_kw_part, bool is_def
    );
    static inline NodeRes interpret_match_as(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodeRes interpret_match_bind(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline Res<MatchCase> interpret_match_case(Comp&& comp);
    static inline Res<std::vector<MatchCase>> interpret_match_cases(
        std::vector<Comp>&& comps, std::uint32_t end, OpID op
    );
    static inline NodeRes interpret_match_general_var(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodeRes interpret_match_kwarg(Comp&& comp);
    static inline NodeRes interpret_match_list(Comp&& comp);
    static inline NodeRes interpret_match_map(Comp&& comp);
    static inline Res<Nodes> interpret_match_map_recurse(Comp&& comp);
    static inline NodeRes interpret_match_obj(Comp&& comp);
    static inline NodeRes interpret_match_options(Comp&& comp);
    static inline Res<Nodes> interpret_match_options_recurse(Comp&& comp);
    static inline Res<Nodes> interpret_match_seq_recurse(Comp&& comp);
    static inline NodeRes interpret_match_tuple(Comp&& comp);
    static inline NodeRes interpret_match_var_args(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodeRes interpret_match_var_kwargs(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );
    static inline NodeRes interpret_matcher(Comp&& comp, MatchKind kind);
    static inline NodeRes interpret_not_in(Comp&& comp);
    static inline NodeRes interpret_num_id(Comp&& comp);
    static inline NodeRes interpret_number(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_optional(Comp&& comp);

    static inline Res<Block> interpret_optional_else(std::vector<Comp>&& comps);
    static inline NodeRes interpret_plain_int(Comp&& comp);
    static inline NodeRes interpret_sep(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_seq(Comp&& comp);

    static inline NodeRes interpret_symbol(Comp&& comp);
    static inline NodeRes interpret_ternary(Comp&& comp);
    static inline NodeRes interpret_try(std::vector<Comp>&& comps);
    static inline NodeRes interpret_type(Comp&& comp);
    static inline NodeRes interpret_type_match(
        Comp&& comp, MatchKind kind, bool& in_kw_part
    );

    template<typename ToType>
    static inline NodeRes interpret_unop(Comp&& comp);

    static inline NodeRes interpret_unop_method(
        Comp&& comp, std::string fn_name
    );
    static inline NodeRes interpret_up(Comp&& comp);
    static inline NodeRes interpret_while(std::vector<Comp>&& comps);
    static inline bool is_map(const Nodes& nodes);
    static inline NodeRes parse_float(const std::string& str, Span src);
    static inline ErrPtr require_pred_and_body(const Comp& comp);
};

ID InterpreterImpl::as_id(Comp&& comp) {
    return ID(std::move(std::get<std::string>(comp.data)), comp.src);
}

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

Res<Nodes> InterpreterImpl::expect_body_only(Comp&& comp) {
    if (comp.op != OpID::BODY) {
        if (comp.op == OpID::LABEL)
            return ErrPtr(new UnexpectedPredicateErr(comp.bin->lhs.span()));
        return ErrPtr(new ExpectedColonErr(comp.span()));
    }
    return interpret_body(std::move(*comp.comp));
}

Res<ID> InterpreterImpl::expect_id(Comp&& comp) {
    if (comp.op != OpID::ID)
        return ErrPtr(new ExpectedIDErr(comp.src));
    return ID(std::move(std::get<std::string>(comp.data)), comp.src);
}

NodeRes InterpreterImpl::interpret_(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BREAK:
        return interpret_break(std::move(comp));
    case CONTINUE:
        return interpret_continue(std::move(comp));
    case CONSTRUCT:
        return interpret_construct(std::move(comp.comps));
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
    case RAISE:
        return interpret_optional<Raise>(std::move(comp));
    case RETURN:
        return interpret_optional<Return>(std::move(comp));
    case SET:
        return interpret_assign(std::move(comp));
    case TYPE_LABEL:
        return interpret_declare(std::move(comp));
    default:
        return interpret_expr(std::move(comp));
    }
}

NodeRes InterpreterImpl::interpret_arg(Comp&& comp) {
    if (comp.op == OpID::UNPACK_ARGS)
        return interpret_unop<Expansion>(std::move(comp));
    return interpret_expr(std::move(comp));
}

NodeRes InterpreterImpl::interpret_arg_matcher(
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

Res<Args> InterpreterImpl::interpret_args(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return Args({}, {}, comp.src);

    bool in_kw_part = false;
    Res<Args> args = interpret_args_recurse(std::move(*comp.comp), in_kw_part);
    if (args.is_err)
        return args;
    args.res.src = comp.src;
    return args;
}

Res<Args> InterpreterImpl::interpret_args_recurse(
    Comp&& comp, bool& in_kw_part
) {
    auto args = Args({}, {}, Span(0, 0, 0, 0));
    Comp* c = &comp;
    if (comp.op == OpID::SEP) {
        Res<Args> args_res = interpret_args_recurse(
            std::move(comp.bin->lhs), in_kw_part
        );
        if (args_res.is_err)
            return args_res;
        args = std::move(args_res.res);
        c = &comp.bin->rhs;
    }

    in_kw_part =
        in_kw_part ||
        c->op == OpID::BIND ||
        c->op == OpID::UNPACK_KWARGS;
    if (in_kw_part) {
        NodeRes res = interpret_kwarg(std::move(*c));
        if (res.is_err)
            return std::move(res.err);
        args.kwargs.push_back(std::move(res.res));
    } else {
        NodeRes res = interpret_arg(std::move(*c));
        if (res.is_err)
            return std::move(res.err);
        args.args.push_back(std::move(res.res));
    }
    return args;
}

NodeRes InterpreterImpl::interpret_assign(Comp&& comp) {
    NodeRes rhs = interpret_expr(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;

    if (comp.bin->lhs.op == OpID::TYPE_LABEL) {
        // Variable initialization
        NodeRes id = interpret_definable(std::move(comp.bin->lhs.bin->lhs));
        if (id.is_err)
            return id;
        NodeRes type = interpret_expr(std::move(comp.bin->lhs.bin->rhs));
        if (type.is_err)
            return type;
        return NodePtr(new Init(
            std::move(id.res),
            std::move(type.res),
            std::move(rhs.res),
            comp.src
        ));
    }
    NodeRes lhs = interpret_expr(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return lhs;

    return NodePtr(
        new Assign(std::move(lhs.res), std::move(rhs.res), comp.src)
    );
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_binop(Comp&& comp) {
    NodeRes lhs = interpret_expr(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return lhs;

    NodeRes rhs = interpret_expr(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;

    return NodePtr(new ToType(
        std::move(lhs.res), std::move(rhs.res), comp.src
    ));
}

Res<Nodes> InterpreterImpl::interpret_block(Comp&& comp) {
    Nodes code;
    for(Comp& comp: comp.comps) {
        NodeRes res = interpret_(std::move(comp));
        if (res.is_err)
            return std::move(res.err);
        code.push_back(std::move(res.res));
    }
    return code;
}

Res<Nodes> InterpreterImpl::interpret_body(Comp&& comp) {
    if (comp.op == OpID::BLOCK)
        return interpret_block(std::move(comp));

    NodeRes res = interpret_(std::move(comp));
    if (res.is_err)
        return std::move(res.err);
    Nodes nodes;
    nodes.push_back(std::move(res.res));
    return nodes;
}

NodeRes InterpreterImpl::interpret_break(Comp&& comp) {
    return NodePtr(new Break(comp.src));
}

NodeRes InterpreterImpl::interpret_call(Comp&& comp) {
    NodeRes callee = interpret_expr(std::move(comp.bin->lhs));
    if (callee.is_err)
        return callee;

    Res<Args> args = interpret_args(std::move(comp.bin->rhs));
    if (args.is_err)
        return std::move(args.err);

    if (comp.bin->rhs.op == OpID::ENCLOSURE)
        return ErrPtr(new ExpectedParentheticalErr(comp.span()));

    if (comp.bin->rhs.op == OpID::LIST)
        return NodePtr(new CachedCall(
            std::move(callee.res),
            std::move(args.res),
            comp.src
        ));

    return NodePtr(new Call(
        std::move(callee.res),
        std::move(args.res),
        comp.src
    ));
}

Res<Case> InterpreterImpl::interpret_case(Comp&& comp) {
    ErrPtr err = require_pred_and_body(comp);
    if (err)
        return err;
    
    NodeRes pred = interpret_expr(std::move(comp.comp->bin->lhs));
    if (pred.is_err)
        return std::move(pred.err);
    
    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    return Case(std::move(pred.res), std::move(body.res), comp.src);
}

NodeRes InterpreterImpl::interpret_concat(Comp&& comp) {
    if (
        comp.bin->lhs.op != OpID::STRING ||
        comp.bin->rhs.op != OpID::STRING
    )
        return ErrPtr(new ExpectedStringErr(comp.src));
    return NodePtr(new String(
        std::get<std::string>(comp.bin->lhs.data) +
            std::get<std::string>(comp.bin->rhs.data),
        comp.src
    ));
}

NodeRes InterpreterImpl::interpret_construct(std::vector<Comp>&& comps) {
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
        return ErrPtr(new UnexpectedOpErr(comps[0]));
    }
}

NodeRes InterpreterImpl::interpret_continue(Comp&& comp) {
    return NodePtr(new Continue(comp.src));
}

Res<Nodes> InterpreterImpl::interpret_csv(Comp&& comp) {
    Nodes nodes;
    Comp* next;

    if (comp.op == OpID::SEP) {
        Res<Nodes> res = interpret_csv(std::move(comp.bin->lhs));
        if (res.is_err)
            return res;

        nodes = std::move(res.res);
        next = &comp.bin->rhs;
    } else
        next = &comp;

    if (next->op == OpID::UNPACK_ARGS) {
        NodeRes expansion = interpret_unop<Expansion>(std::move(*next));
        if (expansion.is_err)
            return std::move(expansion.err);
        nodes.push_back(std::move(expansion.res));
    } else {
        NodeRes value = interpret_expr(std::move(*next));
        if (value.is_err)
            return std::move(value.err);
        nodes.push_back(std::move(value.res));
    }
    return nodes;
}

template<typename ToType>
NodePtr InterpreterImpl::interpret_data(Comp&& comp) {
    return NodePtr(new ToType(
        std::get<typename ToType::Type>(std::move(comp.data)), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_declare(Comp&& comp) {
    NodeRes id = interpret_definable(std::move(comp.bin->lhs));
    if (id.is_err)
        return std::move(id.err);
    NodeRes type = interpret_expr(std::move(comp.bin->rhs));
    if (type.is_err)
        return type;
    return NodePtr(new Declare(
        std::move(id.res), std::move(type.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_def(std::vector<Comp>&& comps) {
    Res<std::pair<NodePtr, DefCase>> first = interpret_first_def_case(
        std::move(comps[0])
    );
    if (first.is_err)
        return std::move(first.err);

    std::vector<DefCase> cases;
    cases.push_back(std::move(first.res.second));
    for (std::uint32_t i = 1; i < comps.size(); i++) {
        if (comps[i].op != OpID::CASE)
            return ErrPtr(new ExpectedCaseErr(comps[i].src));
        Res<DefCase> def_case =
            interpret_def_case(std::move(comps[i]));
        if (def_case.is_err)
            return std::move(def_case.err);
        cases.push_back(std::move(def_case.res));
    }
    return NodePtr(new Def(
        std::move(first.res.first), std::move(cases), comps[0].src
    ));
}

Res<DefCase> InterpreterImpl::interpret_def_case(Comp&& comp) {
    Res<std::tuple<Comp*, NodePtr, NodePtr>> pre = interpret_def_case_pre(comp);
    if (pre.is_err)
        return std::move(pre.err);
    auto [pred_comp, returns, guard] = std::move(pre.res);

    Res<std::pair<MatchArgs, Nodes>> post = interpret_def_case_post(
        std::move(comp), std::move(*pred_comp)
    );
    if (post.is_err)
        return std::move(post.err);
    auto [spec, body] = std::move(post.res);

    return DefCase{
        std::move(spec),
        std::move(guard),
        std::move(returns),
        std::move(body),
        comp.src
    };
}

Res<std::pair<MatchArgs, Nodes>> InterpreterImpl::interpret_def_case_post(
    Comp&& comp, Comp&& args_comp
) {
    Res<MatchArgs> spec = interpret_match_args(std::move(args_comp), true);
    if (spec.is_err)
        return std::move(spec.err);

    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);
    return std::pair(std::move(spec.res), std::move(body.res));
}

Res<std::tuple<Comp*, NodePtr, NodePtr>>
InterpreterImpl::interpret_def_case_pre(Comp& comp) {
    ErrPtr err = require_pred_and_body(comp);
    if (err)
        return err;
    NodePtr returns = nullptr;
    Comp* pred_comp = &comp.comp->bin->lhs;
    if (pred_comp->op == OpID::RETURNS) {
        NodeRes returns_res = interpret_expr(
            std::move(pred_comp->bin->rhs)
        );
        if (returns_res.is_err)
            return std::move(returns_res.err);
        returns = std::move(returns_res.res);
        pred_comp = &pred_comp->bin->lhs;
    }
    NodePtr guard = nullptr;
    if (pred_comp->op == OpID::TERNARY_IF) {
        NodeRes guard_res =
            interpret_expr(std::move(pred_comp->bin->rhs));
        if (guard_res.is_err)
            return std::move(guard_res.err);
        guard = std::move(guard_res.res);
        pred_comp = &pred_comp->bin->lhs;
    }

    return std::tuple(pred_comp, std::move(returns), std::move(guard));
}

NodeRes InterpreterImpl::interpret_definable(Comp&& comp) {
    if (comp.op == OpID::GET) {
        NodeRes lhs = comp.bin->lhs.op == OpID::PLAIN_INT ?
            interpret_num_id(std::move(comp)):
            interpret_expr(std::move(comp));
        if (lhs.is_err)
            return lhs;

        NodeRes rhs = interpret_general_id(std::move(comp));
        if (rhs.is_err)
            return rhs;

        return NodePtr(new GetAttr(
            std::move(lhs.res), std::move(rhs.res), comp.src
        ));
    }
    return interpret_general_id(std::move(comp));
}

NodeRes InterpreterImpl::interpret_dflt(Comp&& comp) {
    using enum OpID;
    switch(comp.bin->lhs.op) {
    case BIND:
        return ErrPtr(new TwiceDefaultedErr(comp.span()));
    case UNPACK_ARGS:
        return ErrPtr(new DefaultedVarArgsErr(comp.span()));
    case UNPACK_KWARGS:
        return ErrPtr(new DefaultedVarKeywordArgsErr(comp.span()));
    default:;
    }
    NodeRes lhs = interpret_matcher(std::move(comp.bin->lhs), MatchKind::DEF);
    if (lhs.is_err)
        return lhs;
    NodeRes rhs = interpret_expr(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;
    return NodePtr(new Defaulted(
        std::move(lhs.res), std::move(rhs.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_enclosure(Comp&& comp) {
    Res<Nodes> elements = interpret_enclosure_recurse(
        std::move(*comp.comp)
    );
    if (elements.is_err)
        return std::move(elements.err);
    if (is_map(elements.res))
        return NodePtr(new Map(std::move(elements.res), comp.src));
    return NodePtr(new Set(std::move(elements.res), comp.src));
}

Res<Nodes> InterpreterImpl::interpret_enclosure_recurse(Comp&& comp) {
    using enum OpID;

    switch(comp.op) {
    case SEP: {
        Res<Nodes> elements =
            interpret_enclosure_recurse(std::move(comp.bin->lhs));
        if (elements.is_err)
            return elements;
        if (is_map(elements.res)) {
            NodeRes element = interpret_map_element(
                std::move(comp.bin->rhs)
            );
            if (element.is_err)
                return std::move(element.err);
            elements.res.push_back(std::move(element.res));
            return elements;
        }
        NodeRes element =
            interpret_arg(std::move(comp.bin->rhs));
        if (element.is_err)
            return std::move(element.err);
        elements.res.push_back(std::move(element.res));
        return elements;
    }
    case BIND:
    case UNPACK_KWARGS: {
        auto elements = Nodes();
        NodeRes element = interpret_map_element(std::move(comp));
        if (element.is_err)
            return std::move(element.err);
        elements.push_back(std::move(element.res));
        return elements;
    }
    case NOTHING:
        return Nodes();
    case UNPACK_ARGS:
    default: {
        auto elements = Nodes();
        NodeRes element = interpret_arg(std::move(comp));
        if (element.is_err)
            return std::move(element.err);
        elements.push_back(std::move(element.res));
        return elements;
    }}
}

NodeRes InterpreterImpl::interpret_expr(Comp&& comp) {
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
    case CONCAT:
        return interpret_concat(std::move(comp));
    case DIV:
        return interpret_binop<Divide>(std::move(comp));
    case ENCLOSURE:
        return interpret_enclosure(std::move(comp));
    case EQ:
        return interpret_binop<Equals>(std::move(comp));
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
        return NodePtr(new dl::ID(as_id(std::move(comp))));
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
        return ErrPtr(new ExpectedExprErr(comp.span()));
    }
}

Res<std::pair<NodePtr, DefCase>> InterpreterImpl::interpret_first_def_case(
    Comp&& comp
) {

    Res<std::tuple<Comp*, NodePtr, NodePtr>> pre = interpret_def_case_pre(comp);
    if (pre.is_err)
        return std::move(pre.err);
    auto [pred_comp, returns, guard] = std::move(pre.res);

    if (pred_comp->op != OpID::CALL)
        return ErrPtr(new ExpectedCallErr(pred_comp->span()));
    NodeRes id = interpret_definable(std::move(pred_comp->bin->lhs));
    if (id.is_err)
        return std::move(id.err);

    Res<std::pair<MatchArgs, Nodes>> post = interpret_def_case_post(
        std::move(comp), std::move(pred_comp->bin->rhs)
    );
    if (post.is_err)
        return std::move(post.err);
    auto [spec, body] = std::move(post.res);

    return std::pair(
        std::move(id.res),
        DefCase(
            std::move(spec),
            std::move(guard),
            std::move(returns),
            std::move(body),
            comp.src
        )
    );
}

NodeRes InterpreterImpl::interpret_float_tail(Comp&& comp) {
    return parse_float(std::get<std::string>(comp.data), comp.src);
}

NodeRes InterpreterImpl::interpret_for(std::vector<Comp>&& comps) {
    ErrPtr err = require_pred_and_body(comps[0]);
    if (err)
        return err;

    if (comps[0].comp->bin->lhs.op != OpID::IN)
        return ErrPtr(new ExpectedInErr(comps[0].comp->bin->lhs.span()));

    Res<Nodes> vars =
        interpret_loop_vars(std::move(comps[0].comp->bin->lhs.bin->lhs));
    if (vars.is_err)
        return std::move(vars.err);

    NodeRes iterable = interpret_expr(std::move(
        comps[0].comp->bin->lhs.bin->rhs
    ));
    if (iterable.is_err)
        return std::move(iterable.err);

    Res<Nodes> body = interpret_body(std::move(comps[0].comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    Res<Block> orelse = interpret_optional_else(std::move(comps));
    if (orelse.is_err)
        return std::move(orelse.err);

    return NodePtr(new For(
        std::move(vars.res),
        std::move(iterable.res),
        std::move(body.res),
        std::move(orelse.res),
        comps[0].src
    ));
}

NodeRes InterpreterImpl::interpret_general_id(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ID:
        return interpret_id(std::move(comp));
    case PLAIN_INT:
        return interpret_num_id(std::move(comp));
    default:
        return ErrPtr(new ExpectedGeneralIDErr(comp.src));
    }
}

NodeRes InterpreterImpl::interpret_get(Comp&& comp) {
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
        return parse_float(s, Span(comp.bin->lhs.src, comp.bin->rhs.src));
    }
    NodeRes obj = interpret_expr(std::move(comp.bin->lhs));
    if (obj.is_err)
        return obj;
    NodeRes attr = interpret_general_id(std::move(comp.bin->rhs));
    if (attr.is_err)
        return attr;
    return NodePtr(new GetAttr(
        std::move(obj.res), std::move(attr.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_group(Comp&& comp) {
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

NodeRes InterpreterImpl::interpret_if(std::vector<Comp>&& comps) {
    auto cases = std::vector<Case>();
    Res<Case> first_case = interpret_case(std::move(comps[0]));
    if (first_case.is_err)
        return std::move(first_case.err);
    cases.push_back(std::move(first_case.res));

    std::uint32_t end = comps.size();
    Block body;
    if (comps.back().op == OpID::ELSE) {
        Res<Nodes> body_res = expect_body_only(std::move(*comps[end - 1].comp));
        if (body_res.is_err)
            return std::move(body_res.err);
        body = Block(std::move(body_res.res), comps.back().src);
        --end;
    }
    for (std::uint32_t i = 1; i < end; i++) {
        if (comps[i].op != OpID::ELIF)
            return ErrPtr(new ExpectedElifOrElseErr(comps[i].src));
        Res<Case> c = interpret_case(std::move(comps[i]));
        if (c.is_err)
            return std::move(c.err);
        cases.push_back(std::move(c.res));
    }

    return NodePtr(new If(
        std::move(cases), std::move(body), comps[0].src
    ));
}

NodeRes InterpreterImpl::interpret_kwarg(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BIND: {
        Res<dl::ID> bind_id = expect_id(std::move(comp.bin->lhs));
        if (bind_id.is_err)
            return NodePtr(new dl::ID(std::move(bind_id.res)));
        NodeRes value = interpret_expr(std::move(comp.bin->rhs));
        if (value.is_err)
            return std::move(value.err);
        return NodePtr(new KeywordArg(
            std::move(bind_id.res), std::move(value.res), comp.src
        ));
    }
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return ErrPtr(new PosAfterKeywordErr(comp.span()));
    }
}

NodeRes InterpreterImpl::interpret_lambda(Comp&& comp) {
    // Compute this before moving.
    Span total_span = comp.span();
    auto [count, arg, span] = count_op(std::move(comp), OpID::LAMBDA);

    if (arg.op == OpID::GROUP && count == 1) {
        NodeRes inner = interpret_expr(std::move(arg));
        if (inner.is_err)
            return inner;
        return NodePtr(new Lambda(std::move(inner.res), total_span));
    }

    NodePtr res;
    using enum OpID;
    switch(arg.op) {
    case LAMBDA_ARGS:
        return NodePtr(new LambdaArgs(count + 1, comp.span()));
    case LAMBDA_KWARGS:
        return NodePtr(new LambdaKeywordArgs(count + 1, comp.span()));
    default: {
        NodeRes var = interpret_general_id(std::move(arg));
        if (var.is_err)
            return var;
        return NodePtr(new LambdaVar(count, std::move(var.res), span));
    }}
    return res;
}

template<typename NodeType>
NodeRes InterpreterImpl::interpret_lambda_var_args(Comp&& comp) {
    return NodePtr(new NodeType(1, comp.src));
}

Res<Nodes> InterpreterImpl::interpret_loop_vars(Comp&& comp) {
    auto vars = Nodes();
    NodeRes var;

    Comp* c = &comp;
    if (comp.op == OpID::LOOP_VAR_SEP) {
        Res<Nodes> vars_res = interpret_loop_vars(std::move(comp.bin->lhs));
        if (vars_res.is_err)
            return vars_res;
        vars = std::move(vars_res.res);
        c = &comp.bin->rhs;
    }

    using enum OpID;
    switch(c->op) {
    case ID:
        var = interpret_id(std::move(*c));
        break;
    case PLACEHOLDER:
        var = NodePtr(new Placeholder(c->src));
        break;
    default:
        return ErrPtr(new ExpectedCommaSeparatedLoopVarsErr(c->span()));
    }
    if (var.is_err)
        return std::move(var.err);
    vars.push_back(std::move(var.res));
    return vars;
}

NodeRes InterpreterImpl::interpret_map_element(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BIND:
        return interpret_binop<Entry>(std::move(comp));
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return ErrPtr(new ExpectedEntryOrVarKeywordArgsErr(comp.span()));
    }
}

NodeRes InterpreterImpl::interpret_match(
    std::vector<Comp>&& comps
) {
    NodeRes matchee = interpret_expr(std::move(*comps[0].comp));
    if (matchee.is_err)
        return matchee;
    if (comps.size() == 1)
        return ErrPtr(new NoMatchCasesErr(comps[0].span()));
    Res<std::vector<MatchCase>> cases = interpret_match_cases(
        std::move(comps), comps.size(), OpID::CASE
    );
    if (cases.is_err)
        return std::move(cases.err);

    return NodePtr(new Match(
        std::move(matchee.res), std::move(cases.res), comps[0].src
    ));
}

Res<MatchArgs> InterpreterImpl::interpret_match_args(Comp&& comp, bool is_def) {
    if (comp.op != OpID::GROUP)
        return ErrPtr(new ExpectedParentheticalErr(comp.src));

    bool in_kw_part = false;
    Res<MatchArgs> args = interpret_match_args_recurse(
        std::move(*comp.comp), in_kw_part, is_def
    );
    if (args.is_err)
        return args;
    args.res.src = comp.src;
    return args;
}

Res<MatchArgs> InterpreterImpl::interpret_match_args_recurse(
    Comp&& comp, bool& in_kw_part, bool is_def
) {
    Res<MatchArgs> args = MatchArgs({}, {}, Span(0, 0));
    if (comp.op == OpID::NOTHING)
        return args;
    Comp* c;
    if (comp.op == OpID::SEP) {
        args = interpret_match_args_recurse(
            std::move(comp.bin->lhs), in_kw_part, is_def
        );
        if (args.is_err)
            return args;
        c = &comp.bin->rhs;
    } else
        c = &comp;

    if (c->op == OpID::POS_KW_SEP) {
        if (!is_def)
            return ErrPtr(new ExpectedExprErr(c->src));
        if (in_kw_part) {
            return ErrPtr(new ArgSepWhereKeywordArgExpectedErr(c->src));
        }
        in_kw_part = true;
        return args;
    }

    MatchKind kind = is_def ? MatchKind::DEF: MatchKind::OBJECT;
    NodeRes arg = interpret_arg_matcher(std::move(*c), kind, in_kw_part);
    if (arg.is_err)
        return std::move(arg.err);

    if (in_kw_part)
        args.res.kwargs.push_back(std::move(arg.res));
    else
        args.res.args.push_back(std::move(arg.res));
    return args;
}

NodeRes InterpreterImpl::interpret_match_as(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    using enum OpID;
    switch(comp.bin->lhs.op) {
    case AS:
        return ErrPtr(new DoubleAsErr(comp.span()));
    case TYPE_LABEL:
        return ErrPtr(new TypeMatchWithAsErr(comp.span()));
    case UNPACK_ARGS:
    case UNPACK_KWARGS:
        return ErrPtr(new VarArgsWithAsErr(comp.span()));
    default:;
    }
    NodeRes lhs = interpret_arg_matcher(
        std::move(comp.bin->lhs), kind, in_kw_part
    );
    if (lhs.is_err)
        return lhs;

    NodeRes rhs = interpret_match_general_var(
        std::move(comp.bin->rhs), kind, in_kw_part
    );
    if (rhs.is_err)
        return rhs;
    return NodePtr(new As(std::move(lhs.res), std::move(rhs.res), comp.src));
}

NodeRes InterpreterImpl::interpret_match_bind(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    switch(kind) {
    case MatchKind::DEF:
        return interpret_dflt(std::move(comp));
    case MatchKind::OBJECT:
        in_kw_part = true;
        return interpret_match_kwarg(std::move(comp));
    default:
        return ErrPtr(new UnexpectedOpErr(comp));
    }
}

Res<MatchCase> InterpreterImpl::interpret_match_case(Comp&& comp) {
    ErrPtr err = require_pred_and_body(comp);
    if (err)
        return err;

    NodeRes matcher;
    NodeRes guard;
    if (comp.comp->bin->lhs.op == OpID::TERNARY_IF) {
        matcher = interpret_matcher(
            std::move(comp.comp->bin->lhs.bin->lhs), MatchKind::PLAIN
        );
        
        guard = interpret_expr(std::move(comp.comp->bin->lhs.bin->rhs));
        if (guard.is_err)
            return std::move(guard.err);
    } else
        matcher = interpret_matcher(
            std::move(comp.comp->bin->lhs), MatchKind::PLAIN
        );
    if (matcher.is_err)
        return std::move(matcher.err);

    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);
    return MatchCase(
        std::move(matcher.res),
        std::move(guard.res),
        std::move(body.res),
        comp.src
    );
}

Res<std::vector<MatchCase>> InterpreterImpl::interpret_match_cases(
    std::vector<Comp>&& comps, std::uint32_t end, OpID op
) {
    auto cases = std::vector<MatchCase>();
    for (std::uint32_t i = 1; i < end; i++) {
        if (comps[i].op != op) {
            if (op == OpID::CASE)
                return ErrPtr(new ExpectedCaseErr(comps[i].src));
            return ErrPtr(new ExpectedExceptOrFinallyErr(comps[i].src));
        }
        Res<MatchCase> mc = interpret_match_case(std::move(comps[i]));
        if (mc.is_err)
            return std::move(mc.err);
        cases.push_back(std::move(mc.res));
    }
    return cases;
}

NodeRes InterpreterImpl::interpret_match_general_var(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    using enum OpID;
    switch(comp.op) {
    case ID:
        return interpret_id(std::move(comp));
    case UNPACK_ARGS:
        if (comp.comp->op != ID)
            return ErrPtr(new ExpectedIDErr(comp.comp->span()));
        return interpret_match_var_args(std::move(comp), kind, in_kw_part);
    case UNPACK_KWARGS:
        if (comp.comp->op != ID)
            return ErrPtr(new ExpectedIDErr(comp.comp->span()));
        return interpret_match_var_kwargs(std::move(comp), kind, in_kw_part);
    default:
        return ErrPtr(new ExpectedIDOrVarArgsErr(comp.span()));
    }
}

NodeRes InterpreterImpl::interpret_match_kwarg(Comp&& comp) {
    Res<ID> lhs = expect_id(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return std::move(lhs.err);
    NodeRes rhs = interpret_matcher(std::move(comp.bin->rhs), MatchKind::PLAIN);
    if (rhs.is_err)
        return rhs;
    return NodePtr(new MatchKeywordArg(
        std::move(lhs.res), std::move(rhs.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_match_list(Comp&& comp) {
    Res<Nodes> nodes = interpret_match_seq_recurse(std::move(*comp.comp));
    if (nodes.is_err)
        return std::move(nodes.err);
    return NodePtr(new MatchList(std::move(nodes.res), comp.src));
}

NodeRes InterpreterImpl::interpret_match_map(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new MatchMap({}, comp.src));
    Res<Nodes> nodes = interpret_match_map_recurse(std::move(*comp.comp));
    if (nodes.is_err)
        return std::move(nodes.err);
    return NodePtr(new MatchMap(std::move(nodes.res), comp.src));
}

Res<Nodes> InterpreterImpl::interpret_match_map_recurse(Comp&& comp) {
    Nodes nodes;
    Comp* c = &comp;
    if (comp.op == OpID::SEP) {
        Res<Nodes> nodes_res = interpret_match_map_recurse(
            std::move(comp.bin->lhs)
        );
        if (nodes_res.is_err)
            return nodes_res;
        nodes = std::move(nodes_res.res);
        c = &comp.bin->rhs;
    }

    NodeRes entry;

    using enum OpID;
    switch(c->op) {
    case AS:
        if (c->bin->rhs.op != UNPACK_KWARGS)
            return ErrPtr(new ExpectedEntryOrVarKeywordArgsErr(c->span()));
        entry = interpret_matcher(std::move(*c), MatchKind::MAP);
        break;
    case BIND: {
        NodeRes key = interpret_expr(std::move(c->bin->lhs));
        if (key.is_err)
            return std::move(key.err);
        NodeRes matcher =
            interpret_matcher(std::move(c->bin->rhs), MatchKind::PLAIN);
        if (matcher.is_err)
            return std::move(matcher.err);
        entry = NodePtr(new Entry(
            std::move(key.res), std::move(matcher.res), c->src
        ));
        break;
    }
    case TYPE_LABEL:
        if (c->bin->lhs.op != UNPACK_KWARGS)
            return ErrPtr(new ExpectedEntryOrVarKeywordArgsErr(c->span()));
        entry = interpret_matcher(std::move(*c), MatchKind::MAP);
        break;
    case UNPACK_KWARGS:
        entry = interpret_unop<Expansion>(std::move(*c));
        break;
    default:
        return ErrPtr(new ExpectedEntryOrVarKeywordArgsErr(c->span()));
    }
    if (entry.is_err)
        return std::move(entry.err);
    nodes.push_back(std::move(entry.res));
    return nodes;
}

NodeRes InterpreterImpl::interpret_match_obj(Comp&& comp) {
    NodeRes type = interpret_expr(std::move(comp.bin->lhs));
    if (type.is_err)
        return type;
    Res<MatchArgs> args = interpret_match_args(std::move(comp.bin->rhs), false);
    if (args.is_err)
        return std::move(args.err);
    return NodePtr(new MatchObject(
        std::move(type.res), std::move(args.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_match_options(Comp&& comp) {
    Res<Nodes> nodes = interpret_match_options_recurse(std::move(comp));
    if (nodes.is_err)
        return std::move(nodes.err);
    auto src = Span(nodes.res.front()->span(), nodes.res.back()->span());
    return NodePtr(new MatchOptions(std::move(nodes.res), src));
}

Res<Nodes> InterpreterImpl::interpret_match_options_recurse(Comp&& comp) {
    Nodes nodes;
    NodeRes matcher;
    if (comp.op == OpID::MATCH_OR) {
        Res<Nodes> nodes_res =
            interpret_match_options_recurse(std::move(comp.bin->lhs));
        if (nodes_res.is_err)
            return nodes_res;
        nodes = std::move(nodes_res.res);
        matcher = interpret_matcher(std::move(comp.bin->rhs), MatchKind::PLAIN);
    } else
        matcher = interpret_matcher(std::move(comp), MatchKind::PLAIN);
    if (matcher.is_err)
        return std::move(matcher.err);
    nodes.push_back(std::move(matcher.res));
    return nodes;
}

Res<Nodes> InterpreterImpl::interpret_match_seq_recurse(Comp&& comp) {
    Nodes nodes;
    NodeRes matcher;
    if (comp.op == OpID::NOTHING)
        return nodes;
    if (comp.op == OpID::SEP) {
        Res<Nodes> nodes_res =
            interpret_match_seq_recurse(std::move(comp.bin->lhs));
        if (nodes_res.is_err)
            return nodes_res;
        nodes = std::move(nodes_res.res);
        matcher = interpret_matcher(std::move(comp.bin->rhs), MatchKind::SEQ);
    } else
        matcher = interpret_matcher(std::move(comp), MatchKind::SEQ);
    if (matcher.is_err)
        return std::move(matcher.err);
    nodes.push_back(std::move(matcher.res));
    return nodes;
}

NodeRes InterpreterImpl::interpret_match_tuple(Comp&& comp) {
    using enum OpID;
    switch(comp.comp->op) {
    case AS:
        if (comp.comp->bin->rhs.op != UNPACK_ARGS)
            return ErrPtr(new ExpectedExprErr(comp.comp->span()));
        break;
    case NOTHING:
    case SEP:
    case UNPACK_ARGS:
        break;
    case TYPE_LABEL:
        if (comp.comp->bin->lhs.op != UNPACK_ARGS)
            return ErrPtr(new ExpectedExprErr(comp.comp->span()));
        break;
    default:
        // Parenthesized value, not a tuple.
        return interpret_expr(std::move(*comp.comp));
    }
    Res<Nodes> nodes = interpret_match_seq_recurse(std::move(*comp.comp));
    if (nodes.is_err)
        return std::move(nodes.err);
    return NodePtr(new MatchTuple(std::move(nodes.res), comp.src));
}

NodeRes InterpreterImpl::interpret_match_var_args(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    if (kind == MatchKind::MAP || kind == MatchKind::PLAIN)
        return ErrPtr(new VarArgsNotAllowedHereErr(comp.span())); 
    if (in_kw_part)
        return ErrPtr(new VarArgsWhereKeywordArgExpectedErr(comp.span()));
    return interpret_unop<Expansion>(std::move(comp));
}

NodeRes InterpreterImpl::interpret_match_var_kwargs(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    if (kind == MatchKind::SEQ || kind == MatchKind::PLAIN)
        return ErrPtr(new VarKeywordArgsNotAllowedHereErr(comp.span()));
    in_kw_part = true;
    return interpret_unop<Expansion>(std::move(comp));
}

NodeRes InterpreterImpl::interpret_matcher(Comp&& comp, MatchKind kind) {
    // Not exactly elegant, but passing in in_kw_part even when it's not needed
    // allows much code to be reused.
    bool in_kw_part = false;
    return interpret_arg_matcher(std::move(comp), kind, in_kw_part);
}

NodeRes InterpreterImpl::interpret_not_in(Comp&& comp) {
    NodeRes in = interpret_binop<In>(std::move(comp));
    if (in.is_err)
        return in;
    return NodePtr(new Not(std::move(in.res), comp.src));
}

NodeRes InterpreterImpl::interpret_num_id(Comp&& comp) {
    std::string& s = std::get<std::string>(comp.data);
    Res<std::int32_t> x =
        read_number<std::int32_t>(
            s, &s[0] + s.size(), 10, comp.src.start()
        );
    if (x.is_err)
        return std::move(x.err);
    if (x.res == 0)
        return ErrPtr(new ZeroUsedAsIDErr(comp.src));
    return NodePtr(new NumID(std::move(x.res), comp.src));
}

NodeRes InterpreterImpl::interpret_number(Comp&& comp) {
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
        return ErrPtr(new AssertionFailedErr("Bad numerical data"));
    }
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_optional(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new ToType(nullptr, comp.src));
    NodeRes res = interpret_expr(std::move(*comp.comp));
    if (res.is_err)
        return res;
    return NodePtr(new ToType(std::move(res.res), comp.src));
}

Res<Block> InterpreterImpl::interpret_optional_else(std::vector<Comp>&& comps) {
    // This function interprets an optional else clause for "for" and "while".
    if (comps.size() > 2 || (comps.size() == 2 && comps[1].op != OpID::ELSE))
        // for and while may only have at most two parts.
        return ErrPtr(new ExpectedElseErr(comps[1].src));
    if (comps.size() == 2) {
        Res<Nodes> body = expect_body_only(std::move(*comps[1].comp));
        if (body.is_err)
            return std::move(body.err);
        return Block(std::move(body.res), comps[1].src);
    }
    // Use an empty Block to indicate lack of else and no error.
    return Block();
}

NodeRes InterpreterImpl::interpret_plain_int(Comp&& comp) {
    std::string& s = std::get<std::string>(comp.data);
    Res<std::int32_t> x = read_number<std::int32_t>(
        s, &s[0] + s.size(), 10, comp.src.start()
    );
    if (x.is_err)
        return std::move(x.err);
    return NodePtr(new Int32(std::move(x.res), comp.src));
}

NodeRes InterpreterImpl::interpret_sep(Comp&& comp) {
    Res<Nodes> elements = interpret_csv(std::move(comp));
    if (elements.is_err)
        return std::move(elements.err);
    return NodePtr(new Tuple(std::move(elements.res), comp.span()));
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_seq(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return NodePtr(new ToType({}, comp.src));
    Res<Nodes> nodes = interpret_csv(std::move(*comp.comp));
    if (nodes.is_err)
        return std::move(nodes.err);
    return NodePtr(new ToType(std::move(nodes.res), comp.src));
}

NodeRes InterpreterImpl::interpret_symbol(Comp&& comp) {
    NodeRes id = interpret_general_id(std::move(*comp.comp));
    if (id.is_err)
        return id;
    return NodePtr(new Symbol(std::move(id.res), comp.span()));
}

NodeRes InterpreterImpl::interpret_ternary(Comp&& comp) {
    if (comp.bin->lhs.op != OpID::TERNARY_IF)
        return ErrPtr(new ExpectedTernaryIfErr(comp.bin->lhs.span()));
    
    NodeRes predicate = interpret_expr(std::move(comp.bin->lhs.bin->rhs));
    if (predicate.is_err)
        return predicate;

    NodeRes if_case = interpret_expr(std::move(comp.bin->lhs.bin->lhs));
    if (if_case.is_err)
        return if_case;

    NodeRes else_case = interpret_expr(std::move(comp.bin->rhs));
    if (else_case.is_err)
        return std::move(else_case.err);

    return NodePtr(new Ternary(
        std::move(predicate.res),
        std::move(if_case.res),
        std::move(else_case.res),
        comp.bin->lhs.src
    ));
}

NodeRes InterpreterImpl::interpret_try(std::vector<Comp>&& comps) {
    if (comps.size() == 1)
        return ErrPtr(new IsolatedTryErr(comps[0].span()));

    Res<Nodes> body = expect_body_only(std::move(*comps[0].comp));
    if (body.is_err)
        return std::move(body.err);

    Nodes finally;
    std::uint32_t end = comps.size();
    if (comps.back().op == OpID::FINALLY) {
        Res<Nodes> finally_body =
            expect_body_only(std::move(*comps.back().comp));
        if (finally_body.is_err)
            return std::move(finally_body.err);
        finally = std::move(finally_body.res);

        // Exclude last comp from cases.
        end--;
    }
    Res<std::vector<MatchCase>> cases = interpret_match_cases(
        std::move(comps), end, OpID::EXCEPT
    );

    if (cases.is_err)
        return std::move(cases.err);

    auto try_block = Block(std::move(body.res), comps[0].src);
    Block finally_block = finally.size() > 0 ?
        Block(std::move(finally), comps.back().src): Block();

    return NodePtr(new Try(
        std::move(try_block),
        std::move(cases.res),
        std::move(finally_block),
        comps[0].src
    ));
}

NodeRes InterpreterImpl::interpret_type_match(
    Comp&& comp, MatchKind kind, bool& in_kw_part
) {
    NodeRes var = interpret_match_general_var(
        std::move(comp.bin->lhs), kind, in_kw_part
    );
    if (var.is_err)
        return var;
    NodeRes type = interpret_expr(std::move(comp.bin->rhs));
    if (type.is_err)
        return type;
    return NodePtr(new TypeMatch(
        std::move(var.res), std::move(type.res), comp.src
    ));
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_unop(Comp&& comp) {
    NodeRes arg = interpret_expr(std::move(*comp.comp));
    if (arg.is_err)
        return arg;
    return NodePtr(new ToType(std::move(arg.res), comp.src));
}

NodeRes InterpreterImpl::interpret_up(Comp&& comp) {
    auto [count, var, span] = count_op(std::move(comp), OpID::UP);
    NodeRes id = interpret_general_id(std::move(var));
    if (id.is_err)
        return id;
    return NodePtr(new Var(count, std::move(id.res), span));
}

NodeRes InterpreterImpl::interpret_while(std::vector<Comp>&& comps) {
    ErrPtr err = require_pred_and_body(comps[0]);
    if (err)
        return err;

    NodeRes pred = interpret_expr(std::move(comps[0].comp->bin->lhs));
    if (pred.is_err)
        return std::move(pred.err);

    Res<Nodes> body = interpret_body(std::move(comps[0].comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    Res<Block> orelse = interpret_optional_else(std::move(comps));
    if (orelse.is_err)
        return std::move(orelse.err);

    return NodePtr(new While(
        std::move(pred.res),
        std::move(body.res),
        std::move(orelse.res),
        comps[0].src
    ));
}

bool InterpreterImpl::is_map(const Nodes& nodes) {
    return
        nodes.empty() || nodes[0]->kind() == NodeKind::ENTRY;
}

NodeRes InterpreterImpl::parse_float(const std::string& str, Span src) {
    const char* begin = &str[0];
    LiteralSuffix ls = lit_suffix(str);
    const char* expected_end = begin + str.size() - lit_suffix_len(ls);
    if (ls == LiteralSuffix::F32) {
        Res<float> res = read_number<float>(
            str, expected_end, 10, src.start()
        );
        if (res.is_err)
            return std::move(res.err);
        return NodePtr(new Float32(std::move(res.res), src));
    }
    Res<double> res = read_number<double>(str, expected_end, 10, src.start());
    if (res.is_err)
        return std::move(res.err);
    return NodePtr(new Float64(std::move(res.res), src));
}

ErrPtr InterpreterImpl::require_pred_and_body(const Comp& comp) {
    if (comp.comp->op != OpID::LABEL) {
        if (comp.comp->op == OpID::BODY)
            return ErrPtr(new MissingPredicateErr(comp.src));
        return ErrPtr(new MissingBodyErr(comp.span()));
    }
    return nullptr;
}

}
