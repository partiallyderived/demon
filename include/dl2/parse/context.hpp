#pragma once

namespace dl {

// Contexts are used to to indicate that we are currently parsing an operation
// which has special conditions to indicate when we are finished
// parsing it.
enum class Context {
    BLOCK,
    CURVED,
    SQUARE,
    CONSTRUCT,
    CONSTRUCT_END
};

}
