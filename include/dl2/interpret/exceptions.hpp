#pragma once

namespace dl {

struct BadInstructionErr {
	char instruction;
};

struct CallBoundaryErr {
	std::uint32_t outer;
	std::uint32_t symbol;
};

struct FrameOverflowErr {
	std::uint32_t limit;
};

struct ImmediateOverflowErr {
	std::uint32_t limit;
};

struct NoSuchSymbolErr {
	Word symbol;
};

struct OuterOOBErr {
	std::uint32_t depth;
	std::uint32_t outer;
	Word symbol;
};

struct SourceOverflowErr {
	std::uint32_t limit;
};

struct SymbolOverflowErr {
	std::uint32_t limit;
};

struct SymbolStackOverflowErr {
	std::uint32_t limit;
};

struct VarOverflowErr {
	std::uint32_t limit;
}

struct VarStackOverflowErr {
	std::uint32_t limit;
};

struct ValuesStackOverflowErr {
	std::uint32_t limit;
};

}
