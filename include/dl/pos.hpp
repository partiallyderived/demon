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
};

std::ostream& operator<<(std::ostream& os, const Pos& pos) {
    return os << "Pos(" << pos.line << ", " << pos.col << ")";
}

}
