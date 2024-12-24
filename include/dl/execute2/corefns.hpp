#pragma once

#include "dl/interpret/types.hpp"

namespace dl::corefn {

template<typename FromType, typename ToType>
Any convert(Vars& globals, Args& arg, ExcInfo& exc_info) {
    auto from = *static_cast<FromType*>(args.args.xs[0]);
    return Any{}
}

Any dunder_new(Vars& globals, Args& args, ExcInfo& exc_info) {
    auto type = *static_cast<Type*>(args.args.xs[0].data);
    return Any{type.tid, new(type.structure.size)};
}

Any dunder_ptradd(State& state) {
    Args& rgs = coreutil::args(state);
    
    if (rgs.args.len != 2) {
        coreutil::raise(NumArgsError{2, 2, args.args.len}, exc_info);
        return coreutil::ERROR_SIGNAL;
    }

    if (rgs.kwargs.len != 0) {
        auto exc = KeywordsError{new Symbol[rgs.kwargs.len], rgs.kwargs.len};
        for (std::uint32_t i = 0; i < rgs.kwargs.len; i++)
            exc.keywords[i] = rgs.kwargs.names[rgs.kwargs.idxs[i]];

        coreutil::raise(exc);
        return coreutil::ERROR_SIGNAL;
    }

    Any ptr = rgs.args.xs[0];
    Any by = rgs.args.xs[1];
    
    if (!coreutil::is_ptr(state, ptr)) {
        coreutil::raise(ArgTypeError{0, ptr});
        return coreutil::ERROR_SIGNAL;
    }

    if (!coreutil::is_integer(by)) {
        coreutil::raise(ArgTypeError{1, by});
        return coreutil::ERROR_SIGNAL;
    }

    std::int64_t converted = coreutil::integer_as_int64(by);
    std::uint64_t sz = get_size(state, deref_tid(state, ptr.tid));

    return Any{
        ptr.tid,
        static_cast<void*>(static_cast<char*>(ptr.data) + converted * sz)
    };
}

Any dunder_ptrsub(State& state)

}
