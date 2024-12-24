#pragma once

#include <cstdint>

#include <ostream>

namespace dl {

template<class Container>
std::ostream& out_container(std::ostream& os, const Container& c) {
    os << '[';
    if (!c.empty()) {
        os << c[0];
        for (std::uint32_t i = 1; i < c.size(); i++)
            os << ", " << c[i];
    }
    return os << ']';
}

}
