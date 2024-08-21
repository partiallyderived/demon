#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "dl/err.hpp"
#include "dl/meta/canderef.hpp"
#include "dl/meta/caneq.hpp"

namespace dl {

// Simple struct which defines a union between a non-error "result" and an      
// error.
template<typename ResType, typename ErrType = ErrPtr>
struct Res {
    union {
        // The non-error result if no error occurred.
        ResType res;

        // The error result if an error occurred.
        ErrType err;
    };

    // Flag indicating whether the error path was chosen.
    bool is_err;

    // Create using a temporary result.
    Res(Res<ResType, ErrType>&& result) noexcept: is_err(result.is_err) {
        if (is_err)
            err = std::move(result.err);
        else
            res = std::move(result.res);
    }

    // Create using a `T`.
    constexpr Res(const ResType& res) noexcept: res(res), is_err(false) {}

    // Create using a temporary `T`.
    constexpr Res(ResType&& res) noexcept: res(std::move(res)), is_err(false) {}

    // Create using a pointer to an error.
    constexpr Res(const ErrType& err) noexcept: err(err), is_err(true) {}

    // Create using an `ErrPtr`.
    constexpr Res(ErrType&& err) noexcept: err(std::move(err)), is_err(true) {}

    // Create using `nullptr`, which must indicate success since errors must
    // never be null.
    template<typename T = ResType, typename = std::enable_if_t<can_deref<T>>>
    constexpr Res(std::nullptr_t null) noexcept: res(nullptr), is_err(false) {}

    template<typename RHSType>
    bool operator==(const RHSType& that) const noexcept {
        if (is_err) {
            if constexpr (can_eq<ErrType, RHSType>)
                return err == that;
            return false;
        }
        if constexpr (can_eq<ResType, RHSType>)
            return res == that;
        return false;
    }

    bool operator==(const ResType& that) const noexcept {
        return !is_err && res == that;
    }

    bool operator==(const ErrType& that) const noexcept {
        return is_err && err == that;
    }

    Res<DerefType<ResType>, DerefType<ErrType>> operator*() {
        if (is_err) {
            if constexpr (can_deref<ErrType>)
                return DerefType<ErrType>(*err);
            else
                return err;
        }
        if constexpr (can_deref<ResType>)
            return DerefType<ResType>(*res);
        else
            return res;
    }

    // Need to explicitly destructor of active union member.
    ~Res() noexcept {
        if (is_err)
            err.~ErrType();
        else
            res.~ResType();
    }
};

// Helper function to continue outputting ampersands until a non-dereferencable
// value is found, which is then output following the ampersands.
template<typename Type>
std::ostream& out_recurse(std::ostream& os, const Type& x) {
    if constexpr (can_deref<Type>) {
        if (x == nullptr)
            return os << "nullptr";
        return out_recurse(os << "&", *x);
    }
    return os << x;
}

template<typename ResType, typename ErrType>
std::ostream& operator<<(std::ostream& os, const Res<ResType, ErrType>& res) {
    os << "Res(";
    if (res.is_err)
        return out_recurse(os, res.err) << ")";
    return out_recurse(os, res.res) << ")";
}

}
