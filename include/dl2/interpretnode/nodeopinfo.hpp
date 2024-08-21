#pragma once

#include "dl/parse/opid.hpp"

namespace dl {

struct NodeOpInfo {
	const char* dunder_name;
};

const NodeOpInfo& node_op_info(OpID op) noexcept {

}

}
