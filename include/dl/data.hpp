#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <variant>

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
    float,
    double,
    std::string
>;

std::ostream& operator<<(std::ostream& os, const Data& d) {
    std::visit([&os](auto x) {
        if constexpr (!std::is_same_v<decltype(x), std::monostate>)
            os << x;
    }, d);
    return os;
}

}
