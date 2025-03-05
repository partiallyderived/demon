#pragma once

#include <cerrno>
#include <cstdlib>

#include <limits>
#include <type_traits>

#include "dl/res.hpp"

namespace dl {

struct InvalidNumericLiteralErr final: SourcedErr {
    InvalidNumericLiteralErr(Pos src) noexcept: SourcedErr(src) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "InvalidNumericLiteralErr";
    }
};

struct OutOfRangeErr final: SourcedErr {
    OutOfRangeErr(Pos src) noexcept: SourcedErr(src) {}

    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "OutOfRangeErr";
    }
};

template<typename T>
T strto(const char* str, char** str_end, int base = 10) {
    if constexpr (std::is_same_v<T, float>)
        return std::strtof(str, str_end);
    if constexpr (std::is_same_v<T, double>)
        return std::strtod(str, str_end);
    if constexpr (std::is_same_v<T, long double>)
        return std::strtold(str, str_end);
    if constexpr (std::is_same_v<T, long long>)
        return std::strtoll(str, str_end, base);
    if constexpr (std::is_same_v<T, long>)
        return std::strtol(str, str_end, base);
    if constexpr (std::is_same_v<T, unsigned long long>)
        return std::strtoull(str, str_end, base);
    if constexpr (std::is_same_v<T, unsigned long>)
        return std::strtoul(str, str_end, base);
    if constexpr (std::is_signed_v<T>) {
        long res = strtol(str, str_end, base);
        if (res < static_cast<long>(std::numeric_limits<T>::min())) {
            errno = ERANGE;
            return std::numeric_limits<T>::min();
        }
        if (res > static_cast<long>(std::numeric_limits<T>::max())) {
            errno = ERANGE;
            return std::numeric_limits<T>::max();
        }
        return static_cast<T>(res);
    } else {
        unsigned long res = strtoul(str, str_end, base);
        if (res > static_cast<unsigned long>(std::numeric_limits<T>::max())) {
            errno = ERANGE;
            return std::numeric_limits<T>::max();
        }
        return static_cast<T>(res);
    }
}

template<typename T>
Res<T> read_number(
    Pos start,
    const char* begin,
    const char* expected_end,
    int base
) noexcept {
    char* end;
    errno = 0;
    T res = strto<T>(begin, &end, base);
    if (errno == ERANGE)
        return ErrPtr(new OutOfRangeErr(start));
    if (end < expected_end)
        return ErrPtr(new InvalidNumericLiteralErr(start));
    return res;
}

}
