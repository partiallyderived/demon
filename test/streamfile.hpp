#pragma once

#include <sstream>

#include "dl/file.hpp"

struct StreamFile final: dl::File {
    std::stringstream stream;

    StreamFile() noexcept: stream() {}

    int getc() noexcept override {
        return stream.get();
    }

    int ungetc(int c) noexcept override {
        stream.unget();
        return c;
    }

    void clear() noexcept {
        stream.clear();
    }

    void ignore() noexcept {
        // Discard everything.
        stream.ignore(1000000);
    }

    void put(const char* str) {
        stream << str;
    }
};

