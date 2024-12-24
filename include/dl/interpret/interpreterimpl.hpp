#pragma once

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>

#include <ostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "dl/err.hpp"
#include "dl/compose/comp.hpp"
#include "dl/convert.hpp"
#include "dl/interpret/addr.hpp"
#include "dl/interpret/addrtype.hpp"
#include "dl/interpret/and.hpp"
#include "dl/interpret/argconst.hpp"
#include "dl/interpret/argdef.hpp"
#include "dl/interpret/args.hpp"
#include "dl/interpret/argspec.hpp"
#include "dl/interpret/block.hpp"
#include "dl/interpret/break.hpp"
#include "dl/interpret/cachedcall.hpp"
#include "dl/interpret/cachedcallattr.hpp"
#include "dl/interpret/call.hpp"
#include "dl/interpret/callattr.hpp"
#include "dl/interpret/continue.hpp"
#include "dl/interpret/def.hpp"
#include "dl/interpret/defcase.hpp"
#include "dl/interpret/except.hpp"
#include "dl/interpret/expansion.hpp"
#include "dl/interpret/for.hpp"
#include "dl/interpret/foreach.hpp"
#include "dl/interpret/getattr.hpp"
#include "dl/interpret/id.hpp"
#include "dl/interpret/if.hpp"
#include "dl/interpret/interpreter.hpp"
#include "dl/interpret/literal.hpp"
#include "dl/interpret/keywordarg.hpp"
#include "dl/interpret/list.hpp"
#include "dl/interpret/literal.hpp"
#include "dl/interpret/match.hpp"
#include "dl/interpret/matchcase.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/not.hpp"
#include "dl/interpret/objvar.hpp"
#include "dl/interpret/or.hpp"
#include "dl/interpret/raise.hpp"
#include "dl/interpret/return.hpp"
#include "dl/interpret/set.hpp"
#include "dl/interpret/setattr.hpp"
#include "dl/interpret/symbol.hpp"
#include "dl/interpret/ternary.hpp"
#include "dl/interpret/this.hpp"
#include "dl/interpret/try.hpp"
#include "dl/interpret/tuple.hpp"
#include "dl/interpret/type.hpp"
#include "dl/interpret/typecheck.hpp"
#include "dl/interpret/update.hpp"
#include "dl/interpret/updateattr.hpp"
#include "dl/interpret/var.hpp"
#include "dl/interpret/varargdef.hpp"
#include "dl/interpret/varkwargdef.hpp"
#include "dl/interpret/while.hpp"

namespace dl {

struct ArgsAfterKwargsErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "ArgsAfterVarArgErr";
	}
};

struct ByWithoutToErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "ByWithoutToErr";
	}
};

struct DuplicateArgErr final: SourcedErr {
	std::string arg;

	DuplicateArgErr(std::string&& arg, std::uint64_t src_id) noexcept:
	SourcedErr(src_id), arg(std::move(arg)) {}

	std::ostream& out_data(std::ostream& os) const override {
		return SourcedErr::out_data(os) << ", " << arg;
	}

	std::ostream& out_name(std::ostream& os) const override {
		return os << "DuplicateArgErr";
	}
}

struct DuplicateKeywordErr final: SourcedErr {
	std::string keyword;

	DuplicateKeywordErr(std::string&& keyword, std::uint64_t src_id) noexcept:
	SourcedErr(src_id), keyword(std::move(keyword)) {}

	std::ostream& out_data(std::ostream& os) const override {
		return SourcedErr::out_data(os) << ", " << keyword;
	}

	std::ostream& out_name(std::ostream& os) const override {
		return os << "DuplicateKeywordErr";
	}
};

struct ExpectedIDErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "ExpectedIDErr";
	}
};

struct FromWithoutToErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "FromWithoutToErr";
	}
};

struct InconsistentConstructErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "InconsistentConstructErr";
	}
};

struct MissingBodyErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "MissingBodyErr";
	}
};

struct MissingPredicateErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "MissingPredicateErr";
	}
};

struct NoMatchCasesErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "NoMatchCasesErr";
	}
}

struct NonDefaultFollowsDefaultErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "NonDefaultFollowsDefaultErr";
	}
};

struct PosAfterKeywordErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "PosAfterKeywordErr";
	}
};

struct UnexpectedCompErr final: SourcedErr {
	OpID op;

	UnexpectedCompErr(std::uint64_t src_id, OpID op) noexcept:
	SourcedErr(src_id), op(op) {}

	UnexpectedCompErr(const Comp& comp) noexcept:
	UnexpectedCompErr(comp.src_id, comp.op) {}

	bool equals(const Err& err) const noexcept override {
		return
			SourcedErr::equals(err) &&
			id == dynamic_cast<const UnexpectedCompErr&>(err).op;
	}

	std::ostream& out_name(std::ostream& os) const override {
		return os << "UnexpectedCompErr";
	}
};

struct UnexpectedPredicateErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "UnexpectedPredicateErr";
	}
};

struct VarArgsFollowsDefaultErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "VarArgsFollowsDefaultErr";
	}
}

struct VarArgsWhereKeywordArgExpectedErr final: SourcedErr {
	std::ostream& out_name(std::ostream& os) const override {
		return os << "VarPosWhereKeywordArgExpectedErr";
	}
};

struct InterpreterImpl final: Interpreter {
	static inline ID as_id(Comp&& comp);
	static inline NodeRes count_up(Comp&& comp);
	static inline ErrPtr csv_to_vec(Comp&& comp, Nodes& nodes);
	static inline Res<Block> expect_body_only(Comp&& comp);
	static inline Res<ID> expect_id(Comp&& comp);
	NodeRes interpret(Comp&& comp) override;
	static inline NodeRes interpret_arg(Comp&& comp);
	static inline NodeRes interpret_arg_const(Comp&& comp);
	static inline NodeRes interpret_arg_def(Comp&& comp);
	static inline Res<ArgSpec> interpret_arg_spec(Comp&& comp);
	static inline ErrPtr interpret_arg_spec_recurse(
		Comp&& comp, ArgSpec& spec, bool& in_kw_part, bool& found_pos_dflt
	);
	static inline Res<Args> interpret_args(Comp&& comp);
	static inline ErrPtr interpret_args_recurse(
		Comp& comp, Nodes& args, Nodes& kwargs, bool& in_kw_part
	);
	static inline NodeRes interpret_basic_arg_def(Comp&& comp, bool in_kw_part);

