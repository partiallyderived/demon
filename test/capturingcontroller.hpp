#pragma once

#include <utility>
#include <vector>

#include "dl/compose/comp.hpp"
#include "dl/compose/composerimpl.hpp"
#include "dl/err.hpp"
#include "dl/file.hpp"
#include "dl/interpret/interpreterimpl.hpp"
#include "dl/interpret/node.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/lex/lex.hpp"
#include "dl/lex/token.hpp"
#include "dl/parse/op.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/parserimpl.hpp"
#include "dl/res.hpp"
#include "dl/span.hpp"
#include "streamfile.hpp"

namespace dl {

struct CapturingController {
    Cursor cursor;
    std::vector<std::pair<Token, Span>> tokens_;
    std::vector<Op> ops_;
    std::vector<Comp> comps_;
    Nodes nodes_;

    std::vector<ErrPtr> lex_errs;
    std::vector<ErrPtr> parse_errs;

    CapturingController(File* file) noexcept: 
    cursor(file) {}

    void capture(const char* str) {
        auto& f = dynamic_cast<StreamFile&>(*cursor.file);
        f.put(str);
        while(true) {  
            Pos start = cursor.pos;
            Res<Token> t = next_token(cursor);
            if (t.is_err) {
                lex_errs.push_back(std::move(t.err));
                continue;
            }
            Pos end;
            if (t.res.id == TokenID::END_OF_FILE)
                end = cursor.pos;
            else if (cursor.pos.col != 1)
                end = Pos(cursor.pos.line, cursor.pos.col - 1);
            else {
                cursor.ungetc();
                end = cursor.pos;
                cursor.getc();
            }
            auto src = Span(start, end);
            tokens_.push_back({std::move(t.res), src});
            if (t.res.id == TokenID::END_OF_FILE)
                break;
        }

        auto parser = ParserImpl();
        for (std::pair<Token, Span>& t: tokens_)
            parser.feed(t.first.copy(), t.second);

        auto composer = ComposerImpl();
        while (true) {
            Res<Op> o = parser.next();
            if (o.is_err) {
                parse_errs.push_back(std::move(o.err));
                continue;
            }
            if (o.res.id == OpID::WAITING)
                break;
            ops_.push_back(o.res.copy());
            composer.feed(std::move(o.res));
        }

        auto interpreter = InterpreterImpl();
        while (true) {
            Comp c = composer.next();
            if (c.op == OpID::WAITING)
                break;
            comps_.push_back(c.copy());
            nodes_.push_back(interpreter.interpret(std::move(c)));
        }
    }

    std::vector<std::pair<Token, Span>> tokens() {
        return std::move(tokens_);
    }

    std::vector<Op> ops() {
        return std::move(ops_);
    }

    std::vector<Comp> comps() {
        return std::move(comps_);
    }

    Nodes nodes() {
        return std::move(nodes_);
    }

    Res<Comp> comp() {
        if (comps_.size() != 1)
            return ErrPtr(new AssertionFailedErr("# Comps is not exactly 1"));
        return comps_[0].copy();
    }

    Res<NodePtr> node() {
        if (nodes_.size() != 1)
            return ErrPtr(new AssertionFailedErr("# Nodes is not exactly 1"));
        return nodes_[0]->copy_ptr();
    }
};

}
