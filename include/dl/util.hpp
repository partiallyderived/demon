#pragma once

#include <cstdint>

#include <ostream>

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

}
