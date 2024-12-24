#pragma once

#include <cstdint>
#include <cstring>

#include "dl/interpret/builtinsymbol.hpp"
#include "dl/interpret/builtintypeid.hpp"
#include "dl/interpret/types.hpp"

namespace dl {

struct InterpreterImpl {
	static constexpr std::uint32_t MAX_TID =
		UINT32_MAX - BuiltinTypeID::DUNDER_FLOAT64;
	
	State state;
	Seq* types;
	TIDs* ptr_tids;
};

Any InterpreterImpl::call(Call c) {
	if (state.stack.call_depth == state.config.max_call_depth)
		raise(stack_overflow_error);
	state.stack.call_depth++;
	std::uint32_t i = 0;
	for (std::uint32_t j = 0; j < c.nargs; j++) {
		if (c.pos_unpacked[j]) {

		}
	}
	Type* type = c.callee.qtype.type;
	if (type == &FUNC_TYPE)

	else if (type == &CALL_TYPE)
	else
}

Any InterpreterImpl::call0(Any callee) {
	auto c = Call{};
	c.callee = callee;

	c.nargs = 0;
	c.args = nullptr;
	c.pos_unpacked = nullptr;

	c.nkwargs = 0;
	c.kws = nullptr;
	c.kwargs = nullptr;
	c.kw_unpacked = nullptr;
	return call(c);
}

Any InterpreterImpl::call1(Any callee, Any x) {
	auto c = Call{};
	c.callee = callee;
	
	c.nargs = 1;
	c.args = new Any(x);
	c.pos_unpacked = new bool(false);

	c.nkwargs = 0;
	c.kws = nullptr;
	c.kwargs = nullptr;
	c.kw_unpacked = nullptr;
	return call(c);
}

Any InterpreterImpl::call2(Any callee, Any x, Any y) {
	auto c = Call{};
	c.callee = callee;

	c.nargs = 2;
	c.args = new Any[]{x, y};
	c.pos_unpacked = new bool[]{false, false};

	c.nkwargs = 0;
	c.kws = nullptr;
	c.kwargs = nullptr;
	c.kw_unpacked = nullptr;
	return call(c);
}

void InterpreterImpl::del(Any arg) {
	
}

void InterpreterImpl::del_vars(Vars& vars) {
	for (std::uint32_t i = 0; i < vars.len; i++) {
		std::uint32_t j = vars.idxs[i];
		vars.syms[j] = static_cast<Symbol>(BuiltinSymbols::NO_SYMBOL);

	}
}

Any InterpreterImpl::deref(Any any) {
	return Any{
		types->xs[any.tid].dunder_struct.len, static_cast<void*>(*any.data)
	};
}

void InterpreterImpl::execute(Any arg) {
	
}

void InterpreterImpl::exit() {

}

Any get_method(Any obj, Symbol name) {
	return get_var(symbol, get_type(obj)->dunder_callattr);
}

Type* InterpreterImpl::get_type(Any obj) noexcept {
	return get_type(obj.tid);
}

Type* InterpreterImpl::get_type(std::uint32_t tid) noexcept {
	return static_cast<Type*>(types->xs[tid].data);
}

Any InterpreterImpl::get_var(Symbol name, const Vars& vars) {
    std::uint32_t idx = name % cap;
    Symbol found = vars.names[idx];
    while (found != name) {
        if (found.name == BuiltinSymbols::NO_SYMBOL)
            return Any{BuiltinTypeID::ERROR_SIGNAL, nullptr};
        idx = (idx + 1) % cap;
        found = vars.names[idx];
    }
    return vars.data[idx];
}

void InterpreterImpl::init() {
	// First, make all the core types without full initialization.
	Type none_type = builtin_type(
		BuiltinSymbol::NONE_TYPE, BuiltinTypeID::NONE_TYPE
	);
	none_type.structure = Struct{nullptr, nullptr, nullptr, 0, 0};

	// Primitives.
	Type address_type =
		primitive_type(BuiltinSymbol::ADDRESS, BuiltinTypeID::ADDRESS);
	Type fn_ptr_type =
		primitive_type(BuiltinSymbol::FN_PTR, BuiltinTypeID::FN_PTR);
	Type bool_type =
		primitive_type(BuiltinSymbol::BOOL, BuiltinTypeID::BOOL);
	Type int8_type =
		primitive_type(BuiltinSymbol::INT8, BuiltinTypeID::INT8);
	Type int16_type =
		primitive_type(BuiltinSymbol::INT16, BuiltinTypeID::INT16);
	Type int32_type =
		primitive_type(BuiltinSymbol::INT32, BuiltinTypeID::INT32);
	Type int64_type =
		primitive_type(BuiltinSymbol::INT64, BuiltinTypeID::INT64);
	Type uint8_type =
		primitive_type(BuiltinSymbol::UINT8, BuiltinTypeID::UINT8);
	Type uint16_type =
		primitive_type(BuiltinSymbol::UINT16, BuiltinTypeID::UINT16);
	Type uint32_type =
		primitive_type(BuiltinSymbol::UINT32, BuiltinTypeID::UINT32);
	Type uint64_type =
		primitive_type(BuiltinSymbol::UINT64, BuiltinTypeID::UINT64);
	Type float32_type =
		primitive_type(BuiltinSymbol::FLOAT32, BuiltinTypeID::FLOAT32);
	Type float64_type =
		primitive_type(BuiltinSymbol::FLOAT64, BuiltinTypeID::FLOAT64);

	// Symbol is distinct from UInt32 even though the underlying data type
	// is the same.
	Type symbol_type =
		primitive_type(BuiltinSymbol::SYMBOL, BuiltinTypeID::UINT32);

	// Now composite types, composed of primitives and other composites.
	Type any_type = builtin_type(
		BuiltinSymbol::ANY,
		BuiltinTypeID::ANY,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::QTYPE, BuiltinTypeID::QUALIFIED),
			BuiltinField(BuiltinSymbol::DATA, BuiltinTypeID::ADDRESS)
		}
	);

	Type def_type = builtin_type(
		BuiltinSymbol::DEF,
		BuiltinTypeID::DEF,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::CODE, BuiltinTypeID::ANY, 1),
			BuiltinField(BuiltinSymbol::LEN, BuiltinTypeID::UINT32),
			BuiltinField(BuiltinSymbol::NAME, BuiltinTypeID::SYMBOL)
		}
	);

	Type seq_type = builtin_type(
		BuiltinSymbol::SEQ,
		BuiltinTypeID::SEQ,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::XS, BuiltinTypeID::ANY, 1),
			BuiltinField(BuiltinSymbol::LEN, BuiltinTypeID::UINT32)
		}
	);

	Type fixed_vars_type = builtin_type(
		BuiltinSymbol::FIXED_VARS,
		BuiltinTypeID::FIXED_VARS,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::NAMES, BuiltinTypeID::SYMBOL, 1),
			BuiltinField(BuiltinSymbol::DATA, BuiltinTypeID::ANY, 1),
			BuiltinField(BuiltinSymbol::IDXS, BuiltinTypeID::UINT32, 1),
			BuiltinField(BuiltinSymbol::LEN, BuiltinTypeID::UINT32)
		}
	);

	Type struct_type = builtin_type(
		BuiltinSymbol::STRUCT_CAP,
		BuiltinTypeID::STRUCT,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::NAMES, BuiltinTypeID::SYMBOL, 1),
			BuiltinField(BuiltinSymbol::TYPES, BuiltinTypeID::QUALIFIED, 1),
			BuiltinField(BuiltinSymbol::OFFSETS, BuiltinTypeID::UINT32, 1),
			BuiltinField(BuiltinSymbol::LEN, BuiltinTypeID::UINT32),
			BuiltinField(BuiltinSymbol::SIZE, BuiltinTypeID::UINT64)
		}
	);

	Type type_seq_type = builtin_type(
		BuiltinSymbol::TYPE_SEQ,
		BuiltinTypeID::TYPE_SEQ,
		std::vector<BuiltinField>{
			BuiltinField(BuiltinSymbol::TIDS, BuiltinTypeID::UINT32, 1),
			BuiltinField(BuiltinSymbol::LEN, BuiltinTypeID::UINT32)
		}
	);

	Type type_type = builtin_type(
		BuiltinSymbol::TYPE,
		BuiltinTypeID::TYPE,
		std::vector<BuiltinField>{
			BuiltinField(
				BuiltinSymbol::DUNDER_STRUCT, BuiltinTypeID::STRUCT
			),
			BuiltinField(
				BuiltinSymbol::DUNDER_BASES, BuiltinTypeID::TYPE_SEQ
			)
			BuiltinField(BuiltinSymbol::DUNDER_NAME, BuiltinTypeID::SYMBOL),
			BuiltinField(BuiltinSymbol::DUNDER_TID, BuiltinTypeID::UINT32),
			BuiltinField(
				BuiltinSymbol::DUNDER_GET, BuiltinTypeID::FIXED_VARS
			),
			BuiltinField(
				BuiltinSymbol::DUNDER_SET, BuiltinTypeID::FIXED_VARS
			),
			BuiltinField(
				BuiltinSymbol::DUNDER_CALL_ATTR, BuiltinTypeID::FIXED_VARS
			),
			BuiltinField(
				BuiltinSymbol::DUNDER_UPDATE_ATTR, BuiltinTypeID::FIXED_VARS
			)
		}
	);
}

