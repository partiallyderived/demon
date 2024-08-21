#pragma once

#include "dl/interpretnode/instruction.hpp"
#include "dl/pos.hpp"

namespace dl {

struct InterpretedNode {
	Instruction instruction;
	void* data;
	Pos pos;
};

}
