#pragma once

#include <iostream>
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

    // Default constructor. Assumes `ResType` has a default constructor.
    Res() noexcept: res(), is_err(false) {}

    // Create using a temporary result.
    Res(Res&& result) noexcept: is_err(result.is_err) {
        if (is_err)
            new(&err) ErrType(std::move(result.err));
        else
            new(&res) ResType(std::move(result.res));
    }

    // Create using a `ResType`.
    constexpr Res(const ResType& res): res(res), is_err(false) {}

    // Create using a temporary `ResType`.
    constexpr Res(ResType&& res) noexcept: res(std::move(res)), is_err(false) {}

    // Create using an `ErrType`.
    constexpr Res(const ErrType& err): err(err), is_err(true) {}

    // Create using a temporary `ErrType`.
    constexpr Res(ErrType&& err) noexcept: err(std::move(err)), is_err(true) {}

    // Create using `nullptr`, which must indicate success since errors must
    // never be null.
    template<typename T = ResType, typename = std::enable_if_t<can_deref<T>>>
    constexpr Res(std::nullptr_t null) noexcept: res(nullptr), is_err(false) {}

    Res& operator=(const ResType& that) {
        if (is_err) {
            err.~ErrType();
            is_err = false;
            new(&res) ResType(that);
        } else
            res = that;
        return *this;
    }

    Res& operator=(ResType&& that) noexcept {
        if (is_err) {
            err.~ErrType();
            is_err = false;
            new(&res) ResType(std::move(that));
        } else
            res = std::move(that);
        return *this;
    }

    Res& operator=(const ErrType& that) noexcept {
        if (!is_err) {
            res.~ResType();
            is_err = true;
            new(&err) ErrType(that);
        } else
            err = that;
        return *this;
    }

    Res& operator=(ErrType&& that) noexcept {
        if (!is_err) {
            res.~ResType();
            is_err = true;
            new(&err) ErrType(std::move(that));
        } else
            err = std::move(that);
        return *this;
    }

    // Move assignment. Useful in conjunction with default constructor to assign
    // an "uninitialized" (meaninglessly default-constructed) result.
    Res& operator=(Res&& that) noexcept {
        if (that.is_err)
            return *this = std::move(that.err);
        return *this = std::move(that.res);
    }

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
                return DerefType<ErrType>(err);
        }
        if constexpr (can_deref<ResType>)
            return DerefType<ResType>(*res);
        else
            return DerefType<ResType>(res);
    }

    // Need to explicitly call destructor of active union member.
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