bool InterpreterImpl::is_ptr(Any obj) {
	return is_ptr_type(obj.tid);
}

bool InterpreterImpl::is_ptr_type(std::uint32_t tid) {
	Struct& structure = get_type(tid)->dunder_struct;
	return structure.names == nullptr && structure.len < MAX_TID;
}

bool issubclass(std::uint32_t child_tid, std::uint32_t parent_tid) {
	Type* child_type = get_type(child_tid);
	for (std::uint32_t i = 0; i < child_type->dunder_bases.len; i++) {
		std::uint32_t base_tid = child_type->dunder_bases.tids[i];
		if (base_tid == parent_tid || issubclass(base_tid, parent_tid))
			return true;
	}
	return false;
}

std::uint32_t InterpreterImpl::nptr(std::uint32_t tid) {
	// Get the level of indirection of a type, that is, the number of times an
	// object of that type must be dereferenced to get a non-pointer.
	std::uint32_t i = 0;
	while (is_ptr(tid)) {
		// TID of pointed-to type encoded into len.
		tid = types->xs[tid].dunder_struct.len;
		i++;
	}
	return i;
}

void InterpreterImpl::pos_unpack(Any args) {
	Type* type = types[args.tid];
	Any iter_method =
		get_var(type.dunder_callattr, BuiltinSymbol::DUNDER_ITER);
	if (iter_method.tid == 0)
		raise(not_iterable_error(args));
	Any iter = call1(iter_method, args);
	if (iter.tid == 0)
		return;
	Type* iter_type = types[iter.tid];
	
	Any adv_method =
		get_var(iter_type.dunder_callattr, BuiltinSymbol::DUNDER_ADVANCE);
	if (adv_method.tid == 0)
		raise(no_iter_advance_method_error(iter));
	
	Any item_method =
		get_var(iter_type.dunder_callattr, BuiltinSymbol::DUNDER_ITEM);
	if (item_method.tid == 0)
		raise(no_iter_item_method_error(iter));
	
	Any end_method =
		get_var(iter_type.dunder_callattr, BuiltinSymbol::DUNDER_END);
	if (end_method.tid == 0)
		raise(no_iter_end_method_error(iter));

	Any is_end = call1(end_method, iter);
}

