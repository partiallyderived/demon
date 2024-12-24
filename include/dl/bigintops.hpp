#pragma once

#include <cstdint>
#include <cstring>

#include <algorithm>

#include "dl/types.hpp"

namespace dl::integer {

constexpr std::uint32_t ALL_ONES = ~static_cast<std::uint32_t>(0);

void ensure_cap(BigInt& x, std::uint32_t req) {
    if (x.cap >= req)
        return;
    x.cap = 2 * req + 1;
    auto new_mag = new std::uint32_t[x.cap];
    std::memcpy(new_mag, x.mag, x.len);
    delete[] x.mag;
    x.mag = new_mag;
}

void fix_len(BigInt& x, std::uint32_t upperbound) {
    x.len = upperbound;
    if (x.sign) {
        while(x.len && x.mag[x.len - 1] == ALL_ONES)
            x.len--;
    } else {
        while(x.len && !x.mag[x.len - 1])
            x.len--;
    }
}

BigInt copy(const BigInt& x, std::uint32_t cap) {
    BigInt res{x.sign, new std::uint32_t[cap], x.len, cap};
    std::memcpy(res.mag, x.mag, 4 * x.len);
    return res;
}

BigInt zero(std::uint32_t cap) {
    return BigInt{false, new std::uint32_t[cap], 0, cap};
}

void iadd(BigInt& x, const BigInt& y) {
    // At most, need `std::max(x.len, y.len) + 1` capacity.
    std::uint32_t maxlen = std::max(x.len, y.len);
    ensure_cap(x, maxlen + 1);
    std::uint32_t carry = 0;
    std::uint32_t i = 0;
    for (i = 0; i < std::min(x.len, y.len); i++) {
        x.mag[i] += y.mag[i] + carry;
        carry = x.mag[i] <= y.mag[i] && (y.mag[i] || carry);
    }
    if (x.len > y.len) {
        if (y.sign) {
            // If y is negative and carry, we are already done, since
            // `carry + ~1 == 0` and keeps carry.
            while (!carry && i < x.len) {
                carry = x.mag[i] != 0;
                x.mag[i++]--;
            }
        } else {
            // If y is positive and !carry, we are already done.
            while (carry && i < x.len) {
                carry = x.mag[i] == ALL_ONES;
                x.mag[i++]++;
            }
        }   
    } else {
        if (x.sign) {
            while (!carry && i < y.len) {
                carry = y.mag[i] != 0;
                x.mag[i] = y.mag[i] - 1;
                i++;
            }
        } else {
            while (carry && i < y.len) {
                carry = y.mag[i] == ALL_ONES;
                x.mag[i] = y.mag[i] + 1;
                i++;
            }
        }
        // Copy remaining u32s from y to x.
        std::memcpy(x.mag + i, y.mag + i, y.len - i);
    }
    if (x.sign != y.sign) {
        x.sign = !carry;
        fix_len(x, maxlen);
    } else if (x.sign != carry) {
        // !carry -> ALL_ONES - 1, carry -> 1.
        x.mag[maxlen] = (ALL_ONES - 1) + 3 * carry;
        x.len = maxlen + 1;
    }
    
}

BigInt add(const BigInt& x, const BigInt& y) {
    BigInt res = copy(x, std::max(x.len, y.len) + 1);
    iadd(res, y);
    return res;
}

void isub(BigInt& x, const BigInt& y) {
    std::uint32_t maxlen = std::max(x.len, y.len);
    ensure_cap(x, maxlen + 1);
    std::uint32_t borrow = 0;
    std::uint32_t i = 0;
    for (i = 0; i < newlen; i++) {
        x.mag[i] -= y.mag[i] + borrow;
        borrow = x.mag[i] >= y.mag[i] && (y.mag[i] || borrow);
    }
    if (x.len > y.len) {
        if (y.sign) {
            while (!borrow && i < y.len) {
                borrow = x.mag[i] != ALL_ONES;
                x.mag[i]++;
                i++;
            }
        } else {
            while (borrow && i < y.len) {
                borrow = x.mag[i] == 0;
                x.mag[i]--;
                i++;
            }
        }
    } else {
        if (x.sign) {
            while (!borrow && i < y.len) {
                borrow = y.mag[i] != ALL_ONES;
                x.mag[i] = y.mag[i] + 1;
                i++;
            }
        } else {
            while (borrow && i < y.len) {
                borrow = y.mag[i] == 0;
                x.mag[i] = y.mag[i] - 1;
                i++;
            }
        }
        std::memcpy(x.mag + i, y.mag + i, y.len - i);
    }
    if (x.sign == y.sign) {
        x.sign = borrow;
        fix_len(x, maxlen);
    } else if (x.sign == borrow) {
        // !borrow -> 1, borrow -> ALL_ONES - 1
        x.mag[maxlen] = 1 - 3 * borrow;
        x.len = maxlen + 1;
    }
}

BigInt sub(const BigInt& x, const BigInt& y) {
    BigInt res = copy(x, std::max(x.len, y.len) + 1);
    isub(res, y);
    return res;
}

BigInt mul(const BigInt& x, const BigInt& y) {
    std::uint32_t maxlen = x.len + y.len;
    BigInt res = zero(maxlen);
}

}
