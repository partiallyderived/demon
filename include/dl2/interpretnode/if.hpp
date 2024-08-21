#pragma once

namespace dl {

struct If {
    Any predicate;
    Any body;
    If* orelse;
};

}
