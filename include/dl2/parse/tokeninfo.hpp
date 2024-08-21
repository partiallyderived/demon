#pragma once

#include <cassert>

#include "dl/lex/tokenid.hpp"
#include "dl/parse/context.hpp"
#include "dl/parse/opid.hpp"
#include "dl/parse/tokenkind.hpp"

namespace dl {

struct TokenInfo {
    TokenKind kind;
    OpID op1; // Associated unary/nullary operator
    OpID op2; // Associated binary operator
    OpID op3; // Associated value operator
    Context match;

    constexpr TokenInfo(
        TokenKind kind, OpID op1, OpID op2, OpID op3, Context match
    ) noexcept:
    kind(kind),
    op1(op1),
    op2(op2),
    op3(op3),
    match() {}

    TokenInfo(TokenKind kind, OpID op) noexcept: TokenInfo(kind) {
        switch(kind) {
        case TokenKind::CONSTRUCT_FIRST:
        case TokenKind::CONSTRUCT_LAST:
        case TokenKind::CONSTRUCT_MIDDLE:
        case TokenKind::NULLARY:
        case TokenKind::RETURN:
        case TokenKind::UNARY:
            op1 = op;
            break;
        case TokenKind::BINARY:
            op2 = op;
            break;
        default:
            assert(false);
        }
    }

    TokenInfo(TokenKind kind, OpID op1, OpID op2) noexcept: TokenKind(kind) {
        switch(kind) {
        case TokenKind::VALUE:
            this->op2 = op1;
            this->op3 = op2;
            break;
        case TokenKind::MULTIARY:
            this->op1 = op1;
            this->op2 = op2;
            break;
        default:
            assert(false);
        }
    }
};

TokenInfo general_unary_info_(TokenKind kind, OpID op) noexcept {
    return TokenInfo(kind, op, OpID::WAITING, OpID::WAITING, Context::BLOCK);
}

TokenInfo binary_info_(OpID op) noexcept {
    return TokenInfo(
        TokenKind::BINARY, OpID::WAITING, op, OpID::WAITING, Context::BLOCK
    );
}

TokenInfo construct_first_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_FIRST, op);
}

TokenInfo construct_first_or_binary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(
        TokenKind::CONSTRUCT_FIRST_OR_BINARY,
        op1,
        op2,
        OpID::WAITING,
        Context::BLOCK
    );
}

TokenInfo construct_last_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_LAST, op);
}

TokenInfo construct_last_or_binary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(
        TokenKind::CONSTRUCT_LAST_OR_BINARY,
        op1,
        op2,
        OpID::WAITING,
        Context::BLOCK
    );
}

TokenInfo construct_middle_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::CONSTRUCT_MIDDLE, op);
}

TokenInfo multiary_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(
        TokenKind::MULTIARY, op1, op2, OpID::WAITING, Context::BLOCK
    );
}

TokenInfo multiary_postfix_info_(OpID op1, OpID op2, OpID op3) noexcept {
    return TokenKind(
        TokenKind::MULTIARY_POSTFIX, op1, op2, op3, Context::BLOCK
    );
}

TokenInfo multiary_value_info_(OpID op1, OpID op2, OpID op3) noexcept {
    return TokenKind(TokenKind::MULTIARY_VALUE, op1, op2, op3, Context::BLOCK);
}

TokenInfo left_info_(OpID op1, OpID op2, Context match)
noexcept {
    return TokenKind(TokenKind::LEFT, OpID::WAITING, op1, op2, match);
}

TokenInfo no_info_(TokenKind kind) noexcept {
    return TokenInfo(
        kind, OpID::WAITING, OpID::WAITING, OpID::WAITING, Context::BLOCK
    );
}

TokenInfo nullary_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::NULLARY, op);
}

TokenInfo right_info_(Context match) noexcept {
    return TokenInfo(
        TokenKind::RIGHT, OpID::WAITING, OpID::WAITING, OpID::WAITING, match
    );
}

TokenInfo unary_info_(OpID op) noexcept {
    return general_unary_info_(TokenKind::UNARY, op);
}

TokenInfo value_info_(OpID op1, OpID op2) noexcept {
    return TokenInfo(TokenKind::VALUE, OpID::WAITING, op1, op2, Context::BLOCK);
}

