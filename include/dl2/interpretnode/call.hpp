#pragma once

#include "dl/interpretnode/interpretednode.hpp"
#include "dl/interpret/types.hpp"

namespace dl {

struct Call {
	Source origin;

	Any callee;

	std::uint32_t nargs;
	Any* args;
	bool* pos_unpacked;

	std::uint32_t nkwargs;
	Symbol* syms;
	Any* kwargs;
	bool* kw_unpacked;
};

}