	template<typename NodeT>
	static inline NodeRes interpret_binop(Comp&& comp);

	static inline NodeRes interpret_binop_method(
		Comp&& comp, std::string fn_name
	);
	static inline Res<Block> interpret_body(Comp&& comp);
	static inline Res<Block> interpret_block(Comp&& comp);
	static inline NodeRes interpret_call(Comp&& comp);
	static inline NodeRes interpret_construct(std::vector<Comp>&& comps);
	static inline NodeRes interpret_def(std::vector<Comp>&& comps);
	static inline Res<DefCase> interpret_def_case(Comp&& comp);
	static inline NodeRes interpret_defaulted_arg_def(Comp&& comp);
	static inline Res<Except> interpret_except(Comp&& comp);
	static inline Res<std::pair<ID, DefCase>> interpret_first_def_case(
		Comp&& comp
	);
	static inline NodeRes interpret_for(std::vector<Comp>& comps);
	static inline NodeRes interpret_for_each(std::vector<Comp>&& comps);
	static inline NodeRes interpret_for_range(std::vector<Comp>&& comps);
	static inline NodeRes interpret_get(Comp&& comp);
	static inline NodeRes interpret_group(Comp&& comp);
	static inline NodeRes interpret_if(
		std::vector<Comp>&& comps, std::uint32_t i=0
	);
	static inline NodeRes interpret_kwarg(
		Comp&& comp, std::unordered_set<std::string>& kw_set
	);
	static inline NodeRes interpret_kwarg_def(Comp&& comp);
	static inline NodeRes interpret_list(Comp&& comp);
	static inline NodeRes interpret_match(std::vector<Comp>&& comps);
	static inline Res<MatchCase> interpret_match_case(Comp&& comp);
	static inline NodeRes interpret_not_in(Comp&& comp);
	static inline NodeRes interpret_number(const Comp& comp);
	static inline Res<Block> interpret_optional_else(std::vector<Comp>&& comps);
	static inline NodeRes interpret_set(Comp&& comp);
	static inline NodeRes interpret_ternary(Comp&& comp);
	static inline NodeRes interpret_try(std::vector<Comp>&& comps);
	static inline NodeRes interpret_type(Comp&& comp);
	static inline NodeRes interpret_typed_arg_def(Comp&& comp);

	template<typename NodeT>
	static inline NodeRes interpret_unop(Comp&& comp);

	static inline NodeRes interpret_unop_method(
		Comp&& comp, std::string fn_name
	);
	static inline NodeRes interpret_value(Comp&& comp);
	static inline NodeRes interpret_while(std::vector<Comp>&& comps);
	static inline ErrPtr require_pred_and_body(const Comp& comp);
};

ID InterpreterImpl::as_id(Comp&& comp) {
	return ID{comp.src_id, std::move(std::get<std::string>(comp.data))};
}

template<typename NodeT>
NodeRes InterpreterImpl::count_up(Comp&& comp) {
	Comp* comp_ptr = &comp;
	std::uint32_t count = 1;
	while(comp_ptr->op == OpID::UP) {
		count++;
		comp_ptr = comp_ptr->comp;
	}
	Res<ID> id = expect_id(std::move(*comp_ptr));
	if (id.is_err)
		return id.err;
	return NodePtr(new NodeT{comp.src_id, std::move(id.res), count});
}

ErrPtr InterpreterImpl::csv_to_vec(Comp&& comp, Nodes& nodes) {
	using enum OpID;
	switch(comp.op) {
	case SEP:
		ErrPtr err = csv_to_vec(std::move(comp.bin->lhs), nodes);
		if (err)
			return err;

		// Redundantly checks for OpID::SEP again but that's OK.
		return csv_to_vec(std::move(comp.bin->rhs), nodes);
	case UNPACK_ARGS:
		NodeRes expansion = interpret_unop<Expansion>(std::move(comp));
		if (expansion.is_err)
			return expansion.err;
		nodes.push_back(std::move(expansion.res));
		return nullptr;
	default:
		NodeRes value = interpret_value(std::move(comp));
		if (value.is_err)
			return value.err;
		nodes.push_back(std::move(value.res));
	}
}

Res<Block> InterpreterImpl::expect_body_only(Comp&& comp) {
	if (comp.op != OpID::EXPR)
		if (comp.op == OpID::LABELED)
			return ErrPtr(new UnexpectedPredicateErr(comp.src_id));
		return ErrPtr(new UnexpectedCompErr(comp));
	return interpret_body(std::move(*comp.comp));
}

Res<ID> InterpreterImpl::expect_id(Comp&& comp) {
	if (comp.op != OpID::ID)
		return ErrPtr(new ExpectedIDErr(comp.src_id));
	return ID{comp.src_id, std::move(std::get<std::string>(comp.data))};
}

NodeRes InterpreterImpl::interpret(Comp&& comp) {
	using enum OpID;
	switch(comp.op) {
	case BREAK:
		return NodePtr(new Break{comp.src_id});
	case CONTINUE:
		return NodePtr(new Continue{comp.src_id});
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
	case IEXP:
		return interpret_binop_method(std::move(comp), "__iexp__");
	case ILSH:
		return interpret_binop_method(std::move(comp), "__ilsh__");
	case IMOD:
		return interpret_binop_method(std::move(comp), "__imod__");
	case IMUL:
		return interpret_binop_method(std::move(comp), "__imul__");
	case IRSH:
		return interpret_binop_method(std::move(comp), "__irsh__");
	case ISUB:
		return interpret_binop_method(std::move(comp), "__isub__");
	case RAISE:
		return interpret_unop<Raise>(std::move(comp));
	case RETURN:
		return interpret_unop<Return>(std::move(comp));
	case SET:
		return interpret_set(std::move(comp));
	case TYPE:
		return interpret_type(std::move(comp));
	default:
		return interpret_value(std::move(comp));
	}
}