const TokenInfo TOKEN_INFO_[] = {
    // AMPERSAND
    multiary_postfix_info_(OpID::ADDR, OpID::BAND, OpID::ADDR_TYPE),
    // AMPERSAND_EQUALS
    binary_info_(OpID::IBAND),
    // AND
    binary_info_(OpID::AND),
    // BANG_EQUALS
    binary_info_(OpID::NEQ),
    // BREAK
    nullary_info_(OpID::BREAK),
    // BY
    binary_info_(OpID::BY),
    // CAROT
    binary_info_(OpID::BXOR),
    // CAROT_EQUALS
    binary_info_(OpID::IBXOR),
    // CASE
    construct_middle_info_(OpID::CASE),
    // COLON
    multiary_info_(OpID::EXPR, OpID::LABEL),
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
    // DOUBLE_RIGHT_ANGLE
    binary_info_(OpID::RSH),
    // DOUBLE_RIGHT_ANGLE_EQUALS
    binary_info_(OpID::IRSH),
    // DOUBLE_STAR
    multiary_value_info_(OpID::UNPACK_KWARGS, OpID::EXP, OpID::DOUBLE_STAR),
    // DOUBLE_STAR_EQUALS
    binary_info_(OpID::IEXP),
    // DOT
    multiary_info_(OpID::UP, OpID::GET),
    // ELIF
    construct_middle_info_(OpID::ELIF),
    // ELSE
    construct_last_or_binary_info_(OpID::ELSE, OpID::TERNARY_ELSE),
    // END_OF_FILE
    no_info_(TokenKind::END_OF_FILE),
    // EQUALS
    binary_info_(OpID::SET),
    // ERR
    no_info_(TokenKind::ERR),
    // FALSE
    value_info_(OpID::SUFFIX, OpID::FALSE),
    // FOR
    construct_first_info_(OpID::FOR),
    // FROM
    binary_info_(OpID::FROM),
    // HASH
    no_info_(TokenKind::HASH),
    // HEX
    value_info_(OpID::SUFFIX, OpID::HEX),
    // ID
    value_info_(OpID::SUFFIX, OpID::ID),
    // IF
    construct_first_or_binary_info_(OpID::IF, OpID::TERNARY_IF),
    // IN
    binary_info_(OpID::IN),
    // INT
    value_info_(OpID::SUFFIX, OpID::INT),
    // INTERFACE
    unary_info_(OpID::INTERFACE),
    // LEFT_ANGLE
    binary_info_(OpID::LT),
    // LEFT_ANGLE_EQUALS
    binary_info_(OpID::LTE),
    // LEFT_CURVED
    left_info_(OpID::CALL, OpID::GROUP, Context::CURVED),
    // LEFT_SQUARE
    left_info_(OpID::CALL, OpID::LIST, Context::SQUARE),
    // MATCH
    construct_first_info_(OpID::MATCH),
    // MINUS
    multiary_info_(OpID::NEG, OpID::SUB),
    // MINUS_EQUALS
    binary_info_(OpID::ISUB),
    // MINUS_RIGHT_ANGLE
    binary_info_(OpID::ARROW),
    // MIXED
    value_info_(OpID::SUFFIX, OpID::MIXED),
    // NEWLINE
    no_info_(TokenKind::NEWLINE),
    // NONE
    value_info_(OpID::SUFFIX, OpID::NONE),
    // NOT
    unary_info_(OpID::NOT),
    // NULL
    value_info_(OpID::SUFFIX, OpID::NULL),
    // OCTAL
    value_info_(OpID::SUFFIX, OpID::OCTAL),
    // OR
    binary_info_(OpID::OR),
    // PERCENT
    binary_info_(OpID::MOD),
    // PERCENT_EQUALS
    binary_info_(OpID::IMOD),
    // PIPE
    binary_info_(OpID::BOR),
    // PIPE_EQUALS
    binary_info_(OpID::IBOR),
    // PLUS
    binary_info_(OpID::ADD),
    // PLUS_EQUALS
    binary_info_(OpID::IADD),
    // RAISE
    unary_info_(OpID::RAISE),
    // RIGHT_ANGLE
    binary_info_(OpID::GT),
    // RIGHT_ANGLE_EQUALS
    binary_info_(OpID::GTE),
    // RIGHT_CURVED
    right_info_(Context::CURVED),
    // RIGHT_SQUARE
    right_info_(Context::SQUARE),
    // SLASH
    binary_info_(OpID::DIV),
    // SLASH_EQUALS
    binary_info_(OpID::IDIV),
    // SPACE
    no_info_(TokenKind::SPACE),
    // STAR
    multiary_value_info_(OpID::UNPACK_ARGS, OpID::MUL, OpID::STAR),
    // STAR_EQUALS
    binary_info_(OpID::IMUL),
    // STRING
    value_info_(OpID::CONCAT, OpID::STRING),
    // THIS
    value_info_(OpID::SUFFIX, OpID::THIS),
    // TILDE
    unary_info_(OpID::BNOT),
    // TO
    binary_info_(OpID::TO),
    // TRUE
    value_info_(OpID::SUFFIX, OpID::TRUE),
    // TYPE
    unary_info_(OpID::TYPE),
    // TYPE_INTERFACE
    unary_info_(OpID::TYPE_INTERFACE),
    // VARS
    value_info_(OpID::SUFFIX, OpID::VARS)
}

const TokenInfo& tokeninfo(TokenID id) {
    return TOKEN_INFO_[static_cast<int>(id)];
}

}
