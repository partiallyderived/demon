#pragma once

#include <type_traits>

#include "dl/meta/reference.hpp"

namespace dl {

// https://stackoverflow.com/a/55377775
template<typename Type>
decltype(static_cast<void>(*std::declval<Type>()), std::true_type{})
can_deref_impl(int);

template<typename>
std::false_type can_deref_impl(...);

template<typename Type>
constexpr bool can_deref = decltype(can_deref_impl<Type>(0))::value;

template<typename Type, typename Enable = void>
struct DerefTypeImpl {
    using T = Type;
};

template<typename Type>
struct DerefTypeImpl<Type, std::enable_if_t<can_deref<Type>>> {
    using T = Reference<std::remove_reference_t<
        decltype(*std::declval<Type>())
    >>;
};

template<typename Type>
using DerefType = typename DerefTypeImpl<Type>::T;

}
