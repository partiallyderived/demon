#pragma once

#include <cassert>
#include <cstdint>

#include "dl/interpet/types.hpp"

namespace dl::coreutil {

constexpr std::uint32_t MAX_TID = UINT32_MAX - BuiltinTypeID::DUNDER_FLOAT64;
constexpr auto ERROR_SIGNAL = Any{BuiltinTypeID::ERROR_SIGNAL, nullptr};

Args& args(State& state) noexcept {
    return state.stack.args[state.stack.call_depth];
}

Def core_def(State& state, BuiltinSymbol name) noexcept {
    return unwrap<Def>(
        get_var(name, unwrap<Vars>(
            get_var(BuiltinSymbol::TRUNDER_COREDEFS, globals(state))
        ))
    );
}

Any deref(State& state, Any obj) noexcept {
    return Any{
        deref_tid(state, obj.tid), *static_cast<void**>(obj.data)
    };
}

std::uint32_t deref_tid(State& state, std::uint32_t tid) noexcept {
    return get_type(state, tid).dunder_struct.len;
}

Any deref_until(State& state, Any obj, std::uint32_t naddr) noexcept {
    // Dereference until the pointer depth matches naddr.
    std::uint32_t numptr = nptr(obj);
    if (naddr > nptr) {
        // Not indirected enough.
        raise(state, PtrDepthError{obj, naddr, nptr});
        return ERROR_SIGNAL;
    }
    for (std::uint32_t i = 0; i < nptr - naddr; i++)
        obj = deref(state, obj);
    return obj;
}

Vars empty_vars() {
    return Vars{nullptr, nullptr, nullptr, 0, 0};
}

Any get_method(State& state, Any obj, Symbol name) noexcept {
    return get_var(symbol, get_type(state, obj).dunder_callattr);
}

std::uint64_t get_size(State& state, Any obj) noexcept {
    return get_size(state, obj.tid);
}

std::uint64_t get_size(State& state, std::uint32_t tid) noexcept {
    return get_type(state, tid).dunder_struct.size;
}

Type& get_type(State& state, Any obj) noexcept {
    return get_type(state, obj.tid);
}

Type& get_type(State& state, std::uint32_t tid) noexcept {
    return *static_cast<Type*>(types(state).xs[tid].data);
}

Any get_var(Symbol name, const Vars& vars) noexcept {
    std::uint32_t idx = name % vars.cap;
    Symbol found = vars.names[idx];
    while (found != name) {
        if (found.name == BuiltinSymbols::NO_SYMBOL)
            return ERROR_SIGNAL;
        idx = (idx + 1) % cap;
        found = vars.names[idx];
    }
    return vars.data[idx];
}

Vars& globals(State& state) {
    return *state.stack.scope[0];
}

std::int64_t integer_as_int64(Any integer) noexcept {
    using enum BuiltinTypeID;

    switch(static_cast<BuiltinTypeID>(integer.tid)) {
    case BOOL:
        return std::int64_t(*static_cast<bool*>(integer.data));
    case UINT8:
        return std::int64_t(*static_cast<std::uint8_t*>(integer.data));
    case UINT16:
        return std::int64_t(*static_cast<std::uint16_t*>(integer.data));
    case UINT32:
        return std::int64_t(*static_cast<std::uint32_t*>(integer.data));
    case UINT64:
        return std::int64_t(*static_cast<std::uint64_t*>(integer.data));
    case INT8:
        return std::int64_t(*static_cast<std::int8_t*>(integer.data));
    case INT16:
        return std::int64_t(*static_cast<std::int16_t*>(integer.data));
    case INT32:
        return std::int64_t(*static_cast<std::int32_t*>(integer.data));
    case INT64:
        return *static_cast<std::int64_t*>(integer.data);
    default:
        assert(false);
    }
}

bool is_error(Any obj) {
    return obj.tid == BuiltinTypeID::ERROR_SIGNAL;
}

bool is_integer(Any obj) {
    return obj.tid >= BuiltinTypeID::BOOL && obj.tid <= BuiltinTypeID::UINT64;
}

bool is_ptr(State& state, Any obj) {
    return is_ptr_type(state, obj.tid);
}

bool is_ptr_type(State& state, std::uint32_t tid) {
    Struct& structure = get_type(state, tid).dunder_struct;
    return structure.names == nullptr && structure.len < MAX_TID;
}

bool issubclass(
    State& state, std::uint32_t child_tid, std::uint32_t parent_tid
) {
    Type& child_type = get_type(state, child_tid);
    for (std::uint32_t i = 0; i < child_type.dunder_bases.len; i++) {
        std::uint32_t base_tid = child_type.dunder_bases.tids[i];
        if (base_tid == parent_tid || issubclass(state, base_tid, parent_tid))
            return true;
    }
    return false;
}

void make_ptr_type(State& state, std::uint32_t tid) {
    auto t = PtrType{};
    t.dunder_struct = Struct{nullptr, nullptr, nullptr, tid, sizeof(void*)};
    t.dunder_bases = TIDs{nullptr, 0};
    t.dunder_name = BuiltinSymbol::PTR;
    
    t.dunder_getattr = empty_vars();
    t.dunder_setattr = empty_vars();

    // TODO: should have iadd and isub
    t.dunder_callattr = empty_vars();
    t.dunder_updateattr = empty_vars();

    t.dunder_add = core_def(state, BuiltinSymbol::PTRADD);
    t.dunder_sub = core_def(state, BuiltinSymbol::PTRSUB);

    register_type(state, wrap(t));
}

std::uint32_t nptr(State& state, std::uint32_t tid) {
    // Get the level of indirection of a type, that is, the number of times an
    // object of that type must be dereferenced to get a non-pointer.
    std::uint32_t i = 0;
    while (is_ptr_type(state, tid)) {
        // TID of pointed-to type encoded into len.
        tid = deref_tid(state, tid);
        i++;
    }
    return i;
}

template<typename E>
void raise(State& state, E exc) {
    state.exc_info.raised = wrap(exc);
    state.exc_info.origin = Source();
}

std::uint32_t register_type(State& state, Any t) {
    Seq& ts = types(state);
    if (ts.len == state.config.max_types)
        raise(state, TypeOverflowError{state.config.max_types});
    static_cast<Type*>(t.data)->tid = ts.len;
    ts[ts.len++] = t;
    return ts.len - 1;
}

int set(State& state, Any from, Any to) {
    Any set_method = get_method(state, from, BuiltinSymbol::DUNDER_SET);
    if (set_method.tid == BuiltinTypeID::ERROR_SIGNAL)
        return set0(state, from, to);
    Any res = call_method1(state, set_method, from, to);
    if (res.tid == BuiltinTypeID::ERROR_SIGNAL)
        return *static_cast<int*>(res.data);
    return 0;
}

int set0(State& state, Any from, Any to) {
    if (!issubclass(state, from, to))
        return 1;
    std::uint64_t sz = get_size(state, to);
    std::memcpy(from.data, to.data, sz);
    return 0;
}

int InterpreterImpl::set_var(
    State& state, Symbol name, Any value, Vars& vars, std::uint32_t naddr
) {
    std::uint32_t idx = name % vars.cap;
    Symbol found = vars.names[idx];
    while (true) {
        if (found == BuiltinSymbols::NO_SYMBOL) {
            if (vars.len == vars.cap) {
                raise(VarOverflowError{vars.cap});
                return 1;
            }
            vars.idxs[vars.len++] = idx;
            vars.data[idx] = value;
            return 0;
        }

        if (found == name) {
            Any data = vars.data[idx];
            data = deref_until(state, data, naddr);
            if (is_error(data))
                return 1;
            return set(state, data, value);
        }
        idx = (idx + 1) % vars.cap;
    }
}

Seq& types(State& state) noexcept {
    return *static_cast<Seq*>(
        get_var(BuiltinSymbol::TRUNDER_TYPES, state.globals).data
    );
}

template<typename T>
T& unwrap(Any obj) {
    return *static_cast<T*>(obj.data);
}

template<typename T>
Any wrap(T obj) {
    return Any{tid_for<T>, new T(obj)};
}

}