#pragma once

#include <cstdint>

#include "dl/err.hpp"
#include "dl/types.hpp"
#include "dl/executor/executor.hpp"

namespace dl {

struct ExecutorImpl: Executor {
    State state;

    void del(Typed x) {
        Type t = state.types.types[x.typeidx];
        
        delete x.data;
    }

    Typed execute(Typed node) override;

    Typed execute_bool(bool x) {
        return Typed{
            static_cast<std::uint32_t>(BUILTIN_TYPES::BOOL), new bool(x)
        };
    }

    Typed execute_id(Word word) {

    }

    Typed execute_int32(std::int32_t x) {
        return Typed{
            static_cast<std::uint32_t>(BUILTIN_TYPES::INT32),
            new std::int32_t(x)
        };
    }

    Typed execute_str(Word word) {
        return Typed{
            static_cast<std::uint32_t>(BUILTIN_TYPES::STRING), new Word(word)
        };
    }
};

Typed ExecutorImpl::execute(Typed node) {
    switch(node.typeidx) {
        
    }
}

}