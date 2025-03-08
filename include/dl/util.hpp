#pragma once

#include <cstdint>

#include <ostream>
#include <type_traits>

namespace dl {

template<typename Container>
struct OutContainerManip {
    const Container& container;
    OutContainerManip(
        const Container& container
    ) noexcept: container(container) {}
};

template<typename Container>
std::ostream& operator<<(
    std::ostream& os,
    const OutContainerManip<Container> manip
) {
    os << '[';
    if (!manip.container.empty()) {
        os << manip.container[0];
        for (std::uint32_t i = 1; i < manip.container.size(); i++)
            os << ", " << manip.container[i];
    }
    return os << ']';
}

template<typename HeadType, typename... TailTypes>
std::ostream& out_csv(
    std::ostream& os, const HeadType& x, const TailTypes&... args
) {
    if (std::is_same_v<HeadType, bool>)
        os << std::boolalpha;
    os << x;
    if constexpr (sizeof...(TailTypes) > 0) {
        os << ", ";
        return out_csv(os, args...);
    }
    return os;
}

}
