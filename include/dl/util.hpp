#pragma once

#include <cstdint>

#include <ostream>
#include <type_traits>
#include <vector>

namespace dl {

template<typename ElementType>
std::vector<ElementType> deep_copy(const std::vector<ElementType>& v) {
    std::vector<ElementType> res;
    for (const ElementType& x: v)
        res.push_back(x.copy());
    return res;
}

template<typename ElementType>
std::vector<ElementType> deep_copy_ptr(const std::vector<ElementType>& v) {
    std::vector<ElementType> res;
    for (const ElementType& x: v) {
        if (x)
            res.push_back(x->copy_ptr());
        else
            res.push_back(nullptr);
    }
    return res;
}

template<typename Container>
struct OutContainerManip {
    const Container& container;
    OutContainerManip(const Container& container) noexcept:
    container(container) {}
};

template<typename Container>
std::ostream& operator<<(
    std::ostream& os, const OutContainerManip<Container>& manip
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
