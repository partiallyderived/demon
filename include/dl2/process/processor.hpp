#pragma once

#include <cstdint>

#include <string>

#include "dl/parse/op.hpp"
#include "dl/process/node.hpp"

namespace dl {

struct Processor {
    // Feed an operation to the processor.
    virtual void feed(Op op) = 0;

    // Get the next executable node.
    virtual Node next() = 0;
    
    virtual ~Processor() noexcept {}
};

}
