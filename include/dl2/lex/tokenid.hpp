#pragma once

#include <ostream>

namespace dl {

// The different unique tokens that may be lexxed, excluding their associated
// data, such as the content of a string.
enum class TokenID {
    ALNUM,
    AMPERSAND,
    AMPERSAND_EQUALS,
    AND,
    BANG_EQUALS,
    BREAK,
    BY,
    CAROT,
    CAROT_EQUALS,
    CASE,
    COLON,
    COMMA,
    CONTINUE,
    DEF,
    DOUBLE_EQUALS,
    DOUBLE_LEFT_ANGLE,
    DOUBLE_LEFT_ANGLE_EQUALS,
    DOUBLE_RIGHT_ANGLE,
    DOUBLE_RIGHT_ANGLE_EQUALS,
    DOUBLE_STAR,
    DOUBLE_STAR_EQUALS,
    DOT,
    ELIF,
    ELSE,
    END_OF_FILE,
    EQUALS,
    FALSE,
    FOR,
    FROM,
    HASH,
    IF,
    IN,
    INTERFACE,
    LEFT_ANGLE,
    LEFT_ANGLE_EQUALS,
    LEFT_CURVED,
    LEFT_SQUARE,
    MATCH,
    MINUS,
    MINUS_EQUALS,
    MINUS_RIGHT_ANGLE,
    NEWLINE,
    NONE,
    NOT,
    NULL,
    OR,
    PERCENT,
    PERCENT_EQUALS,
    PIPE,
    PIPE_EQUALS,
    PLUS,
    PLUS_EQUALS,
    RAISE,
    RIGHT_ANGLE,
    RIGHT_ANGLE_EQUALS,
    RIGHT_CURVED,
    RIGHT_SQUARE,
    SLASH,
    SLASH_EQUALS,
    SPACE,
    STAR,
    STAR_EQUALS,
    STRING,
    THIS,
    TILDE,
    TO,
    TRUE,
    TYPE,
    TYPE_INTERFACE,
    VARS
};

std::ostream& operator<<(std::ostream& os, TokenID id) {
    using enum TokenID;
    switch(id) {
    case ALNUM:
        return os << "ALNUM";
    case AMPERSAND:
        return os << "AMPERSAND";
    case AMPERSAND_EQUALS:
        return os << "AMPERSAND_EQUALS";
    case AND:
        return os << "AND";
    case BANG_EQUALS:
        return os << "BANG_EQUALS";
    case BREAK:
        return os << "BREAK";
    case BY:
        return os << "BY";
    case CAROT:
        return os << "CAROT";
    case CAROT_EQUALS:
        return os << "CAROT_EQUALS";
    case CASE:
        return os << "CASE";
    case COLON:
        return os << "COLON";
    case COMMA:
        return os << "COMMA";
    case CONTINUE:
        return os << "CONTINUE";
    case DEF:
        return os << "DEF";
    case DOUBLE_EQUALS:
        return os << "DOUBLE_EQUALS";
    case DOUBLE_LEFT_ANGLE:
        return os << "DOUBLE_LEFT_ANGLE";
    case DOUBLE_LEFT_ANGLE_EQUALS:
        return os << "DOUBLE_LEFT_ANGLE_EQUALS";
    case DOUBLE_RIGHT_ANGLE:
        return os << "DOUBLE_RIGHT_ANGLE";
    case DOUBLE_RIGHT_ANGLE_EQUALS:
        return os << "DOUBLE_RIGHT_ANGLE_EQUALS";
    case DOUBLE_STAR:
        return os << "DOUBLE_STAR";
    case DOUBLE_STAR_EQUALS:
        return os << "DOUBLE_STAR_EQUALS";
    case DOT:
        return os << "DOT";
    case ELIF:
        return os << "ELIF";
    case ELSE:
        return os << "ELSE";
    case END_OF_FILE:
        return os << "END_OF_FILE";
    case EQUALS:
        return os << "EQUALS";
    case FALSE:
        return os << "FALSE";
    case FOR:
        return os << "FOR";
    case FROM:
        return os << "FROM";
    case HASH:
        return os << "HASH";
    case IF:
        return os << "IF";
    case IN:
        return os << "IN";
    case INTERFACE:
        return os << "INTERFACE";
    case LEFT_ANGLE:
        return os << "LEFT_ANGLE";
    case LEFT_ANGLE_EQUALS:
        return os << "LEFT_ANGLE_EQUALS";
    case LEFT_CURVED:
        return os << "LEFT_CURVED";
    case LEFT_SQUARE:
        return os << "LEFT_SQUARE";
    case MATCH:
        return os << "MATCH";
    case MINUS:
        return os << "MINUS";
    case MINUS_EQUALS:
        return os << "MINUS_EQUALS";
    case MINUS_RIGHT_ANGLE:
        return os << "MINUS_RIGHT_ANGLE";
    case MIXED:
        return os << "MIXED";
    case NEWLINE:
        return os << "NEWLINE";
    case NONE:
        return os << "NONE";
    case NOT:
        return os << "NOT";
    case NULL:
        return os << "NULL";
    case OR:
        return os << "OR";
    case PERCENT:
        return os << "PERCENT";
    case PERCENT_EQUALS:
        return os << "PERCENT_EQUALS";
    case PIPE:
        return os << "PIPE";
    case PIPE_EQUALS:
        return os << "PIPE_EQUALS";
    case PLUS:
        return os << "PLUS";
    case PLUS_EQUALS:
        return os << "PLUS_EQUALS";
    case RAISE:
        return os << "RAISE";
    case RIGHT_ANGLE:
        return os << "RIGHT_ANGLE";
    case RIGHT_ANGLE_EQUALS:
        return os << "RIGHT_ANGLE_EQUALS";
    case RIGHT_CURVED:
        return os << "RIGHT_CURVED";
    case RIGHT_SQUARE:
        return os << "RIGHT_SQUARE";
    case SLASH:
        return os << "SLASH";
    case SLASH_EQUALS:
        return os << "SLASH_EQUALS";
    case SPACE:
        return os << "SPACE";
    case STAR:
        return os << "STAR";
    case STAR_EQUALS:
        return os << "STAR_EQUALS";
    case STRING:
        return os << "STRING";
    case THIS:
        return os << "THIS";
    case TILDE:
        return os << "TILDE";
    case TO:
        return os << "TO";
    case TRUE:
        return os << "TRUE";
    case TYPE:
        return os << "TYPE";
    case TYPE_INTERFACE:
        return os << "TYPE_INTERFACE";
    case VARS:
        return os << "VARS";
}

}
