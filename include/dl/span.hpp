#pragma once

#include <cstdint>

#include <algorithm>
#include <ostream>

#include "dl/pos.hpp"

namespace dl {

struct Span {
    std::uint32_t start_line;
    std::uint32_t start_col;
    std::uint32_t end_line;
    std::uint32_t end_col;

    constexpr Span(
        std::uint32_t start_line,
        std::uint32_t start_col,
        std::uint32_t end_line,
        std::uint32_t end_col
    ) noexcept:
    start_line(start_line),
    start_col(start_col),
    end_line(end_line),
    end_col(end_col) {}

    Span(Span start, Span end) noexcept:
    start_line(start.start_line),
    start_col(start.start_col),
    end_line(end.end_line),
    end_col(end.end_col) {}

    constexpr Span(Pos start, Pos end) noexcept:
    start_line(start.line),
    start_col(start.col),
    end_line(end.line),
    end_col(end.col) {}

    constexpr Span(Pos start, std::uint32_t len = 1) noexcept:
    start_line(start.line),
    start_col(start.col),
    end_line(start.line),
    end_col(start.col + len - 1) {}

    constexpr Span(
        std::uint32_t start_line, std::uint32_t start_col, std::uint32_t len
    ) noexcept:
    start_line(start_line),
    start_col(start_col),
    end_line(start_line),
    end_col(start_col + len - 1) {}

    constexpr Span(std::uint32_t line, std::uint32_t col) noexcept:
    Span(Pos(line, col)) {}

    Pos start() {
        return Pos(start_line, start_col);
    }

    Pos end() {
        return Pos(end_line, end_col);
    }

    static Span after(Span s) {
        return Span(s.end() + 1);
    }

    bool operator==(const Span&) const noexcept = default;
    bool operator!=(const Span&) const noexcept = default;
};

std::ostream& operator<<(std::ostream& os, const Span& s) {
    os << s.start_line << ":" << s.start_col;
    if (s.start_line != s.end_line || s.start_col != s.end_col)
        os << ':' << s.end_line << ":" << s.end_col;
    return os;
}

}
