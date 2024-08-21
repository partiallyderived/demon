#pragma once

#include <ostream>

#include "dl/pos.hpp"

namespace dl {

// Represents an object which has a corresponding position in a file.
template<typename Type>
struct Located {
    Type obj;
    Pos pos;

    Located(Type obj, Pos pos) noexcept: obj(obj), pos(pos) {}

    template<typename TargetType>
    Located<TargetType> replace(TargetType other) const noexcept {
        return Located(other, pos);
    }
}

template<typename Type>
std::ostream& operator<<(std::ostream& os, const Located<Type>& loc) {
    return os << loc.obj << " at " << loc.pos;
}

}
