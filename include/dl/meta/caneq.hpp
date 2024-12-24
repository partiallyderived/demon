#pragma once

#include <type_traits>

namespace dl {

template<typename LHSType, typename RHSType>
decltype(static_cast<void>(
    std::declval<LHSType>() == std::declval<RHSType>()
), std::true_type{}) can_eq_impl(int);

template<typename, typename>
std::false_type can_eq_impl(...);

template<typename LHSType, typename RHSType>
constexpr bool can_eq = decltype(can_eq_impl<LHSType, RHSType>(0))::value;

}
