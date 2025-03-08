#pragma once

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>

#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "dl/err.hpp"
#include "dl/compose/comp.hpp"
#include "dl/convert.hpp"
#include "dl/interpret/argdef.hpp"
#include "dl/interpret/args.hpp"
#include "dl/interpret/argspec.hpp"
#include "dl/interpret/binary.hpp"
#include "dl/interpret/call.hpp"
#include "dl/interpret/callattr.hpp"
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
#include "dl/interpret/node.hpp"
#include "dl/interpret/nullary.hpp"
#include "dl/interpret/seq.hpp"
#include "dl/interpret/setattr.hpp"
#include "dl/interpret/ternary.hpp"
#include "dl/interpret/try.hpp"
#include "dl/interpret/type.hpp"
#include "dl/interpret/unary.hpp"
#include "dl/interpret/update.hpp"
#include "dl/interpret/updateattr.hpp"
#include "dl/interpret/while.hpp"
#include "dl/lex/literalsuffix.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

namespace dl {

struct ArgAfterVarKeywordArgsErr final: SourcedErr {
    ArgAfterVarKeywordArgsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ArgAfterVarKeywordArgsErr";
    }
};

struct DefaultedVarArgsErr final: SourcedErr {
    DefaultedVarArgsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DefaultedVarArgsErr";
    }
};

struct DefaultedVarKeywordArgsErr final: SourcedErr {
    DefaultedVarKeywordArgsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DefaultedVarKeywordArgsErr";
    }
};

struct DuplicateArgErr final: SourcedErr {
    std::string arg;

    DuplicateArgErr(std::string&& arg, Pos src) noexcept:
    SourcedErr(src), arg(std::move(arg)) {}

    std::ostream& out_data(std::ostream& os) const override {
        return SourcedErr::out_data(os) << ", " << arg;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DuplicateArgErr";
    }
};

struct DuplicateKeywordErr final: SourcedErr {
    std::string keyword;

    DuplicateKeywordErr(std::string&& keyword, Pos src) noexcept:
    SourcedErr(src), keyword(std::move(keyword)) {}

    std::ostream& out_data(std::ostream& os) const override {
        return SourcedErr::out_data(os) << ", " << keyword;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "DuplicateKeywordErr";
    }
};

struct ExpectedCallErr final: SourcedErr {
    ExpectedCallErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCallErr";
    }
};

struct ExpectedCaseErr final: SourcedErr {
    ExpectedCaseErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCaseErr";
    }
};

struct ExpectedCaseOrElseErr final: SourcedErr {
    ExpectedCaseOrElseErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCaseOrElseErr";
    }
};

struct ExpectedColonErr final: SourcedErr {
    ExpectedColonErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedColonErr";
    }
};

struct ExpectedCommaSeparatedLoopVarsErr final: SourcedErr {
    ExpectedCommaSeparatedLoopVarsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCommaSeparatedLoopVarsErr";
    }
};

struct ExpectedCompoundIDErr final: SourcedErr {
    ExpectedCompoundIDErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedCompoundIDErr";
    }
};

struct ExpectedElifOrElseErr final: SourcedErr {
    ExpectedElifOrElseErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElifOrElseErr";
    }
};

struct ExpectedElseErr final: SourcedErr {
    ExpectedElseErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedElseErr";
    }
};

struct ExpectedEntryOrVarKeywordArgsErr final: SourcedErr {
    ExpectedEntryOrVarKeywordArgsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedEntryOrVarKeywordArgsErr";
    }
};

struct ExpectedExceptOrFinallyErr final: SourcedErr {
    ExpectedExceptOrFinallyErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExceptOrFinallyErr";
    }
};

struct ExpectedExprErr final: SourcedErr {
    ExpectedExprErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedExprErr";
    }
};

struct ExpectedIDErr final: SourcedErr {
    ExpectedIDErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDErr";
    }
};

struct ExpectedIDOrCallErr final: SourcedErr {
    ExpectedIDOrCallErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrCallErr";
    }
};

struct ExpectedIDOrParentheticalErr final: SourcedErr {
    ExpectedIDOrParentheticalErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrParentheticalErr";
    }
};

struct ExpectedIDOrVarArgsErr final: SourcedErr {
    ExpectedIDOrVarArgsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedIDOrVarArgsErr";
    }
};

struct ExpectedGeneralIDErr final: SourcedErr {
    ExpectedGeneralIDErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedGeneralIDErr";
    }
};

struct ExpectedInErr final: SourcedErr {
    ExpectedInErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedInErr";
    }
};

struct ExpectedParentheticalErr final: SourcedErr {
    ExpectedParentheticalErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedParentheticalErr";
    }
};

struct ExpectedRoundOrSquareBracketsErr final: SourcedErr {
    ExpectedRoundOrSquareBracketsErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedRoundOrSquareBracketsErr";
    }
};

struct ExpectedStringErr final: SourcedErr {
    ExpectedStringErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedStringErr";
    }
};

struct ExpectedTernaryIfErr final: SourcedErr {
    ExpectedTernaryIfErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ExpectedTernaryIfErr";
    }
};

struct IsolatedTryErr final: SourcedErr {
    IsolatedTryErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "IsolatedTryErr";
    }
};

struct MissingBodyErr final: SourcedErr {
    MissingBodyErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingBodyErr";
    }
};

struct MissingPredicateErr final: SourcedErr {
    MissingPredicateErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "MissingPredicateErr";
    }
};

struct NoMatchCasesErr final: SourcedErr {
    NoMatchCasesErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "NoMatchCasesErr";
    }
};

struct NonDefaultFollowsDefaultErr final: SourcedErr {
    NonDefaultFollowsDefaultErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "NonDefaultFollowsDefaultErr";
    }
};

struct PosAfterKeywordErr final: SourcedErr {
    PosAfterKeywordErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "PosAfterKeywordErr";
    }
};

struct TwiceDefaultedErr final: SourcedErr {
    TwiceDefaultedErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "TwiceDefaultedErr";
    }
};

struct UnexpectedCompErr final: SourcedErr {
    OpID op;

    UnexpectedCompErr(Pos src, OpID op) noexcept:
    SourcedErr(src), op(op) {}

