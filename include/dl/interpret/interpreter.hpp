#pragma once

namespace dl {

struct Interpreter {
    virtual NodeRes interpret(Comp&& comp)=0;
};

}
