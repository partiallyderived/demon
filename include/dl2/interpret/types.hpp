#pragma once

#include <cstdint>

#include <type_traits>
#include <utility>

namespace dl {

using Symbol = std::uint32_t;

struct None {};

struct Any {
    std::uint32_t tid;
    void* data;
};

struct Seq {
    Any* xs;
    std::uint32_t len;
};

struct Vars {
    Symbol* names;
    Any* data;
    std::uint32_t* idxs;
    std::uint32_t len;
    std::uint32_t cap;
};

struct Args {
    Seq args;
    Vars kwargs;
};

struct Struct {
    Symbol* names;
    std::uint32_t* tids;
    std::uint32_t* offsets;
    std::uint32_t len;
    std::uint64_t size;
};

struct TIDs {
    std::uint32_t* tids;
    std::uint32_t len;
};

struct Type {
    Struct dunder_struct;
    TIDs dunder_bases;
    Symbol dunder_name;
    std::uint32_t dunder_tid;

    Vars dunder_getattr;
    Vars dunder_setattr;
    Vars dunder_callattr;
    Vars dunder_updateattr;
};

struct PtrType: Type {
    Def dunder_add;
    Def dunder_sub;
};

struct Stack {
    Vars* scope;
    std::uint32_t scope_depth;
    Def* exc_handler;
    bool* is_entry;

    Args* args;
    std::uint32_t call_depth;
};

struct Config {
    std::uint32_t max_args;
    std::uint32_t max_call_depth;
    std::uint32_t max_kwargs;
    std::uint32_t max_scope_depth;
    std::uint32_t max_symbols;
    std::uint32_t max_src;
    std::uint32_t max_types;
    std::uint32_t max_vars;
};

struct ExcInfo {
    Any raised;
    Source origin;
};

struct State {
    Config config;
    Stack stack;
    ExcInfo exc_info;
    bool terminating;
};

using FnPtr = (Any)(*)(State&);

}
