#pragma once

#include <cstdint>

#include "dl/interpret/types.hpp"

namespace dl {

struct ArgTypeError {
    std::uint32_t i;
    Any obj;
};

struct KeywordsError {
    Symbol* keywords;
    std::uint32_t len;
};

struct NumArgsError {
    std::uint32_t min;
    std::uint32_t max;
    std::uint32_t actual;
};

struct PtrDepthError {
    Any obj;
    std::uint32_t requested;
    std::uint32_t actual;
};

struct StackOverflowError {
    std::uint32_t max;
};

struct TypeOverflowError {
    std::uint32_t max;
}

struct VarOverflowError {
    std::uint32_t max;
};

}
