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

enum class Stage {
    LEX,
    PARSE,
    INTERPRET,
    NONE
};

struct CapturingController {
    Cursor cursor;
    std::vector<std::pair<Token, Span>> tokens_;
    std::vector<Op> ops_;
    std::vector<Comp> comps_;
    Nodes nodes_;

    ErrPtr err;
    Stage err_stage;

    CapturingController(File* file) noexcept: 
    cursor(file), err_stage(Stage::NONE) {}

    void capture(const char* str) {
        auto& f = dynamic_cast<StreamFile&>(*cursor.file);
        f.put(str);
        while(true) {  
            Pos start = cursor.pos;
            Res<Token> t = next_token(cursor);
            if (t.is_err) {
                err = std::move(t.err);
                err_stage = Stage::LEX;
                return;
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
        for (std::pair<Token, Span>& t: tokens_) {
            ErrPtr e = parser.feed(t.first.copy(), t.second);
            if (e) {
                err = std::move(e);
                err_stage = Stage::PARSE;
                return;
            }
        }

        auto composer = ComposerImpl();
        while (true) {
            Op o = parser.next();
            if (o.id == OpID::WAITING)
                break;
            ops_.push_back(o.copy());
            composer.feed(std::move(o));
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

    Res<std::vector<std::pair<Token, Span>>> tokens() {
        if (err_stage == Stage::LEX)
            return std::move(err);
        return std::move(tokens_);
    }

    Res<std::vector<Op>> ops() {
        if (err_stage <= Stage::PARSE)
            return std::move(err);
        return std::move(ops_);
    }

    Res<std::vector<Comp>> comps() {
        if (err_stage <= Stage::PARSE)
            return std::move(err);
        return std::move(comps_);
    }

    Res<Nodes> nodes() {
        if (err_stage <= Stage::INTERPRET)
            return std::move(err);
        return std::move(nodes_);
    }

    Res<Comp> comp() {
        if (err_stage <= Stage::PARSE)
            return std::move(err);
        if (comps_.size() != 1)
            return ErrPtr(new AssertionFailedErr("# Comps is not exactly 1"));
        return comps_[0].copy();
    }

    Res<NodePtr> node() {
        if (err_stage <= Stage::INTERPRET)
            return std::move(err);
        if (nodes_.size() != 1)
            return ErrPtr(new AssertionFailedErr("# Nodes is not exactly 1"));
        NodePtr res = nodes_[0]->copy_ptr();
        return res;
    }
};

}
