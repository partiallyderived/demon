#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

#include "dl/err.hpp"

namespace dl {

using Data = std::variant<
    std::monostate,
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    std::string,
    ErrPtr
>;

Data copy_data(const Data& d) {
    return std::visit([](const auto& x) {
        if constexpr(std::is_same_v<decltype(x), const ErrPtr&>)
            return Data(x->copy());
        else
            return Data(x);
    }, d);
}

std::ostream& operator<<(std::ostream& os, const Data& d) {
    std::visit([&os](const auto& x) {
        if constexpr (std::is_same_v<decltype(x), const ErrPtr&>)
            os << *x;
        else if constexpr (!std::is_same_v<decltype(x), const std::monostate&>)
            os << x;
    }, d);
    return os;
}

}