    UnexpectedCompErr(const Comp& comp) noexcept:
    UnexpectedCompErr(comp.src, comp.op) {}

    bool equals(const Err& err) const noexcept override {
        return
            SourcedErr::equals(err) &&
            op == dynamic_cast<const UnexpectedCompErr&>(err).op;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedCompErr";
    }
};

struct UnexpectedContinuationErr final: SourcedErr {
    UnexpectedContinuationErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedContinuationErr";
    }
};

struct UnexpectedPredicateErr final: SourcedErr {
    UnexpectedPredicateErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "UnexpectedPredicateErr";
    }
};

struct VarArgsFollowsDefaultErr final: SourcedErr {
    VarArgsFollowsDefaultErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarArgsFollowsDefaultErr";
    }
};

struct VarArgsWhereKeywordArgExpectedErr final: SourcedErr {
    VarArgsWhereKeywordArgExpectedErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "VarArgsWhereKeywordArgExpectedErr";
    }
};

struct ZeroUsedAsIDErr final: SourcedErr {
    ZeroUsedAsIDErr(Pos src) noexcept: SourcedErr(src) {}

    std::ostream& out_name(std::ostream& os) const override {
        return os << "ZeroUsedAsIDErr";
    }
};

enum class ArgDefKind {
    ARG,
    VAR_ARGS,
    VAR_KWARGS
};

struct ArgDefWithKind {
    ArgDef def;
    ArgDefKind kind;

    ArgDefWithKind(ArgDef&& def, ArgDefKind kind) noexcept:
    def(std::move(def)), kind(kind) {}
};

struct InterpreterImpl final: Interpreter {
    static inline NodeRes interpret_(Comp&& comp);
    NodeRes interpret(Comp&& comp) override {
        return interpret_(std::move(comp));
    }

