#pragma once

#include <cstdint>

#include <ostream>

namespace dl {

// Data representing a line and a column number to specify a position within a
// file.
struct Pos {
    std::uint32_t line;
    std::uint32_t col;

    constexpr Pos() noexcept: line(1), col(1) {}

    constexpr Pos(std::uint32_t line, std::uint32_t col) noexcept:
    line(line), col(col) {}

    bool operator==(const Pos&) const noexcept = default;
    bool operator!=(const Pos&) const noexcept = default;

    Pos operator+(std::uint32_t n) const noexcept {
        return Pos(line, col + n);
    }

    Pos& operator+=(std::uint32_t n) noexcept {
        col += n;
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
    return os << pos.line << ":" << pos.col;
}

}
