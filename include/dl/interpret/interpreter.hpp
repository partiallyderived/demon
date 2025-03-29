#pragma once

#include "dl/compose/comp.hpp"
#include "dl/interpret/node.hpp"

namespace dl {

struct Interpreter {
    virtual NodeRes interpret(Comp&& comp)=0;
};

}
