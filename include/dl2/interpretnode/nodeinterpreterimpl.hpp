#pragma once

#include "dl/err.hpp"
#include "dl/interpretnode/nodeopkind.hpp"
#include "dl/processor/node.hpp"

namespace dl {

struct NodeInterpreterImpl {
	ErrPtr interpret(Node&& node) {
		using enum NodeOpKind;
		switch(node_op_info(node.op).kind) {
		
		}
	}
};

}
