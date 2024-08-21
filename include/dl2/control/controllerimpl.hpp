#pragma once

#include <cstdint>
#include <cstring>

#include <queue>
#include <string>
#include <utility>

#include "dl/file.hpp"
#include "dl/located.hpp"
#include "dl/word.hpp"
#include "dl/lex/cursor.hpp"
#include "dl/lex/lexer.hpp"
#include "dl/lex/tokendata.hpp"
#include "dl/lex/tokenid.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/parser.hpp"
#include "dl/parse/tokeninfo.hpp"
#include "dl/parse/tokenkind.hpp"
#include "dl/process/opinfo.hpp"
#include "dl/process/opkind.hpp"
#include "dl/process/processor.hpp"
#include "dl/execute/executor.hpp"

namespace dl {

struct ControllerImpl {
    Cursor& cursor;
    Lexer& lexer;
    Parser& parser;
    Processor& processor;
    Executor& executor;
    TokenData token_data;
    std::queue<std::string> word_queue;

    ControllerImpl(
        File& file,
        Lexer& lexer,
        Parser& parser,
        Processor& processor,
        Executor& executor
    ):
    cursor(file),
    lexer(lexer),
    parser(parser),
    processor(processor),
    executor(executor),
    token_data(),
    word_queue() {}

    Word pop_word() {
        Word word = word_queue.front();
        word_queue.pop();
        return word;
    }

    ErrPtr advance_lexer() {
        Pos start = cursor.pos;
        TokenID token = lexer.next(cursor, token_data);
        if (token == TokenID::ERR)
            return token_data.err;
        if (start.col == 1) {
            std::uint32_t space_count =
                token == TokenID::SPACE ? token_data.space_count: 0;
            ErrPtr err = parser.feed_leading_space(
                start, token_data.space_count
            );
            if (err)
                return err;
        }
        if (hasword(token)) {
            word_queue.push_back(token_data.word);
            token_data.word.clear();
        }
        return parser.feed(token, start);
    }

    ErrPtr advance_parser() {
        Located<OpID> op = parser.next();
        while(op.obj == OpID::WAITING) {
            ErrPtr err = advance_lexer();
            if (err)
                return err;
            op = parser.next();
        }
        if (opinfo(op.obj).kind == OpKind::WORD)
            processor.feed_word(op, pop_word());
        else
            processor.feed(op);
        return nullptr;
    }

    ErrPtr advance() {
        Typed node = processor.next();
        while (static_cast<OpID>(node.typeidx) == OpID::WAITING) {
            ErrPtr err = advance_parser();
            if (err)
                return err;
            node = processor.next();
        }
        return executor.execute(node);
    }
}

}
