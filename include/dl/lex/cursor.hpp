#pragma once

#include <cstdint>
#include <cstdio>

#include <ostream>

#include "dl/file.hpp"
#include "dl/pos.hpp"

namespace dl::lex {

// Cursor handles the reading of a file in a way that tracks the column and line
// currently being read. It allows for ungetting a single character in a way
// that correctly adjusts the line and column numbers.
struct Cursor {
    // Actual file being read
    File* file;

    // Line & column number of the next character to read
    Pos pos;

    // Tracks the value for `pos.col` in the previous line.
    // Used to "unget" a newline.
    std::uint32_t prevcol;

    // Create a `Cursor` from a file to be read.
    constexpr Cursor(File* file) noexcept: file(file), pos(), prevcol(1) {}

    // Helper function which doesn't include carriage return case.
    void inc_line() noexcept {
        prevcol = pos.col;
        pos.line++;
        pos.col = 1;
    }

    // Read a single character and update `pos` accordingly.
    int getc() noexcept {
        int c = file->getc();
        if (c == '\r') {
            // Interpret carriage return as a newline character.
            // TODO: Am I handling this right?
            int next = file->getc();
            if (next == '\n') {
                inc_line();
                return '\n';
            }
            // No newline, just interpret as isolated '\r'.
            file->ungetc(next);
            pos.col++;
            return '\r';
        }
        if (c == '\n')
            inc_line();
        else if (c != EOF)
            pos.col++;
        return c;
    }

    // Look at the next character without getting it.
    int peek() noexcept {
        int c = file->getc();
        file->ungetc(c);
        return c;
    }

    // Unget a single character, changing the position as appropriate.
    int ungetc(int c) noexcept {
        // Need to handle newline specially in both directions.
        if (c == '\n') {
            pos.col = prevcol;
            pos.line--;
        } else if (c != EOF)
            pos.col--;
        return file->ungetc(c);
    }
};

std::ostream& operator<<(std::ostream& os, Cursor cursor) {
    return os << "Cursor(" << cursor.pos << ", " << cursor.prevcol << ")";
}

}
