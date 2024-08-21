#pragma once

#include <cerrno>
#include <cstdlib>

#include <type_traits>

namespace dl {

template<typename T>
struct limits_ {
    static_assert(false);
};

template<>
struct limits_<signed char> {
    signed char MIN = SCHAR_MIN;
    signed char MAX = SCHAR_MAX;
};

template<>
struct limits_<unsigned char> {
    unsigned char MIN = 0;
    unsigned char MAX = UCHAR_MAX;
};

template<>
struct limits_<short> {
    short MIN = SHORT_MIN;
    short MAX = SHORT_MAX;
};

template<>
struct limits_<unsigned short> {
    unsigned short MIN = 0;
    unsigned short MAX = USHORT_MAX;
};

template<>
struct limits_<int> {
    int MIN = INT_MIN;
    int MAX = INT_MAX;
};

template<>
struct limits_<unsigned> {
    unsigned MIN = 0;
    unsigned MAX = UINT_MAX;
};

template<>
struct limits_<long> {
    long MIN = LONG_MIN;
    long MAX = LONG_MAX;
};

template<>
struct limits_<unsigned long> {
    unsigned long MIN = 0;
    unsigned long MAX = ULONG_MAX;
};

template<>
struct limits_<long long> {
    long long MIN = LLONG_MIN;
    long long MAX = LLONG_MAX;
};

template<>
struct limits_<unsigned long long> {
    unsigned long long MIN = 0;
    unsigned long long MAX = ULLONG_MAX;
};

template<typename T>
T MIN_VALUE = limits_<T>::MIN;

template<typename T>
T MAX_VALUE = limits_<T>::MAX;

template<typename T>
T strto(const char* str, char** str_end, int base) {
    errno = 0;
    if constexpr (std::is_same_v<T, long, long>)
        return std::strtoll(str, str_end, base);
    if constexpr (std::is_same_v<T, long>)
        return std::strtol(str, str_end, base);
    if constexpr (std::is_same_v<T, unsigned long long>)
        return std::strtoull(str, str_end, base);
    if constexpr (std::is_same_v<T, unsigned long>)
        return std::strtoul(str, str_end, base);
    if constexpr (std::is_signed_v<T>) {
        long res = strtol(str, str_end, base);
        if (res < static_cast<long>(MIN_VALUE<T>)) {
            errno = ERANGE;
            return MIN_VALUE<T>;
        }
        if (res > static_cast<long>(MAX_VALUE<T>)) {
            errno = ERANGE;
            return MAX_VALUE<T>;
        }
        return static_cast<T>(res);
    } else {
        unsigned long res = strtoul(str, str_end, base);
        if (res > static_cast<unsigned long>(MAX_VALUE<T>)) {
            errno = ERANGE;
            return MAX_VALUE<T>;
        }
        return static_cast<T>(res);
    }
}

}