std::uint32_t InterpreterImpl::ptr_tid_for(std::uint32_t tid) {
	std::uint32_t res = ptr_tids->tids[tid];
	if (res == 0) {

	}

}

Any InterpreterImpl::ptr_to(Any any) {
	std::uint32_t ptr_tid = ptr_tids[any.tid];
	if (ptr_tid == 0)

}

Type InterpreterImpl::ptr_type_for(std::uint32_t tid) {
	auto t = PtrType{};
	t.dunder_struct = Struct{nullptr, nullptr, nullptr, tid, sizeof(void*)};
	t.dunder_bases = TIDs{nullptr, 0};
	t.dunder_name = BuiltinSymbol::PTR;
	t.dunder_tid = *types_len_ptr;
	*types_len_ptr += 1;
}

void InterpreterImpl::raise(Any exc, Source origin) {
	state.exc_info = ExcInfo{exc, origin};
}

int InterpreterImpl::set(Any from, Any to) {
	Any set_method = get_method(from, BuiltinSymbol::DUNDER_SET);
    if (set_method.tid == BuiltinTypeID::ERROR_SIGNAL)
    	return set0(from, to);
	Any res = call_method1(set_method, from, to);
	if (res.tid == BuiltinTypeID::ERROR_SIGNAL)
		return *static_cast<int*>(res.data);
	return 0;
}

int InterpreterImpl::set0(Any from, Any to) {
	if (!issubclass(from, to))
		return 1;
	std::uint64_t sz = get_type(from)->dunder_struct.size;
	std::memcpy(from.data, to.data, sz);
	return 0;
}

int InterpreterImpl::set_var(
	Symbol name, Any value, Vars& vars, std::uint32_t naddr
) {
    std::uint32_t idx = name % vars.cap;
    Symbol found = vars.names[idx];
    while (true) {
        if (found == BuiltinSymbols::NO_SYMBOL) {
            if (vars.len == vars.cap)
                return 1;
            vars.idxs[vars.len++] = idx;
            vars.data[idx] = value;
            return 0;
        }

        if (found == name) {
            Any data = vars.data[idx];
            std::uint32_t numptr = nptr(res);
            if (naddr > nptr)
            	return 2;
            for (std::uint32_t i = 0; i < nptr - naddr; i++)
            	data = deref(data);
            return set(data, value);
        }
        idx = (idx + 1) % vars.cap;
    }
}

}