NodeRes InterpreterImpl::interpret_arg(Comp&& comp) {
	if (comp.op == OpID::UNPACK_ARGS)
		return interpret_unop<Expansion>(std::move(comp));
	return interpret_value(std::move(comp));
}

NodeRes InterpreterImpl::interpret_arg_const(Comp&& comp) {
	Res<ID> id = expect_id(std::move(comp.bin->lhs));
	if (id.is_err)
		return id.err;
	NodeRes val = interpret_value(std::move(comp.bin->rhs));
	if (val.is_err)
		return val.err;
	return NodePtr(
		new ArgConst{comp.src_id, std::move(id.res), std::move(val.res)}
	);
}

NodeRes InterpreterImpl::interpret_arg_def(Comp&& comp) {
	using enum OpID;
	switch(comp.op) {
	case BIND:
		return interpret_defaulted_arg_def(std::move(comp));
	case LABELED:
		return interpret_typed_arg_def(std::move(comp));
	default:
		return interpret_basic_arg_def(comp);
	}
}

Res<ArgSpec> InterpreterImpl::interpret_arg_spec(Comp&& comp) {
	if (comp.op != OpID::GROUP)
		return ErrPtr(new UnexpectedCompErr(comp));

	ArgSpec spec;
	spec.var_args.src_id = 0;
	spec.var_args.id.src_id = 0;
	spec.var_kwargs.src_id = 0;
	spec.var_kwargs.id.src_id = 0;
	spec.src_id = comp.src_id;
	bool in_kw_part = false;
	bool found_pos_dflt = false;
	std:unordered_set<std::string> id_set;
	
	ErrPtr err = interpret_arg_spec_recurse(
		std::move(*comp.comp), spec, in_kw_part, found_pos_dflt, id_set
	);
	if (err)
		return err;

	return spec;
}

ErrPtr InterpreterImpl::interpret_arg_spec_recurse(
	Comp&& comp,
	ArgSpec& spec,
	bool& in_kw_part,
	bool& found_pos_dflt,
	std::unordered_set<std::string>& id_set
) {
	if (spec.var_kwargs.src_id != 0)
		return ErrPtr(new ArgsAfterKwargsErr(comp.src_id));
	if (comp.op == OpID::SEP) {
		ErrPtr err = interpret_arg_spec_recurse(
			std::move(comp.bin->lhs), spec, in_kw_part, found_pos_dflt, id_set
		);
		if (err)
			return err;
		return interpret_arg_spec_recurse(
			std::move(comp.bin->rhs), spec, in_kw_part, found_pos_dflt, id_set
		);
	}

	if (!in_kw_part && comp.op == OpID::POS_KW_SEP) {
		in_kw_part = true;
		return nullptr;
	}

	NodeRes def = interpret_arg_def(std::move(comp));
	if (def.is_err)
		return std::move(def.err);

	using enum NodeID;
	switch(def.res.kind()) {
	case ARG_DEF: {
		auto casted = dynamic_cast<ArgDef&>(*def.res);
		id_set.insert(casted.id.id);
		if (in_kw_part)
			spec.kwargs.push_back(std::move(casted));
		else {
			if (found_pos_dflt && casted.dflt == nullptr)
				return ErrPtr(new NonDefaultFollowsDefaultErr(comp.src_id));
			else if (casted.dflt != nullptr)
				found_pos_dflt = true;
			spec.args.push_back(std::move(casted));
		}
		return nullptr;
	}
	case VAR_ARG_DEF: {
		if (in_kw_part)
			return ErrPtr(new VarArgsWhereKeywordArgExpectedErr(comp.src_id));
		if (found_pos_dflt)
			return ErrPtr(new VarArgsFollowsDefaultErr(comp.src_id));
		auto casted = dynamic_cast<VarArgsDef&>(*def.res);
		id_set.insert(casted.id.id);
		spec.var_args = std::move(casted);
		in_kw_part = true;
		return nullptr;
	}
	case VAR_KWARG_DEF: {
		// No need to update id_set or in_kw_part, variable keyword argument
		// must always be the last argument, which is checked above.
		auto casted = dynamic_cast<VarKwargsDef&>(*def.res);
		spec.var_kwargs = std::move(casted);
		return nullptr;
	}
	default:
		return ErrPtr(new AssertionFailedErr("Impossible node kind"));
	}
}

Res<Args> InterpreterImpl::interpret_args(Comp&& comp) {
	if (comp.op == OpID::NOTHING)
		return Args{comp.src_id, Nodes(), Nodes()};

	Nodes args;
	Nodes kwargs;

	bool in_kw_part = false;
	std::unordered_set<std::string> kw_set;
	ErrPtr err = interpret_args_recurse(
		std::move(comp), args, kwargs, in_kw_part, kw_set
	);
	if (err)
		return err;
	return Args{comp.src_id, std::move(args), std::move(kwargs)};
}

