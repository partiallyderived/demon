#pragma once

#include "dl/err.hpp"
#include "dl/lex/token.hpp"
#include "dl/res.hpp"


namespace dl {

using TokenRes = Res<Token, ErrPtr>;

}
