#pragma once

#include <cassert>
#include <cstdint>

#include <istream>

#include "dl/interpret/instruction.hpp"
#include "dl/interpret/runresult.hpp"
#include "dl/interpret/types.hpp"

namespace dl {

struct InterpreterImpl {
	State state;
	std::istream& in;

	static Typed NOTHING;

	static std::uint32_t hashsym(Symbol sym) {
		// TODO
	}

	static std::uint32_t hashword(Word word) {
		// TODO
	}

	static bool wordequals(const Word& word1, const Word& word2) {
		if (word1.len != word2.len)
			return false;
		return memcmp(word1, word2, word1.len) == 0;
	}

	static Typed getvar(Symbol sym, const Vars& vars, std::uint32_t cap) {
		std::uint32_t idx = hashsym(sym) % cap;
		Symbol found = vars.syms[idx];
		while (found != sym) {
			if (found.sym == static_cast<Symbol>(BuiltinSymbols::NO_SYMBOL))
				return NOTHING;
			idx = (idx + 1) % cap;
			found = vars.syms[idx];
		}
		return vars.data[idx];
	}

	static bool setvar(Symbol sym, Typed value, Vars& vars, std::uint32_t cap) {
		std::uint32_t idx = hashword(word) % cap;
		Symbol found = vars.syms[idx];
		while (true) {
			if (found == static_cast<Symbol>(BuiltinSymbols::NO_SYMBOL)) {
				if (vars.len == cap)
					return true;
				vars.idxs[vars.len++] = idx;
				vars.data[idx] = value;
				return false;
			}

			if (found == sym) {
				// TODO: Pure data assignment, copy, reference, or move design.
				vars.data[idx] = value;
				return false;
			}
			idx = (idx + 1) % cap;
		}
	}

	InterpreterImpl(Config config, std::istream& in): in(in) {
		state.config = config;
	}

	bool raise(Typed exception) {

	}

	bool validate_qualified(Symbol sym, std::uint32_t outer) {
		// Call to make sure a qualified variable is within appropriate bounds.
		if (outer > stack.vars.depth) {
			// Trying to access a variable outside the outermost scope.
			raise(
				call(static_cast<Symbol>(BuiltinSymbol::CALL_BOUNDARY_ERR))
				get_global())
			return false;
		}
		if (outer > stack.vars.last_callee) {
			// Trying to access a variable from outside a function call.
			state.frame_idx = state.handlers.outer_escapes_call;
			return false;
		}
		return true;
	}

	void push_value(Typed value) {
		if (state.stack.values.depth == state.config.max_values_depth) {
			state.frame_idx = state.handlers.values_stack_overflow;
			return true;
		}
		state.stack.values.values[state.stack.values.depth++] = value;
		return false;
	}

	Typed get_qualified(Symbol sym, std::uint32_t outer) {
		// Outer of 1 means current scope, so decrement to 0-index.
		outer--;
		if (validate_qualified(sym, outer))
			return NOTHING;
		Typed* value = getvar(
			sym,
			state.stack.vars.vars[stack.vars.depth - outer],
			state.config.max_vars
		);
		if (value.type == nullptr) {
			state.frame_idx = state.handlers.no_such_symbol;
			return NOTHING;
		}
	}

	bool push_qualified(Symbol sym, std::uint32_t outer) {
		return push_value(get_qualified(sym, outer));
	}

	bool push_unqualified(Symbol sym) {

	}

	void push_var_value(Symbol sym) {
		Symbol base = sym & ((1 << 26) - 1);
		std::uint32_t outer = (sym >> 26 & ((1 << 6) - 1));
		if (outer)
			get_qualified(sym, outer);
		else
			get_unqualified(sym);
	}

	void set(Symbol sym) {

	}

	void push_symbol(Symbol sym) {
		if (state.stack.syms.depth == state.config.max_symbol_depth) {
			state.frame_idx = state.handlers.symbol_stack_overflow;
			return;
		}
		state.stack.syms.syms[state.stack.syms.depth++] = sym;
	}

	void execute(ExecutionFrame frame) {
		using enum Instruction;
		if (frame.instruction >= static_cast<char>(END)) {
			state.frame_idx = state.handlers.bad_instruction;
			return;
		}

		switch(static_cast<Instruction>(frame.instruction)) {
		case GET:
			get(frame.arg);
			return;
		case SET:
			set(frame.arg);
			return;
		case SYMBOL:
			push_symbol(frame.arg);
			return;
		case OBJECT:
		case STRUCT:
		case TYPE:
		case IMMEDIATE:
		case EXPAND_SEQ:
		case EXPAND_MAP:
		case EXPAND_FIELDS:
		case BRANCH:
		case GOTO:
			state.frame_idx = frame.arg;
			return;
		case RETURN:
		case ENTER:
		case EXIT:
		case CALL:
		case MARK:
		case END:
			assert(false);
		}
	}
};

Typed InterpreterImpl::NOTHING = Typed{Qualified{0, nullptr}, nullptr};

}
