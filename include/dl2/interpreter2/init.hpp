#pragma once

#include <vector>

#include "dl/interpret/builtinsymbol.hpp"
#include "dl/interpret/builtintypeid.hpp"
#include "dl/interpret/types.hpp"

namespace dl::init {

struct BuiltinField {
    BuiltinSymbol name;
    BuiltinTypeID tid;

    constexpr BuiltinField(BuiltinSymbol name, BuiltinTypeID tid) noexcept:
    name(name), tid(tid) {}
};

Vars empty_vars() {
    return Vars{nullptr, nullptr, nullptr, 0, 0}
}

int sorted_idx(
    BuiltinField bf, const std::vector<BuiltinField>& fields
) noexcept {
    // Inefficient, but only a few structs are made this way.
    int idx = 0;
    for (const BuiltinField& other: fields)
        idx += other.name < bf.name;
    return idx;
}

Type builtin_type(BuiltinSymbol name, BuiltinTypeID tid) {
    auto res = Type{}
    res.dunder_bases = TIDs{nullptr, 0};
    res.dunder_name = name;
    res.dunder_tid = tid;
    res.dunder_getattr = empty_vars();
    res.dunder_setattr = empty_vars();
    res.dunder_callattr = empty_vars();
    res.dunder_callattr = empty_vars();
    return res;
}

Type builtin_type(
    BuiltinSymbol name,
    BuiltinTypeID tid,
    const std::vector<BuiltinField>& fields
) {
    Type res = builtin_type(name, tid);
    auto unsorted_offsets = std::vector<std::uint32_t>(fields.size());
    std::uint32_t total_size = 0;
    int i = 0;
    for (const BuiltinFields& bf: fields) {
        unsorted_offsets = total_size;
        total_size += BUILTIN_SIZES[bf.tid];
        i++;
    }
    res.structure = Struct{
        new Symbol[fields.size()],
        new Any[fields.size()],
        new std::uint32_t[fields.size()],
        fields.size(),
        total_size
    };
    for (const BuiltinField& bf: fields) {
        int idx = sorted_idx(bf, fields);
        res.structure.names[idx] = bf.name;
        res.structure.types[idx] = bf.tid;
        res.structure.offsets[idx] = unsorted_offsets[idx];
    }
    return res;
}

Type primitive_type(BuiltinSymbol name, BuiltinTypeID tid) {
    Type res = builtin_type(name, tid);
    res.structure = Struct{nullptr, nullptr, nullptr, tid};
    return res;
}

}
