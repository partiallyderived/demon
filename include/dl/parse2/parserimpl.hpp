#pragma once

namespace dl;

struct ParserImpl: Parser {
    TokenStream& strm;
    Nodes nodes;
    std::uint32_t depth;
    bool in_brackets;

    Res<Sourced<std::uint32_t>> parse_indent() {
        Res<SourcedToken> nxt = strm.get();
        if (nxt.is_err)
            return std::move(nxt.err);
        if (nxt.x.id != TokenID::SPACE) {
            Source src = nxt.src;
            strm.unget(std::move(nxt));
            return Sourced<std::uint32_t>(0, src);
        }
        std::uint32_t num_spaces = std::get<std::uint32_t>(nxt.x.data);
        if (num_spaces % 4 != 0)
            return ErrPtr(new IncompleteIndentErr(nxt.src));
        return num_spaces / 4;
    }

    ErrPtr parse_bracket_indent() {
        Res<SourcedUInt32> indent = parse_indent();
        if (indent.is_err)
            return std::move(indent.err);
        if (indent.res.first < depth)
            return ErrPtr(new UnexpectedDedentErr(indent.res.second));
        return nullptr;
    }

    Res<SourcedToken> next() {
        SourcedToken nxt = strm.get();

        using enum TokenID;
        switch(nxt.first.id) {
        case HASH:
            do
                nxt = strm.next();
            while (nxt.first.id != NEWLINE && nxt.first.id != END_OF_FILE);
            if (nxt.first.id == NEWLINE && in_brackets) {
                Res<SourcedUInt32> indent = parse_indent();
                if (indent.is_err)
                    return std::move(indent.err);
                if (indent.res.first < depth)
                    return ErrPtr(new UnexpectedDedentErr(indent.res.second));
                return next();
            }
            return nxt;
        case SPACE:
            return next();
        case NEWLINE:
            if (in_brackets) {

            }
        default:
            return nxt;
        }
    }

    ErrPtr parse_before() {
        SourcedToken next = strm.next_nonspace();
        auto ops = std::vector<SourcedOp>();
        
        using enum TokenID;
        switch(next.first.id) {
        case AT:
            ops.push_back(SourcedOp(OpID::ADDR, next.second));
            return parse_before();
        case CHAR:
            nodes.push_back(NodePtr(new Char{
                std::get<std::int32_t>(next.first.data)
            }));
            return parse_after();
        case DOT:
        case DOUBLE_STAR:
        case FALSE:
        case ID:
        case LEFT_CURVED:
        case LEFT_SQUARE:
        case MINUS:
        case NONE:
        case NOT:
        case NULL_:
        case NUMBER:
        case STAR:
        case STRING:
        case THIS:
        case TILDE:
        case TRUE:
        case END_OF_FILE:
        case NEWLINE:
        default:
            return ErrPtr(new UnexpectedTokenErr(next));
        }
    }

    NodeRes parse_exprs() {

    }
};
