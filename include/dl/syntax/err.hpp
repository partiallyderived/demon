#pragma once

#include <ostream>

#include "dl/err.hpp"

namespace dl::syntax {

struct SyntaxErr: LocatedErr {
    using LocatedErr::LocatedErr;

    std::ostream& out_name(std::ostream& os) const override {
        return os << "SyntaxErr";
    }
};

}