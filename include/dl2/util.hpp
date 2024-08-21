#pragma once

#include <cstdint>
#include <cstring>

#include "dl/types.hpp"

namespace dl {

// Unsigned left shift in place. 
Int& iulsh(Int& x, unsigned by) {
    unsigned full_shifts = by / 32;
    unsigned remaining = by % 32;
    while(by) {

    }
    return x;
}

bool base8_fits_in_int32(Word word) {
    return word.len < 11 || (word.len == 11 && word[0] <= '3');
}

bool base10_fits_in_int32(Word word) {
    return 
        word.len < 10 ||
        (word.len == 10 && memcmp(word, "2147483647", 10) <= 0);
}

bool base16_fits_in_int32(Word word) {
    return word.len <= 8;
}

Typed base8_to_int(Word word) {

}

Typed base10_to_int(Word word) {

}

Typed base16_to_int(Word word) {

}

}
