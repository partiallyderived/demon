#pragma once

namespace dl {

enum class Instruction {
	GET,
	SET,
	SYMBOL,
	OBJECT,
	STRUCT,
	TYPE,
	IMMEDIATE,
	EXPAND_SEQ,
	EXPAND_MAP,
	EXPAND_FIELDS,
	BRANCH,
	GOTO,
	RETURN,
	ENTER,
	EXIT,
	CALL,
	FRAME_SOURCE_FILE,
	STACKTRACE,
	SET_DEFAULT_EXCEPTION_HANDLER,
	MARK,
	END // Mark last instruction.
};

}
