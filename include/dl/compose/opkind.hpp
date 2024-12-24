#pragma once

namespace dl {

enum class OpKind {
    NULLARY,
    UNARY,
    BINARY,
    SINGLETON,
    DATA,
    AGGREGATE,
    END,
    WAITING,
    DONE
};

}
