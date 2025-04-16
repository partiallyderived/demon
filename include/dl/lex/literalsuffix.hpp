#pragma once

#include <cstdint>

#include <string>

namespace dl {

enum class LiteralSuffix {
    NONE,
    F,
    F32,
    F64,
    S,
    S8,
    S16,
    S32,
    S64,
    U,
    U8,
    U16,
    U32,
    U64,
    R,
    RF,
    RF32,
    RF64,
    RS,
    RS8,
    RS16,
    RS32,
    RS64,
    RU,
    RU8,
    RU16,
    RU32,
    RU64
};

int lit_suffix_len(LiteralSuffix suffix) {
    switch (suffix) {
    case LiteralSuffix::NONE:
        return 0;
    case LiteralSuffix::F:
    case LiteralSuffix::R:
    case LiteralSuffix::S:
    case LiteralSuffix::U:
        return 1;
    case LiteralSuffix::RF:
    case LiteralSuffix::RS:
    case LiteralSuffix::RU:
    case LiteralSuffix::S8:
    case LiteralSuffix::U8:
        return 2;
    case LiteralSuffix::RF32:
    case LiteralSuffix::RF64:
    case LiteralSuffix::RS16:
    case LiteralSuffix::RS32:
    case LiteralSuffix::RS64:
    case LiteralSuffix::RU16:
    case LiteralSuffix::RU32:
    case LiteralSuffix::RU64:
        return 4;
    default:
        return 3;
    }
}

LiteralSuffix lit_suffix0(const std::string& alnum) {
    switch(alnum.back()) {
    case 'f':
    case 'F':
        return LiteralSuffix::F;
    case 's':
    case 'S':
        return LiteralSuffix::S;
    case 'u':
    case 'U':
        return LiteralSuffix::U;
    case '8':
        if (alnum.size() >= 2) {
            switch(alnum[alnum.size() - 2]) {
            case 's':
            case 'S':
                return LiteralSuffix::S8;
            case 'u':
            case 'U':
                return LiteralSuffix::U8;
            }
        }
        break;
    case '6':
        if (alnum.size() >= 3 && alnum[alnum.size() - 2] == '1') {
            switch(alnum[alnum.size() - 3]) {
            case 's':
            case 'S':
                return LiteralSuffix::S16;
            case 'u':
            case 'U':
                return LiteralSuffix::U16;
            }
        }
        break;
    case '2':
        if (alnum.size() >= 3 && alnum[alnum.size() - 2] == '3') {
            switch(alnum[alnum.size() - 3]) {
            case 'f':
            case 'F':
                return LiteralSuffix::F32;
            case 's':
            case 'S':
                return LiteralSuffix::S32;
            case 'u':
            case 'U':
                return LiteralSuffix::U32;
            }
        }
        break;
    case '4':
        if (alnum.size() >= 3 && alnum[alnum.size() - 2] == '6') {
            switch(alnum[alnum.size() - 3]) {
            case 'f':
            case 'F':
                return LiteralSuffix::F64;
            case 's':
            case 'S':
                return LiteralSuffix::S64;
            case 'u':
            case 'U':
                return LiteralSuffix::U64;
            }
        }
        break;
    }
    return LiteralSuffix::NONE;
}

LiteralSuffix lit_suffix(const std::string& alnum) {
    LiteralSuffix ls = lit_suffix0(alnum);
    int len = lit_suffix_len(ls);
    if (alnum.size() > len) {
        char c = alnum[alnum.size() - len - 1];
        if (c == 'r' || c == 'R')
            // Can do this because all the raw suffixes are at the end.
            return LiteralSuffix(int(ls) + int(LiteralSuffix::R));
    }
    return ls;
}

}
