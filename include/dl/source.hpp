#pragma once

#include <cstdint>
#include <cstring>

#include <ostream>

namespace dl {

struct Source {
    const char* file;
    std::uint32_t line;
    std::uint32_t col;

    Source(const char* file, std::uint32_t line, std::uint32_t col) noexcept:
    file(file), line(line), col(col) {}

    Source() noexcept: Source("", 1, 1) {}


    bool operator==(const Source& that) const {
        return
            strcmp(file, that.file) == 0 &&
            line == that.line &&
            col == that.col;
    }
};

std::ostream& operator<<(std::ostream& os, const Source& s) {
    return os << "Source(" << s.file << ", " << s.line << ", " << s.col << ')';
}

}
