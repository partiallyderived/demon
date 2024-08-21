#pragma once

namespace dl {

enum class OpKind {
    NULLARY,
    UNARY,
    BINARY,
    SINGLETON,
    STRING,
    BLOCK,
    END,
    WAITING,
    DONE
};

}
