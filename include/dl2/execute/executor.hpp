#pragma once

#include "dl/err.hpp"
#include "dl/types.hpp"

namespace dl {

struct Executor {
    virtual ErrPtr execute(Typed node) = 0;
};

}
