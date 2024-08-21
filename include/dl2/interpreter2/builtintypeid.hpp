#pragma once

#include <cstdint>

namespace dl {

enum class BuiltinTypeID {
    ERROR_SIGNAL,
    NONE_TYPE,
    ADDRESS,
    FN_PTR,
    DUNDER_BOOL,
    DUNDER_UINT8,
    DUNDER_UINT16,
    DUNDER_UINT32,
    DUNDER_UINT64,
    DUNDER_INT8,
    DUNDER_INT16,
    DUNDER_INT32,
    DUNDER_INT64,
    DUNDER_FLOAT32,
    DUNDER_FLOAT64,
    SYMBOL,
    ANY,
    DEF,
    SEQ,
    VARS,
    STRUCT,
    TIDS,
    TYPE,
    PTR_TYPE,
    BOOL,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64
};

constexpr std::uint32_t BUILTIN_SIZES[] = {
    0, // NONE_TYPE
    sizeof(void*), // ADDRESS
    sizeof(void*), // FN_PTR
    1, // BOOL
    1, // INT8
    2, // INT16
    4, // INT32
    8, // INT64
    1, // UINT8
    2, // UINT16
    4, // UINT32
    8, // UINT64
    4, // FLOAT32
    8, // FLOAT64
    4, // SYMBOL
    8, // QUALIFIED
    8 + sizeof(void*), // ANY
    4 + sizeof(void*), // SEQ
    4 + 3 * sizeof(void*), // FIXED_VARS
    8 + 4 + 3 * sizeof(void*), // STRUCT
    4 + sizeof(void*), // TYPE_SEQ
    5 * (4 + 3 * sizeof(void*)) + (4 + sizeof(void*)) + 4 + 4 // TYPE
}

// Gives the TID for a builtin type.
template<typename T>
constexpr BuiltinTypeID tid_for = BuiltinTypeID::ERROR_SIGNAL;

template<>
constexpr BuiltinTypeID tid_for<bool> = BuiltinTypeID::BOOL;

template<>
constexpr BuiltinTypeID tid_for<std::int8_t> = BuiltinTypeID::INT8;

template<>
constexpr BuiltinTypeID tid_for<std::int16_t> = BuiltinTypeID::INT16;

template<>
constexpr BuiltinTypeID tid_for<std::int32_t> = BuiltinTypeID::INT32;

template<>
constexpr BuiltinTypeID tid_for<std::int64_t> = BuiltinTypeID::INT64;

template<>
constexpr BuiltinTypeID tid_for<std::uint8_t> = BuiltinTypeID::UINT8;

template<>
constexpr BuiltinTypeID tid_for<std::uint16_t> = BuiltinTypeID::UINT16;

template<>
constexpr BuiltinTypeID tid_for<std::uint32_t> = BuiltinTypeID::UINT32;

template<>
constexpr BuiltinTypeID tid_for<std::uint64_t> = BuiltinTypeID::UINT64;

template<>
constexpr BuiltinTypeID tid_for<float> = BuiltinTypeID::FLOAT32;

template<>
constexpr BuiltinTypeID tid_for<double> = BuiltinTypeID::FLOAT64;

template<>
constexpr BuiltinTypeID tid_for<Def> = BuiltinTypeID::DEF;

template<>
constexpr BuiltinTypeID tid_for<Any> = BuiltinTypeID::ANY;

template<>
constexpr BuiltinTypeID tid_for<Seq> = BuiltinTypeID::SEQ;

template<>
constexpr BuiltinTypeID tid_for<Struct> = BuiltinTypeID::STRUCT;

template<>
constexpr BuiltinTypeID tid_for<Type> = BuiltinTypeID::TYPE;

}