ErrPtr InterpreterImpl::interpret_args_recurse(
	Comp&& comp,
	Nodes& args,
	Nodes& kwargs,
	bool& in_kw_part,
	std::unordered_set<std::string>& kw_set
) {
	if (comp.op == NodeID::SEP) {
		ErrPtr err = interpret_args_recurse(
			std::move(comp.bin->lhs), args, kwargs, in_kw_part
		);
		if (err)
			return err;
		return interpret_args_recurse(
			std::move(comp.bin->rhs), args, kwargs, in_kw_part
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

NodeRes InterpreterImpl::interpret_basic_arg_def(Comp&& comp) {
	using enum OpID;
	switch(comp.op) {
	case ID:
		return NodePtr(
			new ArgDef{comp.src_id, as_id(std::move(comp)), nullptr, nullptr}
		);
	case UNPACK_ARGS:
		Res<ID> id = expect_id(std::move(*comp.comp));
		if (id.is_err)
			return std::move(id.err);
		return NodePtr(new VarArgsDef{comp.src_id, std::move(id.res), nullptr});
	case UNPACK_KWARGS:
		Res<ID> id = expect_id(std::move(*comp.comp));
		if (id.is_err)
			return std::move(id.err);
		return NodePtr(
			new VarKwargsDef{comp.src_id, std::move(id.res), nullptr}
		);
	default:
		return ErrPtr(new UnexpectedCompErr(comp));
	}
}

template<typename NodeT>
NodeRes InterpreterImpl::interpret_binop(Comp&& comp) {
	NodeRes lhs = interpret_value(std::move(comp.bin->lhs));
	if (lhs.is_err)
		return lhs;

	NodeRes rhs = interpret_value(std::move(comp.bin->rhs));
	if (rhs.is_err)
		return rhs;

	return NodePtr(new NodeT{
		comp.src_id,
		std::move(lhs.res),
		std::move(rhs.res)
	});
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

	return NodePtr(new CallAttr{
		comp.src_id,
		std::move(lhs.res),
		ID{comp.src_id, std::move(fn_name)},
		Args{rhs.src_id, {{std::move(rhs.res)}}, {}}
	});
}

Res<Block> InterpreterImpl::interpret_body(Comp&& comp) {
	if (comp.op == OpID::BLOCK)
		return interpret_block(std::move(comp));

	NodeRes res = interpret(std::move(comp));
	if (res.is_err)
		return res.err;
	return Block{comp.src_id, {std::move(res.res)}};
}

Res<Block> InterpreterImpl::interpret_block(Comp&& comp) {
	Nodes code;
	for(Comp& comp: comp.comps) {
		NodeRes res = interpret(std::move(comp));
		if (res.is_err)
			return res.err;
		code.push_back(std::move(res.res));
	}
	return Block{comp.src_id, std::move(code)};
}

NodeRes InterpreterImpl::interpret_call(Comp&& comp) {
	NodeRes callee = interpret_value(std::move(comp.bin->lhs));
	if (callee.is_err)
		return callee.err;

	Res<Args> args = interpret_args(std::move(*comp.bin->rhs.comp));
	if (args.is_err)
		return args.err;

	using enum OpID;
	if (callee.res->kind() == NodeID::GET_ATTR) {
		GetAttr& node = dynamic_cast<GetAttr&>(callee.res);
		// Need to transform to call attribute variants.
		switch(comp.bin->rhs.op) {
		case GROUP:
			return NodePtr(new CallAttr{
				comp.src_id,
				std::move(node.object),
				std::move(node.attr),
				std::move(node.args)
			});
		case LIST:
			return NodePtr(new CachedCallAttr{
				comp.src_id,
				std::move(node.object),
				std::move(node.attr),
				std::move(node.args)
			});
		default:
			return ErrPtr(new UnexpectedCompErr(comp));
		}
	}

	switch(comp.bin->rhs.op) {
	case GROUP:
		return NodePtr(new Call{
			comp.src_id, std::move(callee.res), std::move(args.res)
		});
	case LIST:
		return NodePtr(new CachedCall{
			comp.src_id, std::move(callee.res), std::move(args.res)
		});
	default:
		return ErrPtr(new UnexpectedCompErr(comp));
	}
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
		return ErrPtr(new UnexpectedCompErr(comp.comps[0]));
	}
}

NodeRes InterpreterImpl::interpret_def(std::vector<Comp>&& comps) {
	Res<std::pair<ID, DefCase>> first = interpret_first_def_case(
		std::move(*comps[0].comp), true
	);
	if (first.is_err)
		return first.err;

	std::vector<DefCase> cases;
	cases.push_back(std::move(first.res.second));
	for (std::uint32_t i = 1; i < comps.size(); i++) {
		if (comps[i].op != OpID::CASE)
			return InconsistentConstructErr(comps[i].src_id);
		Res<std::pair<ID, DefCase>> def_case =
			interpret_def_case(std::move(*comps[i].comp), false);
		if (def_case.is_err)
			return def_case.err;
		cases.push_back(std::move(def_case.res.second));
	}
	return NodePtr(
		new Def{comps[0].src_id, std::move(cases), std::move(first.res.first)}
	);
}

Res<DefCase> InterpreterImpl::interpret_def_case(Comp&& comp) {
	ErrPtr err = require_pred_and_body(comp);
	if (err)
		return err;
	NodePtr returns = nullptr;
	Comp* pred_comp = &comp.bin->lhs;
	if (pred_comp->op == OpID::ARROW) {
		NodeRes returns_res = interpret_value(std::move(pred_comp->bin->rhs));
		if (returns_res.is_err)
			return returns_res.err;
		returns = std::move(returns_res.res);
		pred_comp = &pred_comp.bin->lhs;
	}

	ArgSpec spec;
	if (pred_comp->op != OpID::GROUP)
		return ErrPtr(new UnexpectedCompErr(*pred_comp));
	Res<ArgSpec> spec_res = interpret_arg_spec(std::move(*pred_comp->comp));
	if (spec_res.is_err)
		return spec_res.err;
	spec = std::move(spec_res.res);

	Res<Block> body = interpret_body(std::move(comp));
	if (body.is_err)
		return body.err;

	return DefCase{
		comp.src_id, std::move(spec), std::move(returns), std::move(body)
	};
}

NodeRes InterpreterImpl::interpret_defaulted_arg_def(Comp&& comp) {
	NodeRes dflt = interpret_value(std::move(comp.bin->rhs));
	if (dflt.is_err)
		return dflt;
	switch(comp.bin->lhs.op) {
	case ID: {
		Res<ID> id = as_id(std::move(comp.bin->lhs));
		return NodePtr(new ArgDef{
			comp.src_id, std::move(id.res), nullptr, std::move(dflt.res)
		});
	}
	case LABEL: {
		NodeRes typed_arg_def = interpret_typed_arg_def(
			std::move(comp.bin->lhs)
		);
		if (typed_arg_def.is_err)
			return typed_arg_def.err;
		if (typed_arg_def.res.kind() != NodeID::ARG_DEF)
			return ErrPtr(new UnexpectedCompErr(comp.bin->lhs));
		auto casted = dynamic_cast<ArgDef&>(*typed_arg_def.res);
		return NodePtr(new ArgDef{
			comp.src_id,
			std::move(casted.id),
			std::move(casted.type),
			std::move(dflt.res)
		});
	}
	default:
		return ErrPtr(new UnexpectedCompErr(comp.bin->lhs));
	}
}

Res<Except> InterpreterImpl::interpret_except(Comp&& comp) {
	if (comp.op != OpID::EXCEPT)
		return ErrPtr(new InconsistentConstructErr(comp.src_id));

	ErrPtr err = require_pred_and_body(*comp.comp);
	if (err)
		return err;

	NodeRes pred = interpret_value(std::move(comp.comp->bin->lhs));
	if (pred.is_err)
		return pred.err;

	Res<Block> body = interpret_body(std::move(comp.comp->bin->rhs));
	if (body.is_err)
		return body.err;

	return Except{comp.src_id, std::move(pred.res), std::move(body.res)};
}

Res<std::pair<ID, DefCase>> InterpreterImpl::interpret_first_def_case(
	Comp&& comp
) {
	ErrPtr err = require_pred_and_body(comp);
	if (err)
		return err;
	NodePtr returns = nullptr;
	Comp* pred_comp = &comp.bin->lhs;
	if (pred_comp->op == OpID::ARROW) {
		NodeRes returns_res = interpret_value(std::move(pred_comp->bin->rhs));
		if (returns_res.is_err)
			return returns_res.err;
		returns = std::move(returns_res.res);
		pred_comp = &pred_comp.bin->lhs;
	}
	ID id{0, ""};
	ArgSpec spec;

	if (pred_comp->op != OpID::CALL)
		return ErrPtr(new UnexpectedCompErr(*pred_comp));
	Res<ID> id_res = expect_id(std::move(pred_comp->bin->lhs));
	if (id_res.is_err)
		return id_res.err;
	id = std::move(id_res.res);

	Res<ArgSpec> spec_res = interpret_arg_spec(std::move(pred_comp->bin->rhs));
	if (spec_res.is_err)
		return spec_res.err;
	spec = std::move(spec_res.res);

	Res<Block> body = interpret_body(std::move(comp));
	if (body.is_err)
		return body.err;

	return {
		std::move(id),
		DefCase{
			comp.src_id, std::move(spec), std::move(returns), std::move(body)
		}
	};
}

NodeRes InterpreterImpl::interpret_for(std::vector<Comp>&& comps) {
	ErrPtr err = require_pred_and_body(*comps[0].comp);
	if (err)
		return err;

	using enum OpID;
	switch(comps[0].comp->bin->lhs.op) {
	case IN:
		return interpret_for_each(std::move(comps));
	case FROM:
	case TO:
	case BY:
		return interpret_for_range(std::move(comps));
	default:
		return ErrPtr(new UnexpectedCompErr(comps[0]));
	}
}

NodeRes InterpreterImpl::interpret_for_each(std::vector<Comp>&& comps) {
	Res<ID> id = expect_id(std::move(comps[0].comp->bin->lhs.bin->lhs));
	if (id.is_err)
		return id.err;

	NodeRes iterable = interpret_value(std::move(
		comps[0].comp->bin->lhs.bin->rhs
	));
	if (iterable.is_err)
		return iterable.err;

	Res<Block> body = interpret_body(std::move(comps[0].comp->bin->rhs));
	if (body.is_err)
		return body.err;

	Res<Block> orelse = interpret_optional_else(std::move(comps));
	if (orelse.is_err)
		return orelse.err;

	return NodePtr(new ForEach{
		comps[0].src_id,
		std::move(id.res),
		std::move(iterable.res),
		std::move(body.res),
		std::move(orelse.res)
	});
}

NodeRes InterpreterImpl::interpret_for_range(std::vector<Comp>&& comps) {
	Comp* pred_comp = &comps[0].comp->bin->lhs;

	NodePtr from = nullptr;
	NodePtr to = nullptr;
	NodePtr by = nullptr;

	if (pred_comp->op == OpID::BY) {
		NodeRes res = interpret_value(std::move(pred_comp->bin->rhs));
		if (res.is_err)
			return res.err;
		by = std::move(res.res);
		pred_comp = &pred_comp->bin->lhs;
	}

	if (pred_comp->op == OpID::TO) {
		NodeRes res = interpret_value(std::move(pred_comp->bin->rhs));
		if (res.is_err)
			return res.err;
		to = std::move(res.res);
		pred_comp = &pred_comp->bin->lhs;
	}

	if (pred_comp->op == OpID::FROM) {
		NodeRes res = interpret_value(std::move(pred_comp->bin->rhs));
		if (res.is_err)
			return res.err;
		from = std::move(res.res);
		pred_comp = &pred_comp->bin->lhs;
	}

	if (from != nullptr && to == nullptr)
		return ErrPtr(new FromWithoutToErr(comps[0].src_id));
	if (by != nullptr && to == nullptr)
		return ErrPtr(new ByWithoutToErr(comps[0].src_id));

	Res<ID> id = expect_id(std::move(*pred_comp));
	if (id.is_err)
		return id.err;

	Res<Block> body = interpret_body(std::move(comps[0].comp->bin->rhs));
	if (body.is_err)
		return body.err;

	Res<Block> orelse = interpret_optional_else(std::move(comps));
	if (orelse.is_err)
		return orelse.err;

	return NodePtr(new For{
		comps[0].src_id,
		std::move(id.res),
		std::move(from),
		std::move(to),
		std::move(by),
		std::move(body.res),
		std::move(orelse.res)
	});
}

NodeRes InterpreterImpl::interpret_get(Comp&& comp) {
	if (comp.bin->lhs.op == OpID::VARS)
		return count_up<Var>(std::move(comp.bin->rhs));
	if (comp.bin->rhs.op != OpID::ID)
		return ErrPtr(new ExpectedIDErr(comp.bin->rhs.src_id));
	NodeRes obj = interpret_value(std::move(comp.bin->lhs));
	if (obj.is_err)
		return obj;
	Res<ID> attr = expect_id(std::move(comp.bin->rhs));
	if (attr.is_err)
		return attr.err;
	return NodePtr(
		new GetAttr{comp.src_id, std::move(obj.res), std::move(attr)}
	);
}

NodeRes InterpreterImpl::interpret_group(Comp&& comp) {
	if (comp.comp->id == OpID::NOTHING)
		return NodePtr(new Tuple{comp.src_id, {}});
	if (comp.comp->id != OpID::SEP)
		// Parenthesized value, not a tuple.
		return interpret_value(std::move(*comp.comp));
	Nodes nodes;
	ErrPtr err = csv_to_vec(std::move(comp), nodes);
	if (err)
		return err;
	return NodePtr(new Tuple{comp.src_id, std::move(nodes)});
}

NodeRes InterpreterImpl::interpret_if(
	std::vector<Comp>&& comps, std::uint32_t i
) {
	ErrPtr err = require_pred_and_body(*comps[i].comp);
	if (err)
		return err;
	NodeRes pred = interpret_value(std::move(comp[i].comp->lhs));
	if (pred.is_err)
		return pred.err;

	Res<Block> body = interpret_body(std::move(comp[i].comp->rhs));

	NodeRes orelse;
	
	if (i < comps.size() - 1) {
		switch(comps[i + 1].op) {
		case OpID::ELIF:
			orelse = interpret_if(std::move(comps), i + 1);
			break;
		case OpID::ELSE
			orelse = expect_body_only(std::move(*comps[i + 1].comp));
			break;
		default:
			return ErrPtr(new InconsistentConstructErr(comps[i + 1].src_id));
		}
	}

	if (orelse.is_err)
		return orelse.err;

	return NodePtr(new If{
		comps[0].src_id,
		std::move(pred.res),
		std::move(body.res),
		std::move(orelse.res)
	});
}

NodeRes InterpreterImpl::interpret_kwarg(
	Comp&& comp,
	std::unordered_set<std::string>& kw_set
) {
	using enum OpID;
	switch(comp.op) {
	case BIND:
		Res<ID> bind_id = expect_id(std::move(comp.bin->lhs));
		if (bind_id.is_err)
			return bind_id;
		if (kw_set.count(bind_id.res.id) != 0)
			return ErrPtr(new DuplicateKeywordErr(
				std::move(bind_id.res.id), bind_id.res.src_id
			));
		kw_set.insert(bind_id.res.id);
		NodeRes value = interpret_value(std::move(comp.bin->rhs));
		if (value.is_err)
			return err;
		return NodePtr(new KeywordArg{
			comp.src_id, std::move(bind_id.res), std::move(value.res)
		});
	case UNPACK_KWARGS:
		return interpret_unop<Expansion>(std::move(*comp.comp));
	default:
		return ErrPtr(new PosAfterKeywordErr(comp.src_id));
	}
}

NodeRes InterpreterImpl::interpret_kwarg_def(Comp&& comp) {
	if (comp.op == OpID::DEF_AS)
		return interpret_arg_const(std::move(comp));
	NodeRes arg_def = interpret_arg_def(std::move(comp));
	if (arg_def.is_err)
		return arg_def.err;
	if (arg_def.res.kind() != NodeID::ARG_DEF)
		return UnexpectedCompErr(comp);
	return arg_def;
}

NodeRes InterpreterImpl::interpret_list(Comp&& comp) {
	if (comp.comp->op == OpID::NOTHING)
		return NodePtr(new List{comp.src_id, {}});
	Nodes nodes;
	ErrPtr err = csv_to_vec(std::move(comp), nodes);
	if (err)
		return err;
	return NodePtr(new List{comp.src_id, std::move(nodes)});
}

NodeRes InterpreterImpl::interpret_match(std::vector<Comp>&& comps) {
	NodeRes matchee = interpret_value(std::move(*comps[0].comp));
	if (matchee.is_err)
		return matchee;
	if (comps.size() == 1)
		return ErrPtr(new NoMatchCasesErr(comps[0].src_id));
	std::vector<MatchCase> cases;
	Block orelse{0, {}};
	for (std::uint32_t i = 1; i < comps.size(); i++) {
		if (comps[i].op == OpID::ELSE) {
			Res<Block> orelse_res = expect_body_only(std::move(*comps[i].comp));
			if (orelse_res.is_err)
				return orelse_res.err;
			orelse = std::move(orelse_res.res);
			break;
		}
		Res<MatchCase> mc = interpret_match_case(std::move(comps[i]));
		if (mc.is_err)
			return mc.err;
		cases.push_back(std::move(mc.res));
	}
	return NodePtr(
		new Match{
			comps[0].src_id,
			std::move(matchee.res),
			std::move(cases),
			std::move(orelse)
		}
	);
}

Res<MatchCase> InterpreterImpl::interpret_match_case(Comp&& comp) {
	if (comp.op != OpID::CASE)
		return ErrPtr(new InconsistentConstructErr(comp.src_id));
	ErrPtr err = require_pred_and_body(*comp.comp);
	if (err)
		return err;
	
	NodeRes match_pred = interpret_value(std::move(*comp.comp.bin->lhs));
	if (match_pred.is_err)
		return match_pred.err;
	
	Res<Block> body = interpret_body(std::move(*comp.comp.bin->rhs));
	if (body.is_err)
		return body.err;

	return MatchCase{
		comp.src_id, std::move(match_pred.res), std::move(body.res)
	};
}

NodeRes InterpreterImpl::interpret_not_in(Comp&& comp) {
	NodeRes of = interpret_value(std::move(*comp.comp));
	if (of.is_err)
		return err;
	return NodePtr(
		new Not{comp.src_id, NodePtr(new In{comp.src_id, std::move(of.res)})}
	);
}

NodeRes interpret_number(const Comp& comp) {
	switch(comp.data.index()) {
	case 1:
		return NodePtr(new Int8(std::get<int8_t>(comp.data)));
	case 2:
		return NodePtr(new Int16(std::get<int16_t>(comp.data)));
	case 3:
		return NodePtr(new Int32(std::get<int32_t>(comp.data)));
	case 4:
		return NodePtr(new Int64(std::get<int64_t>(comp.data)));
	case 5:
		return NodePtr(new UInt8(std::get<uint8_t>(comp.data)));
	case 6:
		return NodePtr(new UInt16(std::get<uint16_t>(comp.data)));
	case 7:
		return NodePtr(new UInt32(std::get<uint32_t>(comp.data)));
	case 8:
		return NodePtr(new UInt64(std::get<uint64_t>(comp.data)));
	case 9:
		return NodePtr(new Float32(std::get<float>(comp.data)));
	case 10:
		return NodePtr(new Float64(std::get<double>(comp.data)));
	default:
		return ErrPtr(new UnexpectedCompErr(comp));
	}
}

Res<Block> InterpreterImpl::interpret_optional_else(std::vector<Comp>&& comps) {
	if (comps.size() > 2 || (comps.size() == 2 && comps[1].op != OpID::ELSE))
		// Assuming no bug in the parser, the second part of the construct
		// cannot be else if there are at least 3 parts, because else may be not
		// followed by another part.
		return ErrPtr(new InconsistentConstructErr(comps[1]));
	if (comps.size() == 2)
		return expect_body_only(std::move(*comps[1].comp));
	// Use a default value for Block to indicate lack of else and no error.
	return Block{0, {}};
}

NodeRes InterpreterImpl::interpret_set(Comp&& comp) {
	NodeRes lhs = interpret_value(std::move(comp.bin->lhs));
	if (lhs.is_err)
		return lhs.err;

	NodeRes rhs = interpret_value(std::move(comp.bin->rhs));
	if (rhs.is_err)
		return rhs.err;

	switch(lhs.res->kind()) {
	case NodeID::GET_ATTR: {
		auto& ga = dynamic_cast<GetAttr&>(*lhs.res);
		return NodePtr(new SetAttr{
			comp.src_id, 
			std::move(ga.object),
			std::move(ga.attr),
			std::move(rhs.res)
		});
	}
	case NodeID::CALL: {
		auto& call = dynamic_cast<Call&>(*lhs.res);
		return NodePtr(new Update{
			comp.src_id,
			std::move(call.object),
			std::move(call.args),
			std::move(rhs.res)
		});
	}
	case NodeID::CALL_ATTR: {
		auto& ca = dynamic_cast<CallAttr&>(*lhs.res);
		return NodePtr(new UpdateAttr{
			comp.src_id,
			std::move(ca.object),
			std::move(ca.attr),
			std::move(ca.args),
			std::move(rhs.res)
		});
	}
	default:
		return new Set{comp.src_id, std::move(lhs.res), std::move(rhs.res)};
	}
}

NodeRes InterpreterImpl::interpret_ternary(Comp&& comp) {
	if (comp.bin->lhs.op != OpID::TERNARY_IF)
		return ErrPtr(new UnexpectedCompErr(comp.bin->lhs));
	
	NodeRes predicate = interpret_value(std::move(comp.bin->lhs.bin->lhs));
	if (predicate.is_err)
		return predicate.err;

	NodeRes if_case = interpret_value(std::move(comp.bin->lhs.bin->rhs));
	if (if_case.is_err)
		return if_case.err;

	NodeRes else_case = interpret_value(std::move(comp.bin->rhs));
	if (else_case.is_err)
		return else_case.err;

	return NodePtr(
		new Ternary{
			comp.src_id,
			std::move(predicate.res),
			std::move(if_case.res),
			std::move(else_case.res)
		}
	);
}

NodeRes InterpreterImpl::interpret_try(std::vector<Comp>&& comps) {
	Res<Block> body = expect_body_only(std::move(*comps[0].comp));
	if (body.is_err)
		return body.err;

	std::vector<Except> excepts;
	Block finally{0, {}};

	for (std::uint32_t i = 1, i < comps.size(); i++) {
		if (comps[i].op == OpID::FINALLY) {
			// Finally is necessarily the last comp since it corresponded to a
			// CONSTRUCT_LAST token.
			Res<Block> finally_res = expect_body_only(
				std::move(*comps[i].comp)
			);
			if (finally_res.is_err)
				return finally_res.err;
			break;
		}

		Res<Except> except = interpret_except(std::move(comps[i]));
		if (except.is_err)
			return except.err;
		excepts.push_back(std::move(except.res));
	}
	return NodePtr(new Try{
		comps[0].src_id,
		std::move(body.res),
		std::move(excepts),
		std::move(finally)
	});
}

NodeRes InterpreterImpl::interpret_type(Comp&& comp) {
	ErrPtr err = require_pred_and_body(*comp.comp);
	if (err)
		return err;

	Res<ID> id;
	Nodes parents;

	switch(comp.comp->bin->lhs.op) {
	case OpID::ID:
		id = expect_id(std::move(comp.comp->bin->lhs.op));
		break;
	case OpID::CALL:
		id = expect_id(std::move(comp.comp->bin->lhs.bin->lhs));
		ErrPtr parents_err = csv_to_vec(
			std::move(comp.comp->bin->lhs.bin->rhs), parents
		);
		if (parents_err)
			return parents_err;
		break;
	default:
		return UnexpectedCompErr(comp.comp->bin->lhs);
	}
	if (id.is_err)
		return id.err;

	Res<Block> body = interpret_body(std::move(comp.comp->bin->rhs));
	if (body.is_err)
		return body.err;
	return NodePtr(
		new Type{
			comp.src_id,
			std::move(id.res),
			std::move(parents),
			std::move(body.res)
		};
	);
}

NodeRes InterpreterImpl::interpret_typed_arg_def(Comp&& comp) {
	NodeRes type = interpret_value(std::move(comp.bin->rhs));
	if (type.is_err)
		return std::move(type.err);
	NodeRes def = interpret_basic_arg_def(std::move(comp.bin->lhs));
	if (def.is_err)
		return std::move(def.err);
	using enum NodeID;
	switch(def.res.kind()) {
	case ARG_DEF:
		auto casted = dynamic_cast<ArgDef&>(*def.res);
		return NodePtr(new ArgDef{
			comp.src_id,
			std::move(casted.id),
			std::move(type.res),
			nullptr
		});
	case VAR_ARG_DEF:
		auto casted = dynamic_cast<VarArgsDef&>(*def.res);
		return NodePtr(new VarArgsDef{
			comp.src_id,
			std::move(casted.id),
			std::move(type.res);
		});
	case VAR_KWARG_DEF:
		auto casted = dynamic_cast<VarKwargsDef&>(*def.res);
		return NodePtr(new VarKwargsDef{
			comp.src_id,
			std::move(casted.id),
			std::move(type.res);
		});
	}
	default:
		return ErrPtr(new AssertionFailedErr("Impossible node kind"));
}

template<typename NodeT>
NodeRes InterpreterImpl::interpret_unop(Comp&& comp) {
	NodeRes arg = interpret_value(std::move(*comp.comp));
	if (arg.is_err)
		return arg;
	return NodePtr(new NodeT{comp.src_id, std::move(arg.res)});
}

NodeRes InterpreterImpl::interpret_unop_method(
	Comp&& comp, std::string fn_name
) {
	NodeRes arg = interpret_value(std::move(*comp.comp));
	if (arg.is_err)
		return arg;

	return NodePtr(new CallAttr{
		comp.src_id,
		std::move(arg),
		ID{comp.src_id, std::move(fn_name)},
		Args{comp.src_id, {}, {}}
	});
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
		return NodePtr(new Char(std::get<std::uint32_t>(comp.data)));
	case CONCAT: {
		if (comp.lhs->id != OpID::STRING || comp.rhs->id != OpID::STRING)
			return ErrPtr(new UnexpectedCompErr(comp));
		return NodePtr(new String(
			comp.src_id,
			std::get<std::string>(comp.lhs->data) +
				std::get<std::string>(comp.rhs->data)
		));
	}
	case DIV:
		return interpret_binop_method(std::move(comp), "__div__");
	case EQ:
		return interpret_binop_method(std::move(comp), "__eq__");
	case EXP:
		return interpret_binop_method(std::move(comp), "__exp__");
	case EXPR: {
		Res<ID> res = expect_id(std::move(*comp.comp));
		if (res.is_err)
			return res;
		return NodePtr(new Symbol(std::move(res.res)));
	}
	case FALSE:
		return NodePtr(new Bool(comp.src_id, false));
	case GET:
		return interpret_get(std::move(comp));
	case GROUP:
		return interpret_group(std::move(comp));
	case GT:
		return interpret_binop_method(std::move(comp), "__gt__");
	case GTE:
		return interpret_binop_method(std::move(comp), "__gte__");
	case IN:
		return interpret_binop_method(std::move(comp), "__in__");
	case LABEL:
		return interpret_binop<TypeCheck>(std::move(comp));
	case LIST:
		return interpret_list(std::move(comp)); 
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
		return NodePtr(new None{comp.src_id});
	case NOT:
		return interpret_unop<Not>(std::move(comp));
	case NOT_IN:
		return interpret_not_in(std::move(comp));
	case NUMBER:
		return interpret_number(comp);
	case OR:
		return interpret_binop<Or>(std::move(comp));
	case RSH:
		return interpret_binop_method(std::move(comp), "__rsh__");
	case SEP: {
		Nodes elements;
		ErrPtr err = csv_to_vec(std::move(comp), elements);
		if (err)
			return err;
		return NodePtr(new Tuple{comp.src_id, std::move(elements)});
	}
	case STRING:
		return NodePtr(new String{
			comp.src_id, std::move(std::get<std::string>(comp.data))
		});
	case SUB:
		return interpret_binop_method(std::move(comp), "__sub__");
	case TERNARY_ELSE:
		return interpret_ternary(std::move(comp));
	case THIS:
		return NodePtr(new This{comp.src_id});
	case TRUE:
		return NodePtr(new Bool{comp.src_id, true});
	case UP:
		return count_up<ObjVar>(std::move(*comp.comp));
	default:
		return ErrPtr(new UnexpectedCompErr(comp));
	}
}

NodeRes InterpreterImpl::interpret_while(std::vector<Comp>&& comps) {
	ErrPtr err = require_pred_and_body(*comps[0].comp);
	if (err)
		return err;

	NodeRes pred = interpret_value(std::move(comps[0].comp->bin->lhs));
	if (pred.is_err)
		return pred.err;

	Res<Block> body = interpret_body(std::move(comps[0].comp->bin->rhs));
	if (body.is_err)
		return body.err;

	Res<Block> orelse = interpret_optional_else(std::move(comps));
	if (orelse.is_err)
		return orelse.err;

	return NodePtr(new While{
		comps[0].src_id, std::move(body.res), std::move(orelse.res)
	});
}

ErrPtr InterpreterImpl::require_pred_and_body(const Comp& comp) {
	if (comp.op != OpID::LABELED) {
		if (comp.op == OpID::EXPR)
			return ErrPtr(new MissingPredicateErr(comp.src_id))
		return ErrPtr(new MissingBodyErr(comp.src_id));
	}
	return nullptr;
}

}
