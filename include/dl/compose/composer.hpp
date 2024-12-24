#pragma once

#include "dl/compose/comp.hpp"
#include "dl/parse/op.hpp"

namespace dl {

struct Composer {
    // Feed an operation to the processor.
    virtual void feed(Op&& op) = 0;

    // Get the next executable node.
    virtual Comp next() = 0;
    
    virtual ~Composer() noexcept {}
};

}
