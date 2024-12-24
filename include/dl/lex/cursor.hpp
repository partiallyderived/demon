#pragma once

#include <cstdint>
#include <cstdio>

#include <ostream>

#include "dl/err.hpp"
#include "dl/file.hpp"
#include "dl/pos.hpp"
#include "dl/res.hpp"

namespace dl {

struct UngetOverflowErr: Err {
    virtual std::ostream& out_name(std::ostream& os) const override {
        return os << "UngetOverflowErr";
    }
};

// Cursor handles the reading of a file in a way that tracks the column and line
// currently being read. It allows for ungetting up to two characters while
// correctly adjusting the line and column numbers.
struct Cursor {
    // Actual file being read
    File* file;

    // Line & column numbers for the next character to read
    Pos pos;

    // Tracks the value for `pos.col` in the previous line.
    // Used to "unget" a newline.
    std::uint32_t prevcol1;

    // Tracks the value for `pos.col` two lines ago, in case two newlines are
    // put back.
    std::uint32_t prevcol2;

    // prev1 is the last UTF-8 character read.
    std::int32_t prev1;

    // prev2 is the second-to-last UTF-8 character read.
    std::int32_t prev2;

    // The number of characters put back.
    // When set to 1, the next character is read from prev1.
    // When set to 2, the next character is read from prev2.
    int put_back;

    // Create a `Cursor` from a file to be read.
    constexpr Cursor(File* file) noexcept:
    file(file),
    pos(),
    prevcol1(1),
    prevcol2(1),
    prev1(-1),
    prev2(-1),
    put_back(0) {}

    // Helper function which doesn't include carriage return case.
    void inc_line() noexcept {
        prevcol2 = prevcol1;
        prevcol1 = pos.col;
        pos.line++;
        pos.col = 1;
    }

    std::int32_t advance(std::int32_t c) noexcept {
        if (c == '\n') {
            pos.line++;
            pos.col = 1;
        }
        else if (c != EOF)
            pos.col++;
        return c;
    }

    std::int32_t decode() {
        // https://en.wikipedia.org/wiki/UTF-8#Description
        std::int32_t code_point = 0;

        int c1 = file->getc();
        if (c1 <= 0x7F || c1 == EOF)
            // 1 Byte or EOF. Currently assumes EOF will never expand to a value
            // which is a valid UTF-8 start character.
            return std::int32_t(c1);

        // First, determine how many bytes we are decoding by the number of
        // leading 1's in the first byte.
        // 0xE0 == 0b1110000
        if ((c1 & 0xE0) == 0xC0) {
            // 2 Bytes
            // 0xC0 == 0b11000000
            int c2 = file->getc();
            if ((c2 & 0xC0) != 0x80) {
                file->ungetc(c2);
                // Replacement character to use when an error is encountered.
                // (see https://en.wikipedia.org/wiki/UTF-8#Error_handling)
                return 0xFFFD;
            }

            code_point |= (c1 & ~0xE0) << 6;
            code_point |= c2 & ~0xC0;

            if (code_point < 0x0080)
                // Overlong encoding
                return 0xFFFD;
        // 0xF0 == 0b11110000
        } else if ((c1 & 0xF0) == 0xE0) {
            // 3 Bytes
            int c2 = file->getc();
            if ((c2 & 0xC0) != 0x80) {
                file->ungetc(c2);
                return 0xFFFD;
            }

            int c3 = file->getc();
            if ((c3 & 0xC0) != 0x80) {
                file->ungetc(c3);
                return 0xFFFD;
            }

            // Minor portability concern: cast to std::int32_t since int only
            // guaranteed to be at least 16 bits.
            code_point |= std::int32_t(c1 & ~0xF0) << 12;
            code_point |= (c2 & ~0xC0) << 6;
            code_point |= c3 & ~0xC0;

            if (code_point < 0x0800)
                // Overlong encoding
                return 0xFFFD;
        // 0xF8 == 0b11111000
        } else if ((c1 & 0xF8) == 0xF0) {
            // 4 Bytes
            int c2 = file->getc();
            if ((c2 & 0xC0) != 0x80) {
                file->ungetc(c2);
                return 0xFFFD;
            }

            int c3 = file->getc();
            if ((c3 & 0xC0) != 0x80) {
                file->ungetc(c3);
                return 0xFFFD;
            }

            int c4 = file->getc();
            if ((c4 & 0xC0) != 0x80) {
                file->ungetc(c4);
                return 0xFFFD;
            }

            code_point |= std::int32_t(c1 & ~0xF8) << 18;
            code_point |= std::int32_t(c2 & ~0xC0) << 12;
            code_point |= (c3 & ~0xC0) << 6;
            code_point |= c4 & ~0xC0;

            if (code_point < 0x01000 || code_point > 0x10FFFF)
                // Overlong encoding or code point too large
                return 0xFFFD;
        } else
            return 0xFFFD;

        if (code_point >= 0xD800 && code_point <= 0xDFFF)
            // Surrogates
            return 0xFFFD;
        return code_point;
    }

    // Read a single Unicode code point and update `pos` accordingly.
    std::int32_t getc() noexcept {
        if (put_back == 2) {
            put_back--;
            return advance(prev2);
        }
        if (put_back == 1) {
            put_back--;
            return advance(prev1);
        }
        std::int32_t c = decode();
        if (c == '\r') {
            // Interpret carriage return as a newline character.
            // TODO: Am I handling this right?
            int next = file->getc();
            if (next == '\n') {
                inc_line();
                return std::int32_t('\n');
            }
            // No newline, just interpret as isolated '\r'.
            file->ungetc(next);
            pos.col++;
            return std::int32_t('\r');
        }
        prev2 = prev1;
        prev1 = c;
        if (c == '\n')
            // Note: advance prev1 or prev2 does not modify prevcol1 or
            // prevcol2 like getting a character from file does.
            inc_line();
        else
            pos.col++;
        return c;
    }

    // Unget a character. Errors if two characters have already been put back
    // without being read again.
    ErrPtr ungetc() {
        if (put_back >= 2)
            return ErrPtr(new UngetOverflowErr());
        if (put_back == 1) {
            if (prev2 == '\n') {
                pos.line--;
                if (prev1 == '\n')
                    pos.col = prevcol2;
                else
                    pos.col = prevcol1;
            } else
                pos.col--;
        } else {
            if (prev1 == '\n') {
                pos.line--;
                pos.col = prevcol1;
            } else
                pos.col--;
        }
        put_back++;
        return nullptr;
    }
};

std::ostream& operator<<(std::ostream& os, Cursor cursor) {
    return os << "Cursor(" << cursor.pos << ")";
}

}
