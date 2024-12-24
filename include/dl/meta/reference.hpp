#pragma once

namespace dl {

// A reference wrapper with an equality operator. For use in `Res`.
template<typename Type>
struct Reference {
    Type& ref;

    constexpr Reference(Type& ref) noexcept: ref(ref) {}

    bool operator==(const Type& that) const noexcept {
        return ref == that;
    }
};

template<typename Type>
std::ostream& operator<<(std::ostream& os, const Reference<Type>& ref) {
    return os << ref.ref;
}

}