    static inline ID as_id(Comp&& comp);
    static inline std::pair<std::uint32_t, Comp> count_op(Comp&& comp, OpID op);
    static inline Res<Nodes> expect_body_only(Comp&& comp);
    static inline Res<ID> expect_id(Comp&& comp);
    static inline NodeRes interpret_assign(Comp&& comp);
    static inline NodeRes interpret_arg(Comp&& comp);
    static inline Res<ArgDefWithKind> interpret_arg_def(Comp&& comp);
    static inline Res<ArgSpec> interpret_arg_spec(Comp&& comp);
    static inline ErrPtr interpret_arg_spec_recurse(
        Comp&& comp,
        ArgSpec& spec,
        std::unordered_set<std::string>& arg_set,
        bool& in_kw_part,
        bool& found_pos_dflt
    );
    static inline Res<Args> interpret_args(Comp&& comp);
    static inline ErrPtr interpret_args_recurse(
        Comp&& comp,
        Nodes& args,
        Nodes& kwargs,
        bool& in_kw_part,
        std::unordered_set<std::string>& kw_set
    );
    static inline Res<ArgDefWithKind> interpret_basic_arg_def(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_binop(Comp&& comp);

    static inline NodeRes interpret_binop_method(
        Comp&& comp, std::string fn_name
    );
    static inline Res<Nodes> interpret_body(Comp&& comp);
    static inline Res<Nodes> interpret_block(Comp&& comp);
    static inline NodeRes interpret_call(Comp&& comp);
    static inline Res<Case> interpret_case(Comp&& comp);
    static inline Res<std::pair<std::vector<Case>, Nodes>> interpret_cases(
        std::vector<Comp>&& comps,
        std::uint32_t start,
        OpID construct
    );
    static inline NodeRes interpret_concat(Comp&& comp);
    static inline NodeRes interpret_construct(std::vector<Comp>&& comps);

    static inline Res<Nodes> interpret_csv(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_data(Comp&& comp);
    static inline NodeRes interpret_declare(Comp&& comp);
    static inline NodeRes interpret_def(std::vector<Comp>&& comps);
    static inline Res<DefCase> interpret_def_case(Comp&& comp);
    static inline Res<ArgDef> interpret_defaulted_arg_def(Comp&& comp);
    static inline NodeRes interpret_definable(Comp&& comp);
    static inline NodeRes interpret_enclosure(Comp&& comp);
    static inline Res<Nodes> interpret_enclosure_recurse(Comp&& comp);
    static inline NodeRes interpret_entry(Comp&& comp);
    static inline NodeRes interpret_expansion_or_value(Comp&& comp);
    static inline Res<std::pair<NodePtr, DefCase>> interpret_first_def_case(
        Comp&& comp
    );
    static inline NodeRes interpret_float_tail(Comp&& comp);
    static inline NodeRes interpret_for(std::vector<Comp>&& comps);
    static inline NodeRes interpret_general_id(Comp&& comp);
    static inline NodeRes interpret_get(Comp&& comp);
    static inline NodeRes interpret_group(Comp&& comp);
    static inline NodeRes interpret_id(Comp&& comp);
    static inline NodeRes interpret_if(std::vector<Comp>&& comps);
    static inline NodeRes interpret_kwarg(
        Comp&& comp, std::unordered_set<std::string>& kw_set
    );
    static inline Res<ArgDefWithKind> interpret_labeled_arg_def(
        Comp&& comp, bool match
    );
    static inline NodeRes interpret_lambda(Comp&& comp);
    static inline Res<std::vector<ID>> interpret_loop_vars(Comp&& comp);
    static inline NodeRes interpret_map_element(Comp&& comp);
    static inline NodeRes interpret_match(std::vector<Comp>&& comps);
    static inline NodeRes interpret_not_in(Comp&& comp);
    static inline NodeRes interpret_num_id(Comp&& comp);
    static inline NodeRes interpret_number(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_optional(Comp&& comp);
    static inline Res<Nodes> interpret_optional_else(std::vector<Comp>&& comps);
    static inline NodeRes interpret_plain_int(Comp&& comp);
    static inline NodeRes interpret_sep(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_seq(Comp&& comp);
    static inline NodeRes interpret_symbol(Comp&& comp);
    static inline NodeRes interpret_ternary(Comp&& comp);
    static inline NodeRes interpret_try(std::vector<Comp>&& comps);
    static inline NodeRes interpret_type(Comp&& comp);

    template<typename ToType>
    static inline NodeRes interpret_unop(Comp&& comp);

    static inline NodeRes interpret_unop_method(
        Comp&& comp, std::string fn_name
    );
    static inline NodeRes interpret_up(Comp&& comp);
    static inline NodeRes interpret_value(Comp&& comp);
    static inline NodeRes interpret_while(std::vector<Comp>&& comps);
    static inline bool is_map(const Nodes& nodes);
    static inline NodeRes parse_float(const std::string& str, Pos src);
    static inline ErrPtr require_pred_and_body(const Comp& comp);
};

ID InterpreterImpl::as_id(Comp&& comp) {
    return ID(std::move(std::get<std::string>(comp.data)), comp.src);
}

std::pair<std::uint32_t, Comp> InterpreterImpl::count_op(Comp&& comp, OpID op) {
    std::uint32_t count = 0;
    Comp* c = &comp;
    while (c->op == op) {
        count++;
        c = c->comp;
    }
    return {count, std::move(*c)};
}

Res<Nodes> InterpreterImpl::expect_body_only(Comp&& comp) {
    if (comp.op != OpID::BODY) {
        if (comp.op == OpID::LABEL)
            return ErrPtr(new UnexpectedPredicateErr(comp.src));
        return ErrPtr(new ExpectedColonErr(comp.src));
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
        return NodePtr(new Break(comp.src));
    case CONTINUE:
        return NodePtr(new Continue(comp.src));
    case CONSTRUCT:
        return interpret_construct(std::move(comp.comps));
    case IADD:
        return interpret_binop_method(std::move(comp), "__iadd__");
    case IBAND:
        return interpret_binop_method(std::move(comp), "__iband__");
    case IBOR:
        return interpret_binop_method(std::move(comp), "__ibor__");
    case IBXOR:
        return interpret_binop_method(std::move(comp), "__ibxor__");
    case IDIV:
        return interpret_binop_method(std::move(comp), "__idiv__");
    case ILSH:
        return interpret_binop_method(std::move(comp), "__ilsh__");
    case IMOD:
        return interpret_binop_method(std::move(comp), "__imod__");
    case IMUL:
        return interpret_binop_method(std::move(comp), "__imul__");
    case IPOW:
        return interpret_binop_method(std::move(comp), "__ipow__");
    case IRSH:
        return interpret_binop_method(std::move(comp), "__irsh__");
    case ISUB:
        return interpret_binop_method(std::move(comp), "__isub__");
    case RAISE:
        return interpret_optional<Raise>(std::move(comp));
    case RETURN:
        return interpret_optional<Return>(std::move(comp));
    case SET:
        return interpret_assign(std::move(comp));
    case TYPE:
        return interpret_type(std::move(comp));
    case TYPE_LABEL:
        return interpret_declare(std::move(comp));
    default:
        return interpret_value(std::move(comp));
    }
}

NodeRes InterpreterImpl::interpret_arg(Comp&& comp) {
    if (comp.op == OpID::UNPACK_ARGS)
        return interpret_unop<Expansion>(std::move(comp));
    return interpret_value(std::move(comp));
}

Res<ArgDefWithKind> InterpreterImpl::interpret_arg_def(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case BIND: {
        Res<ArgDef> arg_def = interpret_defaulted_arg_def(std::move(comp));
        if (arg_def.is_err)
            return std::move(arg_def.err);
        return ArgDefWithKind(std::move(arg_def.res), ArgDefKind::ARG);
    }
    case TYPE_LABEL:
        return interpret_labeled_arg_def(std::move(comp), false);
    case MATCHING:
        return interpret_labeled_arg_def(std::move(comp), true);
    default:
        return interpret_basic_arg_def(std::move(comp));
    }
}

Res<ArgSpec> InterpreterImpl::interpret_arg_spec(Comp&& comp) {
    if (comp.op != OpID::GROUP)
        return ErrPtr(new ExpectedParentheticalErr(comp.src));

    ArgSpec spec({}, {}, ArgDef(), ArgDef(), comp.src);

    if (comp.comp->op == OpID::NOTHING)
        return spec;

    auto arg_set = std::unordered_set<std::string>();
    bool in_kw_part = false;
    bool found_pos_dflt = false;
    
    ErrPtr err = interpret_arg_spec_recurse(
        std::move(*comp.comp), spec, arg_set, in_kw_part, found_pos_dflt
    );
    if (err)
        return err;

    return spec;
}

ErrPtr InterpreterImpl::interpret_arg_spec_recurse(
    Comp&& comp,
    ArgSpec& spec,
    std::unordered_set<std::string>& arg_set,
    bool& in_kw_part,
    bool& found_pos_dflt
) {
    if (spec.var_kwargs.id.data != "")
        return ErrPtr(new ArgAfterVarKeywordArgsErr(comp.src));
    if (comp.op == OpID::SEP) {
        ErrPtr err = interpret_arg_spec_recurse(
            std::move(comp.bin->lhs), spec, arg_set, in_kw_part, found_pos_dflt
        );
        if (err)
            return err;
        return interpret_arg_spec_recurse(
            std::move(comp.bin->rhs), spec, arg_set, in_kw_part, found_pos_dflt
        );
    }

    if (!in_kw_part && comp.op == OpID::POS_KW_SEP) {
        in_kw_part = true;
        return nullptr;
    }

    Res<ArgDefWithKind> def = interpret_arg_def(std::move(comp));
    if (def.is_err)
        return std::move(def.err);

    if (def.res.def.id.data != "") {
        if (arg_set.find(def.res.def.id.data) != arg_set.end())
            return ErrPtr(new DuplicateArgErr(
                std::move(def.res.def.id.data), def.res.def.id.src
            ));
        arg_set.insert(def.res.def.id.data);
    }

    using enum ArgDefKind;
    switch(def.res.kind) {
    case ARG:
        if (in_kw_part)
            spec.kwargs.push_back(std::move(def.res.def));
        else {
            if (found_pos_dflt && def.res.def.dflt == nullptr)
                return ErrPtr(new NonDefaultFollowsDefaultErr(comp.src));
            else if (def.res.def.dflt != nullptr)
                found_pos_dflt = true;
            spec.args.push_back(std::move(def.res.def));
        }
        return nullptr;
    case VAR_ARGS:
        if (in_kw_part)
            return ErrPtr(new VarArgsWhereKeywordArgExpectedErr(comp.src));
        if (found_pos_dflt)
            return ErrPtr(new VarArgsFollowsDefaultErr(comp.src));
        spec.var_args = std::move(def.res.def);
        in_kw_part = true;
        return nullptr;
    case VAR_KWARGS:
        spec.var_kwargs = std::move(def.res.def);
        return nullptr;
    }
}

Res<Args> InterpreterImpl::interpret_args(Comp&& comp) {
    if (comp.comp->op == OpID::NOTHING)
        return Args{{}, {}, comp.src};

    Nodes args;
    Nodes kwargs;

    bool in_kw_part = false;
    std::unordered_set<std::string> kw_set;
    ErrPtr err = interpret_args_recurse(
        std::move(*comp.comp), args, kwargs, in_kw_part, kw_set
    );
    if (err)
        return err;
    return Args{std::move(args), std::move(kwargs), comp.src};
}

ErrPtr InterpreterImpl::interpret_args_recurse(
    Comp&& comp,
    Nodes& args,
    Nodes& kwargs,
    bool& in_kw_part,
    std::unordered_set<std::string>& kw_set
) {
    if (comp.op == OpID::SEP) {
        ErrPtr err = interpret_args_recurse(
            std::move(comp.bin->lhs),
            args,
            kwargs,
            in_kw_part,
            kw_set
        );
        if (err)
            return err;
        return interpret_args_recurse(
            std::move(comp.bin->rhs),
            args,
            kwargs,
            in_kw_part,
            kw_set
        );
    }

    in_kw_part =
        in_kw_part ||
        comp.op == OpID::BIND ||
        comp.op == OpID::UNPACK_KWARGS;
    if (in_kw_part) {
        NodeRes res = interpret_kwarg(std::move(comp), kw_set);
        if (res.is_err)
            return std::move(res.err);
        kwargs.push_back(std::move(res.res));
        return nullptr;
    }
    NodeRes res = interpret_arg(std::move(comp));
    if (res.is_err)
        return std::move(res.err);
    args.push_back(std::move(res.res));
    return nullptr;
}

NodeRes InterpreterImpl::interpret_assign(Comp&& comp) {
    NodeRes rhs = interpret_value(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;

    if (comp.bin->lhs.op == OpID::TYPE_LABEL) {
        // Variable initialization
        NodeRes id = interpret_definable(std::move(comp.bin->lhs.bin->lhs));
        if (id.is_err)
            return std::move(id.err);
        NodeRes type = interpret_value(std::move(comp.bin->lhs.bin->rhs));
        if (type.is_err)
            return type;
        return NodePtr(new Init(
            std::move(id.res),
            std::move(type.res),
            std::move(rhs.res),
            comp.src
        ));
    }
    NodeRes lhs = interpret_value(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return lhs;

    using enum NodeKind;
    switch(lhs.res->kind()) {
    case GET_ATTR: {
        auto& ga = dynamic_cast<GetAttr&>(*lhs.res);
        return NodePtr(new SetAttr(
            std::move(ga.lhs),
            std::move(ga.rhs),
            std::move(rhs.res),
            comp.src
        ));
    }
    case CALL: {
        auto& call = dynamic_cast<Call&>(*lhs.res);
        return NodePtr(new Update(
            std::move(call.callee),
            std::move(call.args),
            std::move(rhs.res),
            comp.src
        ));
    }
    case CALL_ATTR: {
        auto& ca = dynamic_cast<CallAttr&>(*lhs.res);
        return NodePtr(new UpdateAttr(
            std::move(ca.object),
            std::move(ca.attr),
            std::move(ca.args),
            std::move(rhs.res),
            comp.src
        ));
    }
    default:;
    }
    return NodePtr(new Assign(
        std::move(lhs.res), std::move(rhs.res), comp.src
    ));
}

Res<ArgDefWithKind> InterpreterImpl::interpret_basic_arg_def(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ID:
        return ArgDefWithKind(
            ArgDef(as_id(std::move(comp)), nullptr, nullptr, false, comp.src),
            ArgDefKind::ARG
        );
    case UNPACK_ARGS: {
        Res<dl::ID> id = expect_id(std::move(*comp.comp));
        if (id.is_err)
            return std::move(id.err);
        return ArgDefWithKind(
            ArgDef(std::move(id.res), nullptr, nullptr, false, comp.src),
            ArgDefKind::VAR_ARGS
        );
    }
    case UNPACK_KWARGS: {
        Res<dl::ID> id = expect_id(std::move(*comp.comp));
        if (id.is_err)
            return std::move(id.err);
        return ArgDefWithKind(
            ArgDef(std::move(id.res), nullptr, nullptr, false, comp.src),
            ArgDefKind::VAR_KWARGS
        );
    }
    default: {
        NodeRes matcher = interpret_value(std::move(comp));
        if (matcher.is_err)
            return std::move(matcher.err);
        return ArgDefWithKind(
            ArgDef(
                dl::ID("", Pos(0, 0)),
                std::move(matcher.res),
                nullptr,
                true,
                comp.src
            ),
            ArgDefKind::ARG
        );
    }
    }
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_binop(Comp&& comp) {
    NodeRes lhs = interpret_value(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return lhs;

    NodeRes rhs = interpret_value(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;

    return NodePtr(new ToType(
        std::move(lhs.res), std::move(rhs.res), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_binop_method(
    Comp&& comp, std::string fn_name
) {
    NodeRes lhs = interpret_value(std::move(comp.bin->lhs));
    if (lhs.is_err)
        return lhs;

    NodeRes rhs = interpret_value(std::move(comp.bin->rhs));
    if (rhs.is_err)
        return rhs;

    auto args = Nodes();
    args.push_back(std::move(rhs.res));

    return NodePtr(new CallAttr(
        std::move(lhs.res),
        NodePtr(new ID(std::move(fn_name), comp.src)),
        Args(std::move(args), {}, comp.src),
        comp.src
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

NodeRes InterpreterImpl::interpret_call(Comp&& comp) {
    NodeRes callee = interpret_value(std::move(comp.bin->lhs));
    if (callee.is_err)
        return callee;

    Res<Args> args = interpret_args(std::move(comp.bin->rhs));
    if (args.is_err)
        return std::move(args.err);

    if (comp.bin->rhs.op == OpID::ENCLOSURE) {
        return NodePtr(new Matcher(
            std::move(callee.res),
            std::move(args.res),
            comp.src
        ));
    }
    bool cached = comp.bin->rhs.op == OpID::LIST;

    if (callee.res->kind() == NodeKind::GET_ATTR) {
        // Need to transform to call attribute variant.
        auto& ga = dynamic_cast<GetAttr&>(*callee.res);
        if (cached)
            return NodePtr(new CachedCallAttr(
                std::move(ga.lhs),
                std::move(ga.rhs),
                std::move(args.res),
                comp.src
            ));
        return NodePtr(new CallAttr(
            std::move(ga.lhs),
            std::move(ga.rhs),
            std::move(args.res),
            comp.src
        ));
    }

    if (cached)
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
    ErrPtr err = require_pred_and_body(*comp.comp);
    if (err)
        return err;
    
    NodeRes pred = interpret_value(std::move(comp.comp->bin->lhs));
    if (pred.is_err)
        return std::move(pred.err);
    
    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    return Case(std::move(pred.res), std::move(body.res), comp.src);
}

Res<std::pair<std::vector<Case>, Nodes>> InterpreterImpl::interpret_cases(
    std::vector<Comp>&& comps,
    std::uint32_t start,
    OpID construct
) {
    // A somewhat awkward function existing to reduce code reuse between "if",
    // "match", and "try" constructs.
    OpID construct_end = construct == OpID::EXCEPT ? OpID::FINALLY: OpID::ELSE;
    std::vector<Case> cases;
    for (std::uint32_t i = start; i < comps.size(); i++) {
        if (comps[i].op == construct_end) {
            Res<Nodes> orelse = expect_body_only(std::move(*comps[i].comp));
            if (orelse.is_err)
                return std::move(orelse.err);
            return std::pair(std::move(cases), std::move(orelse.res));
        }
        if (comps[i].op != construct && i != 0) {
            // The above condition is checking to make sure we are getting the
            // keyword we expect (i.e., "elif" for an "if" construct).
            // Note that when i == 0, the keyword may be different, like "if"
            // rather than "elif". The i == 0 keyword is already validated as
            // that determined what construct we are handling in the first
            // place (we wouldn't call this function were it not appropriate to
            // handle that keyword).
            using enum OpID;
            switch(construct) {
            case CASE:
                return ErrPtr(new ExpectedCaseOrElseErr(comps[i].src));
            case ELIF:
                return ErrPtr(new ExpectedElifOrElseErr(comps[i].src));
            case EXCEPT:
                return ErrPtr(new ExpectedExceptOrFinallyErr(comps[i].src));
            default:
                return ErrPtr(new AssertionFailedErr("Unexpected construct"));
            }
        }
        Res<Case> c = interpret_case(std::move(comps[i]));
        if (c.is_err)
            return std::move(c.err);
        cases.push_back(std::move(c.res));
    }
    return std::pair(std::move(cases), Nodes());
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
    case TYPE:
        if (comps.size() > 1)
            return ErrPtr(new UnexpectedContinuationErr(comps[1].src));
        return interpret_type(std::move(comps[0]));
    case WHILE:
        return interpret_while(std::move(comps));
    default:
        return ErrPtr(new UnexpectedCompErr(comps[0]));
    }
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
        NodeRes value = interpret_value(std::move(*next));
        if (value.is_err)
            return std::move(value.err);
        nodes.push_back(std::move(value.res));
    }
    return nodes;
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_data(Comp&& comp) {
    return NodePtr(new ToType(
        std::get<typename ToType::Type>(std::move(comp.data)), comp.src
    ));
}

NodeRes InterpreterImpl::interpret_declare(Comp&& comp) {
    NodeRes id = interpret_definable(std::move(comp.bin->lhs));
    if (id.is_err)
        return std::move(id.err);
    NodeRes type = interpret_value(std::move(comp.bin->rhs));
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
    ErrPtr err = require_pred_and_body(*comp.comp);
    if (err)
        return err;
    NodePtr returns = nullptr;
    Comp* pred_comp = &comp.comp->bin->lhs;
    if (pred_comp->op == OpID::RETURNS) {
        NodeRes returns_res = interpret_value(std::move(pred_comp->bin->rhs));
        if (returns_res.is_err)
            return std::move(returns_res.err);
        returns = std::move(returns_res.res);
        pred_comp = &pred_comp->bin->lhs;
    }

    Res<ArgSpec> spec = interpret_arg_spec(std::move(*pred_comp));
    if (spec.is_err)
        return std::move(spec.err);

    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    return DefCase{
        std::move(spec.res), std::move(returns), std::move(body.res), comp.src
    };
}

Res<ArgDef> InterpreterImpl::interpret_defaulted_arg_def(Comp&& comp) {
    NodeRes dflt = interpret_value(std::move(comp.bin->rhs));
    if (dflt.is_err)
        return std::move(dflt.err);

    Res<ArgDefWithKind> def = interpret_arg_def(std::move(comp.bin->lhs));
    if (def.is_err)
        return std::move(def.err);
    if (def.res.def.dflt != nullptr)
        return ErrPtr(new TwiceDefaultedErr(comp.src));
    def.res.def.dflt = std::move(dflt.res);
    def.res.def.src = comp.src;

    using enum ArgDefKind;
    switch(def.res.kind) {
    case ARG:
        return std::move(def.res.def);
    case VAR_ARGS:
        return ErrPtr(new DefaultedVarArgsErr(comp.src));
    case VAR_KWARGS:
        return ErrPtr(new DefaultedVarKeywordArgsErr(comp.src));
    }
}

NodeRes InterpreterImpl::interpret_definable(Comp&& comp) {
    if (comp.op == OpID::GET) {
        NodeRes lhs = comp.bin->lhs.op == OpID::PLAIN_INT ?
            interpret_num_id(std::move(comp)):
            interpret_definable(std::move(comp));
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

NodeRes InterpreterImpl::interpret_enclosure(Comp&& comp) {
    Res<Nodes> elements = interpret_enclosure_recurse(std::move(*comp.comp));
    if (elements.is_err)
        return std::move(elements.err);
    if (is_map(elements.res))
        return NodePtr(new Map(std::move(elements.res), comp.src) );
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
            NodeRes element = interpret_map_element(std::move(comp.bin->rhs));
            if (element.is_err)
                return std::move(element.err);
            elements.res.push_back(std::move(element.res));
            return elements;
        }
        NodeRes element =
            interpret_expansion_or_value(std::move(comp.bin->rhs));
        if (element.is_err)
            return std::move(element.err);
        elements.res.push_back(std::move(element.res));
        return elements;
    }
    case TYPE_LABEL:
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
        NodeRes element = interpret_expansion_or_value(std::move(comp));
        if (element.is_err)
            return std::move(element.err);
        elements.push_back(std::move(element.res));
        return elements;
    }}
}

NodeRes InterpreterImpl::interpret_expansion_or_value(Comp&& comp) {
    using enum OpID;

    switch(comp.op) {
    case UNPACK_ARGS: {
        NodeRes unpacked = interpret_value(std::move(*comp.comp));
        if (unpacked.is_err)
            return std::move(unpacked.err);
        return NodePtr(new Expansion(std::move(unpacked.res), comp.src));
    }
    default: {
        NodeRes value = interpret_value(std::move(comp));
        if (value.is_err)
            return std::move(value.err);
        return value;
    }}
}

Res<std::pair<NodePtr, DefCase>> InterpreterImpl::interpret_first_def_case(
    Comp&& comp
) {
    ErrPtr err = require_pred_and_body(*comp.comp);
    if (err)
        return err;
    NodePtr returns = nullptr;
    Comp* pred_comp = &comp.comp->bin->lhs;
    if (pred_comp->op == OpID::RETURNS) {
        NodeRes returns_res = interpret_value(std::move(pred_comp->bin->rhs));
        if (returns_res.is_err)
            return std::move(returns_res.err);
        returns = std::move(returns_res.res);
        pred_comp = &pred_comp->bin->lhs;
    }

    if (pred_comp->op != OpID::CALL)
        return ErrPtr(new ExpectedCallErr(pred_comp->src));
    NodeRes id = interpret_definable(std::move(pred_comp->bin->lhs));
    if (id.is_err)
        return std::move(id.err);

    Res<ArgSpec> spec = interpret_arg_spec(std::move(pred_comp->bin->rhs));
    if (spec.is_err)
        return std::move(spec.err);

    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    return std::pair(
        std::move(id.res),
        DefCase(
            std::move(spec.res),
            std::move(returns),
            std::move(body.res),
            comp.src
        )
    );
}

NodeRes InterpreterImpl::interpret_float_tail(Comp&& comp) {
    return parse_float(std::get<std::string>(comp.data), comp.src);
}

NodeRes InterpreterImpl::interpret_for(std::vector<Comp>&& comps) {
    ErrPtr err = require_pred_and_body(*comps[0].comp);
    if (err)
        return err;

    if (comps[0].comp->bin->lhs.op != OpID::IN)
        return ErrPtr(new ExpectedInErr(comps[0].comp->bin->lhs.src));

    Res<std::vector<ID>> vars =
        interpret_loop_vars(std::move(comps[0].comp->bin->lhs.bin->lhs));
    if (vars.is_err)
        return std::move(vars.err);

    NodeRes iterable = interpret_value(std::move(
        comps[0].comp->bin->lhs.bin->rhs
    ));
    if (iterable.is_err)
        return std::move(iterable.err);

    Res<Nodes> body = interpret_body(std::move(comps[0].comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    Res<Nodes> orelse = interpret_optional_else(std::move(comps));
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
        return parse_float(s, comp.bin->lhs.src);
    }
    NodeRes obj = interpret_value(std::move(comp.bin->lhs));
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
    if (comp.comp->op != OpID::SEP && comp.comp->op != OpID::NOTHING)
        // Parenthesized value, not a tuple.
        return interpret_value(std::move(*comp.comp));
    return interpret_seq<Tuple>(std::move(comp));
}

NodeRes InterpreterImpl::interpret_id(Comp&& comp) {
    return interpret_data<ID>(std::move(comp));
}

NodeRes InterpreterImpl::interpret_if(std::vector<Comp>&& comps) {
    Res<std::pair<std::vector<Case>, Nodes>> cases = interpret_cases(
        std::move(comps), 0, OpID::ELIF 
    );

    if (cases.is_err)
        return std::move(cases.err);

    return NodePtr(new If(
        std::move(cases.res.first),
        std::move(cases.res.second),
        comps[0].src
    ));
}

NodeRes InterpreterImpl::interpret_kwarg(
    Comp&& comp, std::unordered_set<std::string>& kw_set
) {
    using enum OpID;
    switch(comp.op) {
    case BIND: {
        Res<dl::ID> bind_id = expect_id(std::move(comp.bin->lhs));
        if (bind_id.is_err)
            return NodePtr(new dl::ID(std::move(bind_id.res)));
        if (kw_set.count(bind_id.res.data) != 0)
            return ErrPtr(new DuplicateKeywordErr(
                std::move(bind_id.res.data), bind_id.res.src
            ));
        kw_set.insert(bind_id.res.data);
        NodeRes value = interpret_value(std::move(comp.bin->rhs));
        if (value.is_err)
            return std::move(value.err);
        return NodePtr(new KeywordArg(
            std::move(bind_id.res), std::move(value.res), comp.src
        ));
    }
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return ErrPtr(new PosAfterKeywordErr(comp.src));
    }
}

Res<ArgDefWithKind> InterpreterImpl::interpret_labeled_arg_def(
    Comp&& comp, bool match
) {
    NodeRes label = interpret_value(std::move(comp.bin->rhs));
    if (label.is_err)
        return std::move(label.err);
    Res<ArgDefWithKind> def = interpret_basic_arg_def(std::move(comp.bin->lhs));
    if (def.is_err)
        return def;
    if (def.res.def.id.data == "")
        return ErrPtr(new ExpectedIDOrVarArgsErr(comp.bin->lhs.src));
    def.res.def.match = match;
    def.res.def.label = std::move(label.res);
    def.res.def.src = comp.src;
    return def;
}

NodeRes InterpreterImpl::interpret_lambda(Comp&& comp) {
    std::pair<std::uint32_t, Comp> counted =
        count_op(std::move(comp), OpID::LAMBDA);
    std::uint32_t count = counted.first;
    Comp arg = std::move(counted.second);

    if (arg.op == OpID::GROUP && count == 1) {
        NodeRes inner = interpret_value(std::move(arg));
        if (inner.is_err)
            return inner;
        return NodePtr(new Lambda(std::move(inner.res), comp.src));
    }

    using enum OpID;
    switch(arg.op) {
    case LAMBDA_ARGS:
        return NodePtr(new LambdaArgs(count + 1, comp.src));
    case LAMBDA_KWARGS:
        return NodePtr(new LambdaKeywordArgs(count + 1, comp.src));
    default: {
        NodeRes var = interpret_general_id(std::move(arg));
        if (var.is_err)
            return var;
        return NodePtr(new LambdaVar(count, std::move(var.res), comp.src));
    }}
}

Res<std::vector<ID>> InterpreterImpl::interpret_loop_vars(Comp&& comp) {
    if (comp.op == OpID::ID) {
        auto v = std::vector<ID>();
        v.push_back(as_id(std::move(comp)));
        return v;
    }
    if (comp.op != OpID::LOOP_VAR_SEP)
        return ErrPtr(new ExpectedCommaSeparatedLoopVarsErr(comp.src));
    Res<std::vector<ID>> head =
        interpret_loop_vars(std::move(comp.bin->lhs));
    if (head.is_err)
        return head;
    Res<ID> var = expect_id(std::move(comp.bin->rhs));
    if (var.is_err)
        return std::move(var.err);
    head.res.push_back(std::move(var.res));
    return head;
}

NodeRes InterpreterImpl::interpret_map_element(Comp&& comp) {
    using enum OpID;

    switch(comp.op) {
    case TYPE_LABEL:
        return interpret_binop<Entry>(std::move(comp));
    case UNPACK_KWARGS:
        return interpret_unop<Expansion>(std::move(comp));
    default:
        return ErrPtr(new ExpectedEntryOrVarKeywordArgsErr(comp.src));
    }
}

NodeRes InterpreterImpl::interpret_match(std::vector<Comp>&& comps) {
    NodeRes matchee = interpret_value(std::move(*comps[0].comp));
    if (matchee.is_err)
        return matchee;
    if (comps.size() == 1)
        return ErrPtr(new NoMatchCasesErr(comps[0].src));
    Res<std::pair<std::vector<Case>, Nodes>> cases =
        interpret_cases(std::move(comps), 1, OpID::CASE);
    if (cases.is_err)
        return std::move(cases.err);

    return NodePtr(new Match(
        std::move(matchee.res),
        std::move(cases.res.first),
        std::move(cases.res.second),
        comps[0].src
    ));
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
        read_number<std::int32_t>(comp.src, &s[0], &s[0] + s.size(), 10);
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
    NodeRes res = interpret_value(std::move(*comp.comp));
    if (res.is_err)
        return res;
    return NodePtr(new ToType(std::move(res.res), comp.src));
}

Res<Nodes> InterpreterImpl::interpret_optional_else(std::vector<Comp>&& comps) {
    if (comps.size() > 2 || (comps.size() == 2 && comps[1].op != OpID::ELSE))
        // Assuming no bug in the parser, the second part of the construct
        // cannot be else if there are at least three parts, because else may
        // not be followed by another part. All optional else's are currently
        // only found in the second of an at-most two-part construct.
        return ErrPtr(new ExpectedElseErr(comps[1].src));
    if (comps.size() == 2)
        return expect_body_only(std::move(*comps[1].comp));
    // Use an empty Nodes to indicate lack of else and no error.
    return Nodes();
}

NodeRes InterpreterImpl::interpret_plain_int(Comp&& comp) {
    std::string& s = std::get<std::string>(comp.data);
    Res<std::int32_t> x = read_number<std::int32_t>(
        comp.src,
        &s[0],
        &s[0] + s.size(),
        10
    );
    if (x.is_err)
        return std::move(x.err);
    return NodePtr(new Int32(std::move(x.res), comp.src));
}

NodeRes InterpreterImpl::interpret_sep(Comp&& comp) {
    Res<Nodes> elements = interpret_csv(std::move(comp));
    if (elements.is_err)
        return std::move(elements.err);
    return NodePtr(new Tuple(std::move(elements.res), elements.res[0]->src));
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
    return NodePtr(new Symbol(std::move(id.res), comp.src));
}

NodeRes InterpreterImpl::interpret_ternary(Comp&& comp) {
    if (comp.bin->lhs.op != OpID::TERNARY_IF)
        return ErrPtr(new ExpectedTernaryIfErr(comp.bin->lhs.src));
    
    NodeRes predicate = interpret_value(std::move(comp.bin->lhs.bin->rhs));
    if (predicate.is_err)
        return predicate;

    NodeRes if_case = interpret_value(std::move(comp.bin->lhs.bin->lhs));
    if (if_case.is_err)
        return if_case;

    NodeRes else_case = interpret_value(std::move(comp.bin->rhs));
    if (else_case.is_err)
        return std::move(else_case.err);

    return NodePtr(new Ternary(
        std::move(predicate.res),
        std::move(if_case.res),
        std::move(else_case.res),
        comp.src
    ));
}

NodeRes InterpreterImpl::interpret_try(std::vector<Comp>&& comps) {
    if (comps.size() == 1)
        return ErrPtr(new IsolatedTryErr(comps[0].src));

    Res<Nodes> body = expect_body_only(std::move(*comps[0].comp));
    if (body.is_err)
        return std::move(body.err);

    Res<std::pair<std::vector<Case>, Nodes>> cases = interpret_cases(
        std::move(comps), 1, OpID::EXCEPT
    );

    if (cases.is_err)
        return std::move(cases.err);

    return NodePtr(new Try(
        std::move(body.res),
        std::move(cases.res.first),
        std::move(cases.res.second),
        comps[0].src
    ));
}

NodeRes InterpreterImpl::interpret_type(Comp&& comp) {
    ErrPtr err = require_pred_and_body(*comp.comp);
    if (err)
        return err;

    Res<ID> id = ID("", Pos(0, 0));
    Res<Nodes> parents;

    switch(comp.comp->bin->lhs.op) {
    case OpID::ID: {
        id = expect_id(std::move(comp.comp->bin->lhs));
        break;
    }
    case OpID::CALL: {
        id = expect_id(std::move(comp.comp->bin->lhs.bin->lhs));
        parents = interpret_csv(
            std::move(comp.comp->bin->lhs.bin->rhs)
        );
        if (parents.is_err)
            return std::move(parents.err);
        break;
    }
    default:
        return ErrPtr(
            new ExpectedIDOrCallErr(comp.comp->bin->lhs.src)
        );
    }
    if (id.is_err)
        return std::move(id.err);

    Res<Nodes> body = interpret_body(std::move(comp.comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);
    return NodePtr(
        new Type(
            std::move(id.res),
            std::move(parents.res),
            std::move(body.res),
            comp.src
        )
    );
}

template<typename ToType>
NodeRes InterpreterImpl::interpret_unop(Comp&& comp) {
    NodeRes arg = interpret_value(std::move(*comp.comp));
    if (arg.is_err)
        return arg;
    return NodePtr(new ToType(std::move(arg.res), comp.src));
}

NodeRes InterpreterImpl::interpret_unop_method(
    Comp&& comp, std::string fn_name
) {
    NodeRes arg = interpret_value(std::move(*comp.comp));
    if (arg.is_err)
        return arg;

    return NodePtr(new CallAttr(
        std::move(arg.res),
        NodePtr(new ID(std::move(fn_name), comp.src)),
        Args({}, {}, comp.src),
        comp.src
    ));
}

NodeRes InterpreterImpl::interpret_value(Comp&& comp) {
    using enum OpID;
    switch(comp.op) {
    case ADD:
        return interpret_binop_method(std::move(comp), "__add__");
    case ADDR:
        return interpret_unop<Addr>(std::move(comp));
    case ADDR_TYPE:
        return interpret_unop<AddrType>(std::move(comp));
    case AND:
        return interpret_binop<And>(std::move(comp));
    case BAND:
        return interpret_binop_method(std::move(comp), "__band__");
    case BNOT:
        return interpret_unop_method(std::move(comp), "__bnot__");
    case BOR:
        return interpret_binop_method(std::move(comp), "__bor__");
    case BXOR:
        return interpret_binop_method(std::move(comp), "__bxor__");
    case CALL:
        return interpret_call(std::move(comp));
    case CHAR:
        return interpret_data<Char>(std::move(comp));
    case CONCAT:
        return interpret_concat(std::move(comp));
    case DIV:
        return interpret_binop_method(std::move(comp), "__div__");
    case ENCLOSURE:
        return interpret_enclosure(std::move(comp));
    case EQ:
        return interpret_binop_method(std::move(comp), "__eq__");
    case FALSE:
        return NodePtr(new Bool(false, comp.src));
    case FLOAT_TAIL:
        return interpret_float_tail(std::move(comp));
    case GET:
        return interpret_get(std::move(comp));
    case GROUP:
        return interpret_group(std::move(comp));
    case GT:
        return interpret_binop_method(std::move(comp), "__gt__");
    case GTE:
        return interpret_binop_method(std::move(comp), "__gte__");
    case ID:
        return NodePtr(new dl::ID(as_id(std::move(comp))));
    case IN:
        return interpret_binop_method(std::move(comp), "__in__");
    case LAMBDA:
        return interpret_lambda(std::move(comp));
    case LAMBDA_ARGS:
        return NodePtr(new LambdaArgs(1, comp.src));
    case LAMBDA_KWARGS:
        return NodePtr(new LambdaKeywordArgs(1, comp.src));
    case LIST:
        return interpret_seq<List>(std::move(comp)); 
    case LSH:
        return interpret_binop_method(std::move(comp), "__lsh__");
    case LT:
        return interpret_binop_method(std::move(comp), "__lt__");
    case LTE:
        return interpret_binop_method(std::move(comp), "__lte__");
    case MOD:
        return interpret_binop_method(std::move(comp), "__mod__");
    case MUL:
        return interpret_binop_method(std::move(comp), "__mul__");
    case NEG:
        return interpret_unop_method(std::move(comp), "__neg__");
    case NEQ:
        return interpret_binop_method(std::move(comp), "__neq__");
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
        return interpret_binop_method(std::move(comp), "__pow__");
    case RSH:
        return interpret_binop_method(std::move(comp), "__rsh__");
    case SEP:
        return interpret_sep(std::move(comp));
    case STRING:
        return NodePtr(new String(
            std::move(std::get<std::string>(comp.data)), comp.src
        ));
    case SUB:
        return interpret_binop_method(std::move(comp), "__sub__");
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
        return ErrPtr(new ExpectedExprErr(comp.src));
    }
}

NodeRes InterpreterImpl::interpret_up(Comp&& comp) {
    std::pair<std::uint32_t, Comp> counted =
        count_op(std::move(comp), OpID::UP);
    NodeRes id = interpret_general_id(std::move(counted.second));
    if (id.is_err)
        return id;
    return NodePtr(new Var(counted.first, std::move(id.res), comp.src));
}

NodeRes InterpreterImpl::interpret_while(std::vector<Comp>&& comps) {
    ErrPtr err = require_pred_and_body(*comps[0].comp);
    if (err)
        return err;

    NodeRes pred = interpret_value(std::move(comps[0].comp->bin->lhs));
    if (pred.is_err)
        return std::move(pred.err);

    Res<Nodes> body = interpret_body(std::move(comps[0].comp->bin->rhs));
    if (body.is_err)
        return std::move(body.err);

    Res<Nodes> orelse = interpret_optional_else(std::move(comps));
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

NodeRes InterpreterImpl::parse_float(const std::string& str, Pos src) {
    const char* begin = &str[0];
    LiteralSuffix ls = lit_suffix(str);
    const char* expected_end = begin + str.size() - lit_suffix_len(ls);
    if (ls == LiteralSuffix::F32) {
        Res<float> res = read_number<float>(src, begin, expected_end, 10);
        if (res.is_err)
            return std::move(res.err);
        return NodePtr(new Float32(std::move(res.res), src));
    }
    Res<double> res = read_number<double>(src, begin, expected_end, 10);
    if (res.is_err)
        return std::move(res.err);
    return NodePtr(new Float64(std::move(res.res), src));
}

ErrPtr InterpreterImpl::require_pred_and_body(const Comp& comp) {
    if (comp.op != OpID::LABEL) {
        if (comp.op == OpID::BODY)
            return ErrPtr(new MissingPredicateErr(comp.src));
        return ErrPtr(new MissingBodyErr(comp.src));
    }
    return nullptr;
}

}
