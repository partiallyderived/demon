#pragma once

#include <cassert>

#include "dl/lex/tokenid.hpp"
#include "dl/parse/context.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/tokenkind.hpp"

namespace dl {

struct TokenInfo {
    TokenKind kind;

    // Associated unary/nullary/value operator.
    // OpID::WAITING is used to indicate absence of this operator.
    OpID op1;

    // Associated binary operator.
    // OpID::WAITING is used to indicate absence of this operator.
    OpID op2; 
    
    // The matching context for lefts and rights.
    // Context::BLOCK is used to indicate that no match applies.
    Context match;

    constexpr TokenInfo(TokenKind kind, OpID op1, OpID op2, Context match)
    noexcept: kind(kind), op1(op1), op2(op2), match(match) {}
};

TokenInfo general_unary_info_(TokenKind kind, OpID op) noexcept {
    return TokenInfo(kind, op, OpID::WAITING, Context::BLOCK);
}

TokenInfo binary_info_(OpID op) noexcept {
    return TokenInfo(TokenKind::BINARY, OpID::WAITING, op, Context::BLOCK);
}

TokenInfo construct_first_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_FIRST, op);
}

TokenInfo construct_first_or_binary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(
        TokenKind::CONSTRUCT_FIRST_OR_BINARY, op1, op2, Context::BLOCK
    );
} 

TokenInfo construct_last_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_LAST, op);
}

TokenInfo construct_last_or_binary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(
        TokenKind::CONSTRUCT_LAST_OR_BINARY, op1, op2, Context::BLOCK
    );
}

TokenInfo construct_middle_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_MIDDLE, op);
}

TokenInfo dual_affix_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(TokenKind::DUAL_AFFIX, op1, op2, Context::BLOCK);
}

TokenInfo multiary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(TokenKind::MULTIARY, op1, op2, Context::BLOCK);
}

TokenInfo left_info_(OpID op1, OpID op2, Context match)
noexcept {
    return TokenInfo(TokenKind::LEFT, op1, op2, match);
}

TokenInfo no_info_() noexcept {
    return TokenInfo(
        TokenKind::SPECIAL, OpID::WAITING, OpID::WAITING, Context::BLOCK
    );
}

TokenInfo nullary_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::NULLARY, op);
}

TokenInfo optional_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::OPTIONAL_STMT, op);
}

TokenInfo right_info_(Context match) noexcept {
    return TokenInfo(TokenKind::RIGHT, OpID::WAITING, OpID::WAITING, match);
}

TokenInfo unary_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::UNARY, op);
}

TokenInfo value_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(TokenKind::VALUE, op1, op2, Context::BLOCK);
}

