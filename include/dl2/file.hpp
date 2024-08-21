#pragma once

#include <cstdio>

namespace dl {

struct File {
    virtual int getc() noexcept = 0;
    virtual int ungetc(int c) noexcept = 0;
};

// Wrapper around a C file. Used to provide a more flexible interface.
// Especially useful for testing.
struct CFile final: File {
    FILE* file;

    constexpr CFile(FILE* file) noexcept: file(file) {}

    int getc() noexcept override {
        return std::fgetc(file);
    }

    int ungetc(int c) noexcept override {
        return std::ungetc(c, file);
    }
};

}
