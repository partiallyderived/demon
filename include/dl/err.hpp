#pragma once

#include <cstddef>

#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "dl/pos.hpp"

namespace dl {

// Type of errors for this project.
struct Err {
    virtual bool equals(const Err& that) const noexcept {
        return true;
    }

    bool operator==(const Err& that) const noexcept {
        return typeid(*this) == typeid(that) && equals(that);
    }

    bool operator!=(const Err&) const noexcept = default;

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }

    virtual std::ostream& out_name(std::ostream& os) const {
        return os << "Err";
    }

    std::ostream& out(std::ostream& os) const {
        out_name(os);
        os << "(";
        out_data(os);
        return os << ")";
    }

    virtual ~Err() noexcept {}
};

std::ostream& operator<<(std::ostream& os, const Err& err) {
    return err.out(os);
}

using ErrPtr = std::unique_ptr<Err>;

// Indicate that an asserting failed.
struct AssertionFailedErr: Err {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "AssertionFailedErr";
    }
};

// Errors associated with line/column numbers.
struct LocatedErr: Err {
    Pos loc;

    constexpr LocatedErr(Pos loc) noexcept: loc(loc) {}

    virtual bool equals(const Err& that) const noexcept override {
        return loc == dynamic_cast<const LocatedErr&>(that).loc;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return os << loc;
    }

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "LocatedErr";
    }
};

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

template<typename LHSType, typename RHSType>
decltype(static_cast<void>(
    std::declval<LHSType>() == std::declval<RHSType>()
), std::true_type{}) can_eq_impl(int);

template<typename, typename>
std::false_type can_eq_impl(...);

template<typename LHSType, typename RHSType>
constexpr bool can_eq = decltype(can_eq_impl<LHSType, RHSType>(0))::value;


// Simple struct which defines a union between a non-error "result" and an      
// error.
template<typename ResType, typename ErrType>
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
