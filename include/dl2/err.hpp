#pragma once

#include <cstring>

#include <memory>
#include <ostream>
#include <typeinfo>

namespace dl {

// Type of errors for this project.
struct Err {
    virtual bool equals(const Err& that) const noexcept {
        return true;
    }

    bool operator==(const Err& that) const noexcept {
        return typeid(*this) == typeid(that) && equals(that);
    }

    bool operator!=(const Err&) const noexcept = default;

    virtual std::ostream& out_data(std::ostream& os) const {
        return os;
    }

    virtual std::ostream& out_name(std::ostream& os) const {
        return os << "Err";
    }

    std::ostream& out(std::ostream& os) const {
        out_name(os);
        os << "(";
        out_data(os);
        return os << ")";
    }

    virtual ~Err() noexcept {}
};

std::ostream& operator<<(std::ostream& os, const Err& err) {
    return err.out(os);
}

using ErrPtr = std::unique_ptr<Err>;

// Indicate that an assertion failed.
struct AssertionFailedErr: Err {
    const char* what;

    AssertionFailedErr(const char* what) noexcept: what(what) {}

    bool equals(const Err& that) const noexcept override {
        return !std::strcmp(
            what, dynamic_cast<const AssertionFailedErr&>(that).what
        );
    }

    std::ostream& out_data(std::ostream& os) const override {
        return os << what;
    }

    std::ostream& out_name(std::ostream& os) const override {
        return os << "AssertionFailedErr";
    }
};

}