const TokenInfo TOKEN_INFO_[static_cast<int>(TokenID::WHILE) + 1] = {
    // AMPERSAND
    binary_info_(OpID::BAND),
    // AMPERSAND_EQUALS
    binary_info_(OpID::IBAND),
    // AND
    binary_info_(OpID::AND),
    // AS
    binary_info_(OpID::AS),
    // AT
    dual_affix_info_(OpID::ADDR, OpID::ADDR_TYPE),
    // BANG_EQUALS
    binary_info_(OpID::NEQ),
    // BREAK
    nullary_info_(OpID::BREAK),
    // CAROT
    binary_info_(OpID::BXOR),
    // CAROT_EQUALS
    binary_info_(OpID::IBXOR),
    // CASE
    construct_middle_info_(OpID::CASE),
    // CHAR
    value_info_(OpID::CHAR, OpID::CONCAT),
    // COLON
    multiary_info_(OpID::SYMBOL, OpID::TYPE_LABEL),
    // COMMA
    binary_info_(OpID::SEP),
    // CONTINUE
    nullary_info_(OpID::CONTINUE),
    // DEF
    construct_first_info_(OpID::DEF),
    // DOUBLE_EQUALS
    binary_info_(OpID::EQ),
    // DOUBLE_LEFT_ANGLE
    binary_info_(OpID::LSH),
    // DOUBLE_LEFT_ANGLE_EQUALS
    binary_info_(OpID::ILSH),
    // DOUBLE_PIPE
    binary_info_(OpID::MATCH_OR),
    // DOUBLE_RIGHT_ANGLE
    binary_info_(OpID::RSH),
    // DOUBLE_RIGHT_ANGLE_EQUALS
    binary_info_(OpID::IRSH),
    // DOUBLE_STAR
    multiary_info_(OpID::UNPACK_KWARGS, OpID::POW),
    // DOUBLE_STAR_EQUALS
    binary_info_(OpID::IPOW),
    // DOT
    multiary_info_(OpID::UP, OpID::GET),
    // ELIF
    construct_middle_info_(OpID::ELIF),
    // ELSE
    construct_last_or_binary_info_(OpID::ELSE, OpID::TERNARY_ELSE),
    // END_OF_FILE
    no_info_(),
    // EQUALS
    binary_info_(OpID::SET),
    // EXCEPT
    construct_middle_info_(OpID::EXCEPT),
    // FALSE
    value_info_(OpID::FALSE, OpID::SUFFIX),
    // FINALLY
    construct_last_info_(OpID::FINALLY),
    // FLOAT_TAIL
    value_info_(OpID::FLOAT_TAIL, OpID::SUFFIX),
    // FOR
    construct_first_info_(OpID::FOR),
    // HASH
    no_info_(),
    // ID
    value_info_(OpID::ID, OpID::SUFFIX),
    // IF
    construct_first_or_binary_info_(OpID::IF, OpID::TERNARY_IF),
    // IN
    binary_info_(OpID::IN),
    // LEFT_ANGLE
    binary_info_(OpID::LT),
    // LEFT_ANGLE_EQUALS
    binary_info_(OpID::LTE),
    // LEFT_CURLY
    left_info_(OpID::ENCLOSURE, OpID::CALL, Context::CURLY),
    // LEFT_CURVED
    left_info_(OpID::GROUP, OpID::CALL, Context::CURVED),
    // LEFT_SQUARE
    left_info_(OpID::LIST, OpID::CALL, Context::SQUARE),
    // MATCH
    construct_first_info_(OpID::MATCH),
    // MINUS
    multiary_info_(OpID::NEG, OpID::SUB),
    // MINUS_EQUALS
    binary_info_(OpID::ISUB),
    // MINUS_RIGHT_ANGLE
    binary_info_(OpID::RETURNS),
    // NEWLINE
    no_info_(),
    // NONE
    value_info_(OpID::NONE, OpID::SUFFIX),
    // NOT
    unary_info_(OpID::NOT),
    // NULL
    value_info_(OpID::NULL_, OpID::SUFFIX),
    // NUMBER
    value_info_(OpID::NUMBER, OpID::SUFFIX),
    // OR
    binary_info_(OpID::OR),
    // PERCENT
    multiary_info_(OpID::LAMBDA, OpID::MOD),
    // PERCENT_DOUBLE_STAR
    value_info_(OpID::LAMBDA_KWARGS, OpID::SUFFIX),
    // PERCENT_EQUALS
    binary_info_(OpID::IMOD),
    // PERCENT_STAR
    value_info_(OpID::LAMBDA_ARGS, OpID::SUFFIX),
    // PIPE
    multiary_info_(OpID::LITERALLY, OpID::BOR),
    // PIPE_EQUALS
    binary_info_(OpID::IBOR),
    // PLACEHOLDER
    value_info_(OpID::PLACEHOLDER, OpID::SUFFIX),
    // PLAIN_INT
    value_info_(OpID::PLAIN_INT, OpID::SUFFIX),
    // PLUS
    binary_info_(OpID::ADD),
    // PLUS_EQUALS
    binary_info_(OpID::IADD),
    // RAISE
    optional_info_(OpID::RAISE),
    // RETURN
    optional_info_(OpID::RETURN),
    // RIGHT_ANGLE
    binary_info_(OpID::GT),
    // RIGHT_ANGLE_EQUALS
    binary_info_(OpID::GTE),
    // RIGHT_CURLY
    right_info_(Context::CURLY),
    // RIGHT_CURVED
    right_info_(Context::CURVED),
    // RIGHT_SQUARE
    right_info_(Context::SQUARE),
    // SLASH
    binary_info_(OpID::DIV),
    // SLASH_EQUALS
    binary_info_(OpID::IDIV),
    // SPACE
    no_info_(),
    // STAR
    multiary_info_(OpID::UNPACK_ARGS, OpID::MUL),
    // STAR_EQUALS
    binary_info_(OpID::IMUL),
    // STRING
    value_info_(OpID::STRING, OpID::CONCAT),
    // THIS
    value_info_(OpID::THIS, OpID::SUFFIX),
    // TILDE
    unary_info_(OpID::BNOT),
    // TRUE
    value_info_(OpID::TRUE, OpID::SUFFIX),
    // TRY
    construct_first_info_(OpID::TRY),
    // TYPE
    construct_first_info_(OpID::TYPE),
    // VARS
    value_info_(OpID::VARS, OpID::SUFFIX),
    // WHILE
    construct_first_info_(OpID::WHILE)
};

const TokenInfo& tokeninfo(TokenID id) {
    return TOKEN_INFO_[static_cast<int>(id)];
}

}
